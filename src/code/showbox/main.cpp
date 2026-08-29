#include <CLIBuilder.h>
#include <ParserMain.h>

#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDialog>
#include <QLayout>
#include <QSocketNotifier>
#include <QTimer>

#include <cerrno>
#include <fcntl.h>
#include <unistd.h>

namespace {
void processInput(QByteArray &buffer, ParserMain &parser) {
    qsizetype newline = -1;
    while ((newline = buffer.indexOf('\n')) >= 0) {
        QByteArray line = buffer.left(newline);
        buffer.remove(0, newline + 1);
        if (line.endsWith('\r'))
            line.chop(1);
        parser.processLine(QString::fromUtf8(line));
    }
}
} // namespace

int main(int argc, char *argv[]) {
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    QApplication app(argc, argv);
    QApplication::setApplicationName("showbox");
    QApplication::setApplicationVersion("1.0.0");

    QCommandLineParser commandLine;
    commandLine.setApplicationDescription(
        "Translate commands on stdin into Qt 6 widgets and report events on stdout.");
    commandLine.addHelpOption();
    commandLine.addVersionOption();
    const QCommandLineOption hiddenOption({"d", "hidden"},
                                          "Wait for an explicit show command.");
    const QCommandLineOption resizableOption({"r", "resizable"},
                                             "Allow the dialog to be resized.");
    const QCommandLineOption newParserOption(
        "new-parser", "Compatibility option; the modern parser is always active.");
    commandLine.addOption(hiddenOption);
    commandLine.addOption(resizableOption);
    commandLine.addOption(newParserOption);
    commandLine.process(app);

    CLIBuilder builder;
    ParserMain parser(&builder);
    parser.setRootWidget(builder.window());

    QObject::connect(&parser, &ParserMain::showRequested, &builder,
                     &CLIBuilder::show);
    QObject::connect(qobject_cast<QDialog *>(builder.window()), &QDialog::finished,
                     &app, [&app](int result) { app.exit(result); });

    QByteArray inputBuffer;
    const int oldFlags = fcntl(STDIN_FILENO, F_GETFL, 0);
    if (oldFlags >= 0)
        fcntl(STDIN_FILENO, F_SETFL, oldFlags | O_NONBLOCK);

    QSocketNotifier stdinNotifier(STDIN_FILENO, QSocketNotifier::Read, &app);
    QObject::connect(&stdinNotifier, &QSocketNotifier::activated, &app,
                     [&](QSocketDescriptor, QSocketNotifier::Type) {
        char chunk[4096];
        for (;;) {
            const ssize_t count = read(STDIN_FILENO, chunk, sizeof(chunk));
            if (count > 0) {
                inputBuffer.append(chunk, count);
                processInput(inputBuffer, parser);
                continue;
            }
            if (count == 0) {
                stdinNotifier.setEnabled(false);
                if (!inputBuffer.isEmpty()) {
                    parser.processLine(QString::fromUtf8(inputBuffer));
                    inputBuffer.clear();
                }
            }
            if (count < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
                stdinNotifier.setEnabled(false);
            break;
        }
    });

    if (!commandLine.isSet(hiddenOption))
        QTimer::singleShot(0, &builder, &CLIBuilder::show);

    if (!commandLine.isSet(resizableOption)) {
        builder.window()->layout()->setSizeConstraint(QLayout::SetFixedSize);
    }

    return app.exec();
}

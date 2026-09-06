// Harness Qt: recompila a entrada de produção sem adicionar opções ao showbox.
#define main showboxProductionMain
#include "../../runtime/main.cpp"
#undef main
#include <QFile>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QtTest>

static void installDriver() {
    QTimer::singleShot(0, QCoreApplication::instance(), [] {
        auto *timer = new QTimer(qApp);
        timer->setInterval(30);
        auto ticks = std::make_shared<int>(0);
        auto clicked = std::make_shared<bool>(false);
        QObject::connect(timer, &QTimer::timeout, qApp, [timer, ticks, clicked] {
            if (++*ticks > 200) { qApp->exit(91); return; }
            for (QWidget *window : QApplication::topLevelWidgets()) {
                auto *button = window->findChild<QPushButton *>("run");
                auto *entryWidget = window->findChild<QWidget *>("entry");
                auto *entry = entryWidget ? qobject_cast<QLineEdit *>(entryWidget->focusProxy() ? entryWidget->focusProxy() : entryWidget) : nullptr;
                auto *label = window->findChild<QLabel *>("result");
                if (!button || !entry || !label || !window->isVisible()) continue;
                if (!*clicked) {
                    entry->setText(qEnvironmentVariable("SB_TEST_INPUT"));
                    QTest::mouseClick(button, Qt::LeftButton);
                    *clicked = true;
                    if (qEnvironmentVariable("SB_TEST_MODE") == "queued") {
                        if (auto *second = window->findChild<QPushButton *>("second"))
                            QTimer::singleShot(50, second, [second] { QTest::mouseClick(second, Qt::LeftButton); });
                    }
                }
                const QString mode = qEnvironmentVariable("SB_TEST_MODE");
                bool success = mode == "visual" && *ticks > 8;
                if (mode == "normal" || mode == "queued") success = label->text() == qEnvironmentVariable("SB_TEST_EXPECTED");
                if (mode == "close-running") success = QFile::exists(qEnvironmentVariable("SB_CHILD_PID_FILE"));
                if (success) {
                    QFile marker(qEnvironmentVariable("SB_RESULT_FILE"));
                    if (!marker.open(QIODevice::WriteOnly)) { qApp->exit(92); return; }
                    marker.write(label->text().toUtf8());
                    marker.close();
                    timer->stop();
                    if (!qEnvironmentVariable("SB_SCREENSHOT_FILE").isEmpty())
                        window->grab().save(qEnvironmentVariable("SB_SCREENSHOT_FILE"));
                    qApp->quit();
                }
            }
        });
        timer->start();
    });
}
Q_COREAPP_STARTUP_FUNCTION(installDriver)

int main(int argc, char **argv) { return showboxProductionMain(argc, argv); }

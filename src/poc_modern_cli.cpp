#include <QApplication>
#include <QTimer>
#include <ShowboxBuilder.h>
#include <CLIBuilder.h>
#include <ParserMain.h>
#include <QThread>

class InputThread : public QThread {
    ParserMain *parser;
public:
    InputThread(ParserMain *p) : parser(p) {}
    void run() override {
        parser->run();
        QMetaObject::invokeMethod(qApp, "quit");
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Use CLIBuilder which implements the IShowboxBuilder interface
    // but wraps the logic. Or use ShowboxBuilder directly if it's sufficient.
    // ParserMain expects IShowboxBuilder.
    // However, ShowboxBuilder returns QWidget*, ParserMain needs to manage them.
    // Wait, ParserMain logic assumes IShowboxBuilder returns QWidgets and handles parenting itself.
    // ShowboxBuilder is perfect for this.
    
    ShowboxBuilder builder;
    ParserMain parser(&builder);

    // Connect signals if needed, e.g. showRequested
    // But ParserMain handles show internally or emits it?
    // Current ParserMain implementation emits showRequested(), 
    // but usually in V1 'show' makes a specific widget visible.
    // Let's just run it.

    // Run parser in a separate thread to not block the GUI event loop
    // reading from stdin is blocking.
    InputThread thread(&parser);
    thread.start();

    return app.exec();
}

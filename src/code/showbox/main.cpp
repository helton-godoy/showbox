#include <QApplication>
#include <CLIBuilder.h>
#include <ParserMain.h>
#include <thread>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    CLIBuilder builder;
    ParserMain parser(&builder);

    // Simple connection to quit when stdin ends or special command
    QObject::connect(&parser, &ParserMain::showRequested, [&app]() {
        // In a real scenario, this would trigger the window display
        std::cout << "Show requested" << std::endl;
    });

    std::thread parserThread([&parser]() {
        parser.run();
        // Option: emit a finished signal to quit app
    });
    parserThread.detach();

    return app.exec();
}

#include <CLIBuilder.h>
#include <ParserMain.h>
#include <QApplication>
#include <iostream>
#include <thread>

int main(int argc, char *argv[]) {
  QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
      Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
  QApplication app(argc, argv);

  CLIBuilder builder;
  ParserMain parser(&builder);

  // Simple connection to quit when stdin ends or special command
  QObject::connect(&parser, &ParserMain::showRequested, [&builder]() {
    // Mostrar a janela construída pelo builder
    builder.show();
  });

  std::thread parserThread([&parser]() {
    parser.run();
    // Option: emit a finished signal to quit app
  });
  parserThread.detach();

  return app.exec();
}

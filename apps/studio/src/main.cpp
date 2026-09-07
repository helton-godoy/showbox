#include "gui/MainWindow.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QCoreApplication>

int main(int argc, char *argv[]) {
  QCoreApplication::setApplicationName("showbox-studio");
  QCoreApplication::setApplicationVersion("1.0.0");

  {
    QCoreApplication core(argc, argv);
    QCommandLineParser commandLine;
    commandLine.setApplicationDescription("Visual editor for ShowBox interfaces.");
    commandLine.addHelpOption();
    commandLine.addVersionOption();
    commandLine.process(core);
  }

  QApplication a(argc, argv);
  MainWindow w;
  w.show();

  return a.exec();
}

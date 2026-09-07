#include "gui/MainWindow.h"
#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  QApplication::setApplicationName("showbox-studio");
  QApplication::setApplicationVersion("1.0.0");

  QCommandLineParser commandLine;
  commandLine.setApplicationDescription("Visual editor for ShowBox interfaces.");
  commandLine.addHelpOption();
  commandLine.addVersionOption();
  commandLine.process(a);

  MainWindow w;
  w.show();

  return a.exec();
}

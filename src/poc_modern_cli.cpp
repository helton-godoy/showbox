#include <CLIBuilder.h>
#include <ParserMain.h>
#include <QApplication>
#include <QThread>
#include <QTimer>
#include <QWidget>
#include <ShowboxBuilder.h>
#include <iostream>
#include <string>

// Thread dedicada apenas para ler do STDIN sem bloquear a GUI
class StdinReader : public QThread {
  Q_OBJECT
signals:
  void lineRead(const QString &line);
  void finishedReading();

public:
  void run() override {
    std::string line;
    while (std::getline(std::cin, line)) {
      emit lineRead(QString::fromStdString(line));
    }
    emit finishedReading();
  }
};

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  bool keepOpen = false;
  for (int i = 1; i < argc; ++i) {
    if (QString(argv[i]) == "--keep-open") {
      keepOpen = true;
    }
  }

  ShowboxBuilder builder;
  ParserMain parser(&builder);

  // Conectar showRequested para mostrar janelas
  QObject::connect(&parser, &ParserMain::showRequested, [&]() {
    for (QWidget *widget : QApplication::topLevelWidgets()) {
      if (!widget->isHidden()) {
        widget->raise();
        widget->activateWindow();
      } else {
        widget->show();
      }
    }
  });

  StdinReader reader;

  // Conectar leitura (Thread) -> Processamento (Main Thread)
  // O Qt::QueuedConnection (padrão entre threads) garante que o lambda
  // rode na thread principal, onde 'parser' vive.
  QObject::connect(&reader, &StdinReader::lineRead, &parser,
                   [&](const QString &line) {
                     QString qline = line.trimmed();
                     if (!qline.isEmpty() && !qline.startsWith("#")) {
                       parser.processLine(qline);
                     }
                   });

  QObject::connect(&reader, &StdinReader::finishedReading, [&]() {
    if (!keepOpen) {
      app.quit();
    }
  });

  reader.start();

  int ret = app.exec();
  reader.wait(); // Join thread to avoid destructive crash
  return ret;
}

#include "poc_modern_cli.moc"

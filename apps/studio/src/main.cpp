#include "gui/MainWindow.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QCoreApplication>
#include "automation/StudioAutomationServer.h"
#include <unistd.h>

int main(int argc, char *argv[]) {
  QCoreApplication::setApplicationName("showbox-studio");
  QCoreApplication::setApplicationVersion(SHOWBOX_VERSION);

  bool automation = false;
  bool automationReadOnly = false;
  bool automationAllowExecution = false;
  QString automationSocket;
  {
    QCoreApplication core(argc, argv);
    QCommandLineParser commandLine;
    commandLine.setApplicationDescription("Visual editor for ShowBox interfaces.");
    commandLine.addHelpOption();
    commandLine.addVersionOption();
    commandLine.addOption({"automation", "Ativa a Showbox Studio Automation Interface."});
    commandLine.addOption({"automation-socket", "Nome/caminho do socket local.", "socket"});
    commandLine.addOption({"automation-read-only", "Recusa mutações pela interface de automação."});
    commandLine.addOption({"automation-allow-execution", "Permite iniciar execução Bash pela automação."});
    commandLine.process(core);
    automation = commandLine.isSet("automation");
    automationReadOnly = commandLine.isSet("automation-read-only");
    automationAllowExecution = commandLine.isSet("automation-allow-execution");
    automationSocket = commandLine.value("automation-socket");
  }

  QApplication a(argc, argv);
  MainWindow w;
  w.show();

  StudioAutomationServer *server = nullptr;
  if (automation) {
    if (automationSocket.isEmpty())
      automationSocket = QString("showbox-studio-%1").arg(static_cast<uint>(::getuid()));
    server = new StudioAutomationServer(&w, automationReadOnly,
                                         automationAllowExecution, &w);
    QString error;
    if (!server->listen(automationSocket, &error)) {
      qCritical().noquote() << "Não foi possível ativar a automação:" << error;
      return 2;
    }
    qInfo().noquote() << "Showbox Studio Automation Interface ouvindo em"
                      << server->serverName();
  }

  return a.exec();
}

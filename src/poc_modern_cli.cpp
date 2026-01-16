#include <QApplication>
#include <QTimer>
#include <ShowboxBuilder.h>
#include <WidgetConfigs.h>
#include <QLayout>
#include <QWidget>

using namespace Showbox::Models;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    ShowboxBuilder builder;

    // 1. Criar a Janela Principal
    WindowConfig winConfig;
    winConfig.title = "Showbox Modern CLI PoC";
    winConfig.width = 600;
    winConfig.height = 400;
    QWidget *window = builder.buildWindow(winConfig);

    // 2. Configurar o Layout Vertical Principal
    // buildWindow já cria um QVBoxLayout padrão.
    QLayout *layout = window->layout();
    if (layout) {
        layout->setSpacing(10);
        layout->setContentsMargins(20, 20, 20, 20);
    }

    // 3. Adicionar Widgets
    LabelConfig lblConfig;
    lblConfig.text = "<b>Bem-vindo à Nova Arquitetura Showbox</b>";
    layout->addWidget(builder.buildLabel(lblConfig));

    LineEditConfig leConfig;
    leConfig.placeholder = "Digite algo aqui...";
    layout->addWidget(builder.buildLineEdit(leConfig));

    ButtonConfig btnConfig;
    btnConfig.text = "Executar Ação";
    btnConfig.name = "btn_action";
    layout->addWidget(builder.buildButton(btnConfig));
    
    ProgressBarConfig pbConfig;
    pbConfig.value = 75;
    layout->addWidget(builder.buildProgressBar(pbConfig));

    // O PoC deve fechar após 2 segundos se em modo CI para não travar o build
    if (qEnvironmentVariableIsSet("CI")) {
        QObject::connect(&app, &QApplication::aboutToQuit, [](){});
        QTimer::singleShot(2000, &app, &QApplication::quit);
    }

    window->show();
    return app.exec();
}

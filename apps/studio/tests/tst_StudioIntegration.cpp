#include <QtTest>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QCheckBox>

// Incluir classes do Core e GUI para testar a integração
#include "core/StudioWidgetFactory.h"
#include "core/StudioController.h"
#include "gui/Canvas.h"

class tst_StudioIntegration : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testWidgetCreation();
    void testPropertyReflection();
    void testCanvasSelection();

private:
    StudioWidgetFactory *m_factory;
    Canvas *m_canvas;
    StudioController *m_controller;
};

void tst_StudioIntegration::initTestCase()
{
    m_factory = new StudioWidgetFactory();
    // Canvas precisa de um widget pai em testes visuais geralmente, mas para lógica não.
    m_canvas = new Canvas(m_factory);
    m_controller = new StudioController(this);
    
    // Conectar como na MainWindow
    connect(m_canvas, &Canvas::widgetSelected, m_controller, &StudioController::selectWidget);
}

void tst_StudioIntegration::cleanupTestCase()
{
    delete m_canvas;
    delete m_factory;
}

void tst_StudioIntegration::testWidgetCreation()
{
    // Simular Drag & Drop (via código direto na Factory)
    QWidget *btn = m_factory->createWidget("PushButton", "btn_login");
    QVERIFY(btn != nullptr);
    QCOMPARE(btn->objectName(), "btn_login");
    
    // Verificar se é um botão real (ou wrapper)
    // No ShowboxBuilder atual, ele retorna QWidgets configurados.
    // O botão deve ter texto "Button" por padrão segundo a Factory
    
    // Nota: Dependendo da implementação do Builder, pode ser um QPushButton ou custom.
    // Vamos verificar via propriedade 'text' se existir
    QVariant textProp = btn->property("text");
    if (textProp.isValid()) {
        QCOMPARE(textProp.toString(), "Button");
    }
    
    m_canvas->addWidget(btn);
}

void tst_StudioIntegration::testPropertyReflection()
{
    // Criar um CheckBox
    QWidget *chk = m_factory->createWidget("CheckBox", "chk_remember");
    m_canvas->addWidget(chk);
    
    // Simular PropertyEditor alterando valor
    // 1. Ler meta-objeto
    const QMetaObject *meta = chk->metaObject();
    int idx = meta->indexOfProperty("checked");
    QVERIFY(idx != -1);
    
    // 2. Alterar propriedade via sistema de reflexão (como o PropertyEditor faz)
    bool success = chk->setProperty("checked", true);
    QVERIFY(success);
    
    // 3. Verificar se o widget atualizou
    QVariant val = chk->property("checked");
    QVERIFY(val.toBool() == true);
}

void tst_StudioIntegration::testCanvasSelection()
{
    QWidget *lbl = m_factory->createWidget("Label", "lbl_title");
    m_canvas->addWidget(lbl);
    
    QSignalSpy spy(m_controller, &StudioController::widgetSelected);
    
    // Simular seleção vinda do Canvas
    // Como o Canvas emite sinal interno, vamos chamar o método público (se houver) 
    // ou simular via Controller diretamente, já que o Canvas delega pro Controller agora.
    
    // O Controller deve filtrar eventos. Vamos simular um clique?
    // Difícil simular clique em teste unitário sem janela visível.
    // Vamos chamar o selectWidget diretamente no Controller para testar a lógica de seleção.
    
    m_controller->selectWidget(lbl);
    
    QCOMPARE(spy.count(), 1);
    QWidget *selected = spy.takeFirst().at(0).value<QWidget*>();
    QCOMPARE(selected, lbl);
    
    // Verificar se a propriedade dinâmica foi setada (para styling via qss)
    QVERIFY(lbl->property("selected").toBool() == true);
}

QTEST_MAIN(tst_StudioIntegration)
#include "tst_StudioIntegration.moc"

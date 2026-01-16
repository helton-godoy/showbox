#include <QtTest>
#include <ShowboxBuilder.h>
#include <WidgetConfigs.h>
#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>

class TestShowboxBuilder : public QObject
{
    Q_OBJECT

private slots:
    void testCreateWindow();
    void testBuildLayout();
    void testBuildControls();
    void testPerformance();
};

void TestShowboxBuilder::testCreateWindow()
{
    ShowboxBuilder builder;
    Showbox::Models::WindowConfig config;
    config.title = "Test Window";
    config.width = 400;
    config.height = 300;
    
    QWidget* window = builder.buildWindow(config);
    
    QVERIFY(window != nullptr);
    QCOMPARE(window->windowTitle(), QString("Test Window"));
    // Note: window geometry might not be exactly 400x300 until shown, 
    // but resize() should set the values.
    QCOMPARE(window->width(), 400);
    QCOMPARE(window->height(), 300);
    
    delete window;
}

void TestShowboxBuilder::testBuildLayout()
{
    ShowboxBuilder builder;
    Showbox::Models::LayoutConfig config;
    config.type = Showbox::Models::LayoutConfig::HBox;
    config.spacing = 10;
    config.margin = 15;
    
    QLayout* layout = builder.buildLayout(config);
    
    QVERIFY(layout != nullptr);
    QCOMPARE(layout->spacing(), 10);
    QCOMPARE(layout->contentsMargins().left(), 15);
    QVERIFY(qobject_cast<QHBoxLayout*>(layout) != nullptr);
    
    delete layout;
}

void TestShowboxBuilder::testBuildControls()
{
    ShowboxBuilder builder;
    
    // Test Button
    Showbox::Models::ButtonConfig btnConfig;
    btnConfig.name = "btn1";
    btnConfig.text = "Click Me";
    QWidget* btn = builder.buildButton(btnConfig);
    QVERIFY(btn != nullptr);
    QCOMPARE(qobject_cast<QPushButton*>(btn)->text(), QString("Click Me"));
    delete btn;
    
    // Test Label
    Showbox::Models::LabelConfig lblConfig;
    lblConfig.name = "lbl1";
    lblConfig.text = "Hello World";
    QWidget* lbl = builder.buildLabel(lblConfig);
    QVERIFY(lbl != nullptr);
    QCOMPARE(qobject_cast<QLabel*>(lbl)->text(), QString("Hello World"));
    delete lbl;
    
    // Test LineEdit
    Showbox::Models::LineEditConfig leConfig;
    leConfig.name = "le1";
    leConfig.placeholder = "Enter name";
    QWidget* le = builder.buildLineEdit(leConfig);
    QVERIFY(le != nullptr);
    QCOMPARE(qobject_cast<QLineEdit*>(le)->placeholderText(), QString("Enter name"));
    delete le;
}

void TestShowboxBuilder::testPerformance()
{
    ShowboxBuilder builder;
    Showbox::Models::ButtonConfig config;
    config.name = "btn";
    
    QBENCHMARK {
        for (int i = 0; i < 100; ++i) {
            delete builder.buildButton(config);
        }
    }
}

QTEST_MAIN(TestShowboxBuilder)
#include "tst_showbox_builder.moc"

#include <QtTest>
#include <CLIBuilder.h>
#include <push_button_widget.h>

class TestCLIBuilder : public QObject
{
    Q_OBJECT

private slots:
    void testBuildPushButton();
};

void TestCLIBuilder::testBuildPushButton()
{
    CLIBuilder builder;
    PushButtonWidget *btn = builder.buildPushButton("OK", "btn_ok");
    
    QVERIFY(btn != nullptr);
    QCOMPARE(btn->text(), QString("OK"));
    QCOMPARE(btn->objectName(), QString("btn_ok"));
    
    delete btn;
}

QTEST_MAIN(TestCLIBuilder)
#include "tst_clibuilder.moc"

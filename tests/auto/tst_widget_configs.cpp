#include <QtTest>
// We expect this header to exist in libs/showbox-ui/include
#include <WidgetConfigs.h>

class TestWidgetConfigs : public QObject
{
    Q_OBJECT

private slots:
    void testWindowConfigDefaults();
    void testButtonConfigDefaults();
    void testLabelConfigDefaults();
};

void TestWidgetConfigs::testWindowConfigDefaults()
{
    Showbox::Models::WindowConfig config;
    QCOMPARE(config.title, QString(""));
    QCOMPARE(config.width, 800);
    QCOMPARE(config.height, 600);
    QVERIFY(config.isValid());
}

void TestWidgetConfigs::testButtonConfigDefaults()
{
    Showbox::Models::ButtonConfig config;
    QCOMPARE(config.text, QString("Button"));
    QCOMPARE(config.name, QString(""));
    QVERIFY(!config.isValid()); // Name is required for controls
    
    config.name = "btn1";
    QVERIFY(config.isValid());
}

void TestWidgetConfigs::testLabelConfigDefaults()
{
    Showbox::Models::LabelConfig config;
    QCOMPARE(config.text, QString("Label"));
    QVERIFY(!config.isValid()); // Name is required
    
    config.name = "lbl1";
    QVERIFY(config.isValid());
}

QTEST_MAIN(TestWidgetConfigs)
#include "tst_widget_configs.moc"

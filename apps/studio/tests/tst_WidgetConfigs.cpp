#include <QtTest>
#include "WidgetConfigs.h"

using namespace Showbox::Models;

class TestWidgetConfigs : public QObject {
    Q_OBJECT

private slots:
    void testCheckBoxConfig();
    void testRadioButtonConfig();
    void testSpinBoxConfig();
    void testSliderConfig();
    void testCalendarConfig();
    void testTextEditConfig();
    void testSeparatorConfig();
    void testGroupBoxConfig();
    void testFrameConfig();
    void testTabWidgetConfig();
};

void TestWidgetConfigs::testCheckBoxConfig() {
    CheckBoxConfig config;
    config.name = "chk";
    config.text = "Option";
    config.checked = true;
    
    QCOMPARE(config.name, QString("chk"));
    QCOMPARE(config.text, QString("Option"));
    QCOMPARE(config.checked, true);
    QVERIFY(config.isValid());
}

void TestWidgetConfigs::testRadioButtonConfig() {
    RadioButtonConfig config;
    config.name = "radio";
    config.text = "Choice";
    config.checked = true;
    
    QCOMPARE(config.name, QString("radio"));
    QCOMPARE(config.text, QString("Choice"));
    QCOMPARE(config.checked, true);
    QVERIFY(config.isValid());
}

void TestWidgetConfigs::testSpinBoxConfig() {
    SpinBoxConfig config;
    config.name = "spin";
    config.min = 0;
    config.max = 100;
    config.step = 5;
    config.value = 10;
    config.prefix = "$";
    config.suffix = " USD";
    
    QCOMPARE(config.min, 0);
    QCOMPARE(config.max, 100);
    QCOMPARE(config.step, 5);
    QCOMPARE(config.value, 10);
    QCOMPARE(config.prefix, QString("$"));
    QCOMPARE(config.suffix, QString(" USD"));
    QVERIFY(config.isValid());
}

void TestWidgetConfigs::testSliderConfig() {
    SliderConfig config;
    config.name = "slider";
    config.min = 0;
    config.max = 10;
    config.orientation = Qt::Horizontal;
    
    QCOMPARE(config.min, 0);
    QCOMPARE(config.max, 10);
    QCOMPARE(config.orientation, Qt::Horizontal);
    QVERIFY(config.isValid());
}

void TestWidgetConfigs::testCalendarConfig() {
    CalendarConfig config;
    config.name = "cal";
    QVERIFY(config.isValid());
}

void TestWidgetConfigs::testTextEditConfig() {
    TextEditConfig config;
    config.name = "editor";
    config.text = "Content";
    config.readOnly = true;
    config.richText = true;
    
    QCOMPARE(config.text, QString("Content"));
    QCOMPARE(config.readOnly, true);
    QCOMPARE(config.richText, true);
    QVERIFY(config.isValid());
}

void TestWidgetConfigs::testSeparatorConfig() {
    SeparatorConfig config;
    config.name = "sep";
    config.orientation = Qt::Vertical;
    QCOMPARE(config.orientation, Qt::Vertical);
    QVERIFY(config.isValid());
}

void TestWidgetConfigs::testGroupBoxConfig() {
    GroupBoxConfig config;
    config.name = "group";
    config.title = "Settings";
    config.layout.type = LayoutConfig::Grid;
    
    QCOMPARE(config.title, QString("Settings"));
    QCOMPARE(config.layout.type, LayoutConfig::Grid);
    QVERIFY(config.isValid());
}

void TestWidgetConfigs::testFrameConfig() {
    FrameConfig config;
    config.name = "frame";
    config.layout.type = LayoutConfig::HBox;
    QCOMPARE(config.layout.type, LayoutConfig::HBox);
    QVERIFY(config.isValid());
}

void TestWidgetConfigs::testTabWidgetConfig() {
    TabWidgetConfig config;
    config.name = "tabs";
    
    PageConfig p1;
    p1.name = "page1";
    p1.title = "General";
    
    PageConfig p2;
    p2.name = "page2";
    p2.title = "Advanced";
    
    config.pages.append(p1);
    config.pages.append(p2);
    
    QCOMPARE(config.pages.size(), 2);
    QCOMPARE(config.pages[0].title, QString("General"));
    QCOMPARE(config.pages[1].title, QString("Advanced"));
    QVERIFY(config.isValid());
}

QTEST_MAIN(TestWidgetConfigs)
#include "tst_WidgetConfigs.moc"

#include <QtTest>
#include <ParserMain.h>
#include <IShowboxBuilder.h>
#include <QWidget>

// Mock Builder
class MockBuilder : public IShowboxBuilder {
public:
    PushButtonWidget* buildPushButton(const QString &title, const QString &name) override {
        Q_UNUSED(title); Q_UNUSED(name);
        return nullptr;
    }
    
    QWidget* buildWindow(const Showbox::Models::WindowConfig& config) override {
        lastWindow = config;
        called = true;
        return nullptr; 
    }
    
    QWidget* buildButton(const Showbox::Models::ButtonConfig& config) override {
        lastButton = config;
        called = true;
        return nullptr; 
    }
    
    QWidget* buildLabel(const Showbox::Models::LabelConfig& config) override {
        lastLabel = config;
        called = true;
        return nullptr; 
    }

    QWidget* buildCheckBox(const Showbox::Models::CheckBoxConfig& config) override {
        lastCheckBox = config;
        called = true;
        return nullptr;
    }

    QWidget* buildRadioButton(const Showbox::Models::RadioButtonConfig& config) override {
        lastRadioButton = config;
        called = true;
        return nullptr;
    }

    QWidget* buildComboBox(const Showbox::Models::ComboBoxConfig& config) override {
        lastComboBox = config;
        called = true;
        return nullptr;
    }

    QWidget* buildList(const Showbox::Models::ListConfig& config) override {
        lastList = config;
        called = true;
        return nullptr;
    }

    // Stubs for remaining methods
    QWidget* buildTable(const Showbox::Models::TableConfig& config) override { Q_UNUSED(config); return nullptr; }
    QWidget* buildProgressBar(const Showbox::Models::ProgressBarConfig& config) override { Q_UNUSED(config); return nullptr; }
    QWidget* buildChart(const Showbox::Models::ChartConfig& config) override { Q_UNUSED(config); return nullptr; }
    QWidget* buildCalendar(const Showbox::Models::CalendarConfig& config) override { Q_UNUSED(config); return nullptr; }
    QWidget* buildSeparator(const Showbox::Models::SeparatorConfig& config) override { Q_UNUSED(config); return nullptr; }
    QWidget* buildSpinBox(const Showbox::Models::SpinBoxConfig& config) override { Q_UNUSED(config); return nullptr; }
    QWidget* buildSlider(const Showbox::Models::SliderConfig& config) override { Q_UNUSED(config); return nullptr; }
    QWidget* buildLineEdit(const Showbox::Models::LineEditConfig& config) override { Q_UNUSED(config); return nullptr; }
    QWidget* buildTextEdit(const Showbox::Models::TextEditConfig& config) override { Q_UNUSED(config); return nullptr; }
    QWidget* buildGroupBox(const Showbox::Models::GroupBoxConfig& config) override { Q_UNUSED(config); return nullptr; }
    QWidget* buildFrame(const Showbox::Models::FrameConfig& config) override { Q_UNUSED(config); return nullptr; }
    QWidget* buildTabWidget(const Showbox::Models::TabWidgetConfig& config) override { Q_UNUSED(config); return nullptr; }
    QLayout* buildLayout(const Showbox::Models::LayoutConfig& config) override { Q_UNUSED(config); return nullptr; }

    Showbox::Models::ButtonConfig lastButton;
    Showbox::Models::LabelConfig lastLabel;
    Showbox::Models::CheckBoxConfig lastCheckBox;
    Showbox::Models::RadioButtonConfig lastRadioButton;
    Showbox::Models::ComboBoxConfig lastComboBox;
    Showbox::Models::ListConfig lastList;
    Showbox::Models::WindowConfig lastWindow;
    bool called = false;
};

class TestParser : public QObject
{
    Q_OBJECT

private slots:
    void testParseAddButton();
    void testParseAddLabel();
    void testParseAddCheckBox();
    void testParseAddRadioButton();
    void testParseAddComboBox();
    void testParseAddListBox();
    void testParseAddWindow();
};

void TestParser::testParseAddButton()
{
    MockBuilder builder;
    ParserMain parser(&builder);
    parser.processLine("add button \"Click Me\" btn_test checkable checked");
    QVERIFY(builder.called);
    QCOMPARE(builder.lastButton.text, QString("Click Me"));
    QCOMPARE(builder.lastButton.name, QString("btn_test"));
    QCOMPARE(builder.lastButton.checkable, true);
    QCOMPARE(builder.lastButton.checked, true);
}

void TestParser::testParseAddLabel()
{
    MockBuilder builder;
    ParserMain parser(&builder);
    parser.processLine("add label \"Hello World\" lbl1 wordwrap");
    QVERIFY(builder.called);
    QCOMPARE(builder.lastLabel.text, QString("Hello World"));
    QCOMPARE(builder.lastLabel.name, QString("lbl1"));
    QCOMPARE(builder.lastLabel.wordWrap, true);
}

void TestParser::testParseAddCheckBox()
{
    MockBuilder builder;
    ParserMain parser(&builder);
    parser.processLine("add checkbox \"Turbo\" chk1 checked");
    QVERIFY(builder.called);
    QCOMPARE(builder.lastCheckBox.text, QString("Turbo"));
    QCOMPARE(builder.lastCheckBox.checked, true);
}

void TestParser::testParseAddRadioButton()
{
    MockBuilder builder;
    ParserMain parser(&builder);
    parser.processLine("add radiobutton \"Option 1\" rb1 checked");
    QVERIFY(builder.called);
    QCOMPARE(builder.lastRadioButton.text, QString("Option 1"));
    QCOMPARE(builder.lastRadioButton.checked, true);
}

void TestParser::testParseAddComboBox()
{
    MockBuilder builder;
    ParserMain parser(&builder);
    parser.processLine("add dropdownlist \"Choice\" combo1");
    QVERIFY(builder.called);
    QCOMPARE(builder.lastComboBox.name, QString("combo1"));
}

void TestParser::testParseAddListBox()
{
    MockBuilder builder;
    ParserMain parser(&builder);
    parser.processLine("add listbox \"Select\" list1 selection");
    QVERIFY(builder.called);
    QCOMPARE(builder.lastList.name, QString("list1"));
    QCOMPARE(builder.lastList.multipleSelection, true);
}

void TestParser::testParseAddWindow()
{
    MockBuilder builder;
    ParserMain parser(&builder);
    parser.processLine("add window \"My App\" main_win width 1024 height 768");
    QVERIFY(builder.called);
    QCOMPARE(builder.lastWindow.title, QString("My App"));
    QCOMPARE(builder.lastWindow.width, 1024);
    QCOMPARE(builder.lastWindow.height, 768);
}

QTEST_MAIN(TestParser)
#include "tst_parser.moc"

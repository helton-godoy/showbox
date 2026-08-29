#include <QtTest>
#include <ShowboxBuilder.h>
#include <WidgetConfigs.h>
#include <custom_table_widget.h>
#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QListWidget>
#include <QTableWidget>
#include <QProgressBar>
#include <QCheckBox>
#include <QRadioButton>
#include <QCalendarWidget>
#include <QFrame>
#include <QSpinBox>
#include <QSlider>
#include <QTextEdit>
#include <QGroupBox>
#include <QTabWidget>
#include <custom_chart_widget.h>

class TestShowboxBuilder : public QObject
{
    Q_OBJECT

private slots:
    void testCreateWindow();
    void testBuildLayout();
    void testBuildControls();
    void testBuildItemBasedWidgets();
    void testBuildSelectionWidgets();
    void testBuildUtilityWidgets();
    void testBuildNumericalInputs();
    void testBuildTextInputs();
    void testBuildBasicContainers();
    void testBuildTabWidget();
    void testBuildAdvancedVisuals();
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
    auto *lineEdit = qobject_cast<QLineEdit *>(le->focusProxy());
    QVERIFY(lineEdit != nullptr);
    QCOMPARE(lineEdit->placeholderText(), QString("Enter name"));
    delete le;
}

void TestShowboxBuilder::testBuildItemBasedWidgets()
{
    ShowboxBuilder builder;
    
    // Test ComboBox
    Showbox::Models::ComboBoxConfig cbConfig;
    cbConfig.name = "cb1";
    cbConfig.items << "Item 1" << "Item 2" << "Item 3";
    cbConfig.currentIndex = 1;
    QWidget* cb = builder.buildComboBox(cbConfig);
    QVERIFY(cb != nullptr);
    auto *combo = qobject_cast<QComboBox *>(cb->focusProxy());
    QVERIFY(combo != nullptr);
    QCOMPARE(combo->count(), 3);
    QCOMPARE(combo->currentIndex(), 1);
    delete cb;
    
    // Test List
    Showbox::Models::ListConfig listConfig;
    listConfig.name = "list1";
    listConfig.items << "Option A" << "Option B";
    QWidget* list = builder.buildList(listConfig);
    QVERIFY(list != nullptr);
    auto *listWidget = qobject_cast<QListWidget *>(list->focusProxy());
    QVERIFY(listWidget != nullptr);
    QCOMPARE(listWidget->count(), 2);
    delete list;
    
    // Test Table
    Showbox::Models::TableConfig tableConfig;
    tableConfig.name = "table1";
    tableConfig.headers << "Col 1" << "Col 2";
    tableConfig.rows << (QStringList() << "R1C1" << "R1C2");
    QWidget* table = builder.buildTable(tableConfig);
    QVERIFY(table != nullptr);
    auto *customTable = qobject_cast<CustomTableWidget*>(table);
    QVERIFY(customTable != nullptr);
    QCOMPARE(customTable->table()->columnCount(), 2);
    QCOMPARE(customTable->table()->rowCount(), 1);
    QCOMPARE(customTable->table()->item(0, 0)->text(), QString("R1C1"));
    delete table;
}

void TestShowboxBuilder::testBuildSelectionWidgets()
{
    ShowboxBuilder builder;

    // Test CheckBox
    Showbox::Models::CheckBoxConfig checkConfig;
    checkConfig.name = "check1";
    checkConfig.text = "Check Me";
    checkConfig.checked = true;
    QWidget* checkBox = builder.buildCheckBox(checkConfig);
    QVERIFY(checkBox != nullptr);
    QCheckBox* cb = qobject_cast<QCheckBox*>(checkBox);
    QVERIFY(cb != nullptr);
    QCOMPARE(cb->text(), QString("Check Me"));
    QCOMPARE(cb->isChecked(), true);
    delete checkBox;

    // Test RadioButton
    Showbox::Models::RadioButtonConfig radioConfig;
    radioConfig.name = "radio1";
    radioConfig.text = "Select Me";
    radioConfig.checked = true;
    QWidget* radio = builder.buildRadioButton(radioConfig);
    QVERIFY(radio != nullptr);
    QRadioButton* rb = qobject_cast<QRadioButton*>(radio);
    QVERIFY(rb != nullptr);
    QCOMPARE(rb->text(), QString("Select Me"));
    QCOMPARE(rb->isChecked(), true);
    delete radio;
}

void TestShowboxBuilder::testBuildUtilityWidgets()
{
    ShowboxBuilder builder;

    // Test Calendar
    Showbox::Models::CalendarConfig calConfig;
    calConfig.name = "calendar1";
    QWidget* calendar = builder.buildCalendar(calConfig);
    QVERIFY(calendar != nullptr);
    QCalendarWidget* cw = qobject_cast<QCalendarWidget*>(calendar);
    QVERIFY(cw != nullptr);
    delete calendar;

    // Test Separator
    Showbox::Models::SeparatorConfig sepConfig;
    sepConfig.name = "sep1";
    sepConfig.orientation = Qt::Horizontal;
    QWidget* separator = builder.buildSeparator(sepConfig);
    QVERIFY(separator != nullptr);
    QFrame* frame = qobject_cast<QFrame*>(separator);
    QVERIFY(frame != nullptr);
    QCOMPARE(frame->frameShape(), QFrame::HLine);
    delete separator;
}

void TestShowboxBuilder::testBuildNumericalInputs()
{
    ShowboxBuilder builder;

    // Test SpinBox
    Showbox::Models::SpinBoxConfig spinConfig;
    spinConfig.name = "spin1";
    spinConfig.min = 10;
    spinConfig.max = 50;
    spinConfig.value = 25;
    spinConfig.step = 5;
    spinConfig.suffix = " units";
    
    QWidget* spinBox = builder.buildSpinBox(spinConfig);
    QVERIFY(spinBox != nullptr);
    QSpinBox* sb = qobject_cast<QSpinBox*>(spinBox);
    QVERIFY(sb != nullptr);
    QCOMPARE(sb->minimum(), 10);
    QCOMPARE(sb->maximum(), 50);
    QCOMPARE(sb->value(), 25);
    QCOMPARE(sb->singleStep(), 5);
    QCOMPARE(sb->suffix(), QString(" units"));
    delete spinBox;

    // Test Slider
    Showbox::Models::SliderConfig sliderConfig;
    sliderConfig.name = "slider1";
    sliderConfig.min = 0;
    sliderConfig.max = 100;
    sliderConfig.value = 75;
    sliderConfig.orientation = Qt::Vertical;

    QWidget* slider = builder.buildSlider(sliderConfig);
    QVERIFY(slider != nullptr);
    QSlider* sl = qobject_cast<QSlider*>(slider);
    QVERIFY(sl != nullptr);
    QCOMPARE(sl->minimum(), 0);
    QCOMPARE(sl->maximum(), 100);
    QCOMPARE(sl->value(), 75);
    QCOMPARE(sl->orientation(), Qt::Vertical);
    delete slider;
}

void TestShowboxBuilder::testBuildTextInputs()
{
    ShowboxBuilder builder;

    // Test TextEdit
    Showbox::Models::TextEditConfig textConfig;
    textConfig.name = "text1";
    textConfig.text = "Initial Text";
    textConfig.readOnly = true;

    QWidget* textEdit = builder.buildTextEdit(textConfig);
    QVERIFY(textEdit != nullptr);
    QTextEdit* te = qobject_cast<QTextEdit*>(textEdit);
    QVERIFY(te != nullptr);
    QCOMPARE(te->toPlainText(), QString("Initial Text"));
    QCOMPARE(te->isReadOnly(), true);
    delete textEdit;
}

void TestShowboxBuilder::testBuildBasicContainers()
{
    ShowboxBuilder builder;

    // Test GroupBox
    Showbox::Models::GroupBoxConfig gbConfig;
    gbConfig.name = "gb1";
    gbConfig.title = "My Group";
    gbConfig.layout.type = Showbox::Models::LayoutConfig::VBox;

    QWidget* gb = builder.buildGroupBox(gbConfig);
    QVERIFY(gb != nullptr);
    QGroupBox* groupBox = qobject_cast<QGroupBox*>(gb);
    QVERIFY(groupBox != nullptr);
    QCOMPARE(groupBox->title(), QString("My Group"));
    QVERIFY(groupBox->layout() != nullptr);
    QVERIFY(qobject_cast<QVBoxLayout*>(groupBox->layout()) != nullptr);
    delete gb;

    // Test Frame
    Showbox::Models::FrameConfig frConfig;
    frConfig.name = "fr1";
    frConfig.layout.type = Showbox::Models::LayoutConfig::HBox;

    QWidget* fr = builder.buildFrame(frConfig);
    QVERIFY(fr != nullptr);
    QFrame* frame = qobject_cast<QFrame*>(fr);
    QVERIFY(frame != nullptr);
    QVERIFY(frame->layout() != nullptr);
    QVERIFY(qobject_cast<QHBoxLayout*>(frame->layout()) != nullptr);
    delete fr;
}

void TestShowboxBuilder::testBuildTabWidget()
{
    ShowboxBuilder builder;

    Showbox::Models::TabWidgetConfig tabConfig;
    tabConfig.name = "tabs1";
    
    Showbox::Models::PageConfig page1;
    page1.title = "Page 1";
    page1.layout.type = Showbox::Models::LayoutConfig::VBox;
    
    Showbox::Models::PageConfig page2;
    page2.title = "Page 2";
    page2.layout.type = Showbox::Models::LayoutConfig::HBox;

    tabConfig.pages << page1 << page2;

    QWidget* tw = builder.buildTabWidget(tabConfig);
    QVERIFY(tw != nullptr);
    QTabWidget* tabWidget = qobject_cast<QTabWidget*>(tw);
    QVERIFY(tabWidget != nullptr);
    QCOMPARE(tabWidget->count(), 2);
    QCOMPARE(tabWidget->tabText(0), QString("Page 1"));
    QCOMPARE(tabWidget->tabText(1), QString("Page 2"));
    
    // Check if pages have layouts
    QVERIFY(tabWidget->widget(0)->layout() != nullptr);
    QVERIFY(qobject_cast<QVBoxLayout*>(tabWidget->widget(0)->layout()) != nullptr);
    
    delete tw;
}

void TestShowboxBuilder::testBuildAdvancedVisuals()
{
    ShowboxBuilder builder;
    
    // Test ProgressBar
    Showbox::Models::ProgressBarConfig pbConfig;
    pbConfig.name = "pb1";
    pbConfig.value = 50;
    QWidget* pb = builder.buildProgressBar(pbConfig);
    QVERIFY(pb != nullptr);
    QCOMPARE(qobject_cast<QProgressBar*>(pb)->value(), 50);
    delete pb;
    
    // Test Chart
    Showbox::Models::ChartConfig chartConfig;
    chartConfig.name = "chart1";
    chartConfig.title = "Sales";
    chartConfig.data["Jan"] = 100.0;
    chartConfig.data["Feb"] = 150.0;
    QWidget* chart = builder.buildChart(chartConfig);
    QVERIFY(chart != nullptr);
    // CustomChartWidget setTitle sets the windowTitle if it's a top-level widget or internally.
    // Let's just verify it exists for now as specific internal state check might be complex.
    QVERIFY(qobject_cast<CustomChartWidget*>(chart) != nullptr);
    delete chart;
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

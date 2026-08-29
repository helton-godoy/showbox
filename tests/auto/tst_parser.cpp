#include <QtTest>
#include <ParserMain.h>
#include <CLIBuilder.h>
#include <IShowboxBuilder.h>
#include <custom_chart_widget.h>
#include <custom_table_widget.h>
#include <QtCharts/QPieSeries>
#include <QtCharts/QHorizontalBarSeries>
#include <QtCharts/QLineSeries>
#include <QWidget>
#include <QTabWidget>
#include <QTableWidget>
#include <QTemporaryFile>
#include <QTemporaryDir>
#include <QFileInfo>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QCalendarWidget>
#include <QFrame>
#include <QGroupBox>
#include <QRadioButton>
#include <QSpinBox>
#include <QSlider>
#include <QLineEdit>
#include <QListWidget>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QPointer>
#include <QProgressBar>
#include <sstream>
#include <iostream>

// Mock Builder returning real widgets for runtime interaction tests
class MockBuilder : public IShowboxBuilder {
public:
    QList<QPointer<QWidget>> createdWidgets;

    ~MockBuilder() {
        for (auto w : createdWidgets) {
            if (w && !w->parentWidget()) {
                delete w;
            }
        }
    }

    template<typename T>
    T* createReal(const QString& name) {
        T* w = new T();
        w->setObjectName(name);
        createdWidgets.append(w);
        return w;
    }

    PushButtonWidget* buildPushButton(const QString &title, const QString &name) override {
        Q_UNUSED(title); Q_UNUSED(name);
        return nullptr;
    }
    
    QWidget* buildWindow(const Showbox::Models::WindowConfig& config) override { 
        lastWindow = config;
        called = true;
        auto* w = createReal<QWidget>(config.name); 
        w->setLayout(new QVBoxLayout());
        return w;
    }
    
    QWidget* buildButton(const Showbox::Models::ButtonConfig& config) override { 
        lastButton = config;
        called = true;
        return createReal<QPushButton>(config.name); 
    }
    
    QWidget* buildLabel(const Showbox::Models::LabelConfig& config) override { 
        lastLabel = config;
        called = true;
        return createReal<QLabel>(config.name); 
    }

    QWidget* buildCheckBox(const Showbox::Models::CheckBoxConfig& config) override {
        lastCheckBox = config;
        called = true;
        return createReal<QCheckBox>(config.name);
    }

    QWidget* buildRadioButton(const Showbox::Models::RadioButtonConfig& config) override {
        lastRadioButton = config;
        called = true;
        return createReal<QRadioButton>(config.name);
    }

    QWidget* buildComboBox(const Showbox::Models::ComboBoxConfig& config) override {
        lastComboBox = config;
        called = true;
        return createReal<QWidget>(config.name); // Simplified
    }

    QWidget* buildList(const Showbox::Models::ListConfig& config) override {
        lastList = config;
        called = true;
        return createReal<QWidget>(config.name); // Simplified
    }

    QWidget* buildSpinBox(const Showbox::Models::SpinBoxConfig& config) override {
        lastSpinBox = config;
        called = true;
        return createReal<QSpinBox>(config.name);
    }

    QWidget* buildSlider(const Showbox::Models::SliderConfig& config) override {
        lastSlider = config;
        called = true;
        return createReal<QSlider>(config.name);
    }

    QWidget* buildLineEdit(const Showbox::Models::LineEditConfig& config) override {
        lastLineEdit = config;
        called = true;
        return createReal<QLineEdit>(config.name);
    }

    QWidget* buildTextEdit(const Showbox::Models::TextEditConfig& config) override {
        lastTextEdit = config;
        called = true;
        return createReal<QTextEdit>(config.name);
    }

    QWidget* buildGroupBox(const Showbox::Models::GroupBoxConfig& config) override {
        lastGroupBox = config;
        called = true;
        auto* w = createReal<QWidget>(config.name);
        w->setLayout(new QVBoxLayout());
        return w;
    }

    QWidget* buildFrame(const Showbox::Models::FrameConfig& config) override {
        lastFrame = config;
        called = true;
        auto* w = createReal<QWidget>(config.name);
        w->setLayout(new QVBoxLayout());
        return w;
    }

    QWidget* buildTabWidget(const Showbox::Models::TabWidgetConfig& config) override {
        lastTabWidget = config;
        called = true;
        auto* w = new QTabWidget();
        w->setObjectName(config.name);
        createdWidgets.append(w);
        return w;
    }

    // Stubs for remaining methods
    QWidget* buildTable(const Showbox::Models::TableConfig& config) override { Q_UNUSED(config); return nullptr; }
    QWidget* buildProgressBar(const Showbox::Models::ProgressBarConfig& config) override { Q_UNUSED(config); return nullptr; }
    QWidget* buildChart(const Showbox::Models::ChartConfig& config) override { Q_UNUSED(config); return nullptr; }
    QWidget* buildCalendar(const Showbox::Models::CalendarConfig& config) override {
        lastCalendar = config; called = true; return createReal<QCalendarWidget>(config.name);
    }
    QWidget* buildSeparator(const Showbox::Models::SeparatorConfig& config) override {
        lastSeparator = config; called = true; return createReal<QFrame>(config.name);
    }
    QLayout* buildLayout(const Showbox::Models::LayoutConfig& config) override { Q_UNUSED(config); return nullptr; }

    // State capture for verification
    Showbox::Models::ButtonConfig lastButton;
    Showbox::Models::LabelConfig lastLabel;
    Showbox::Models::CheckBoxConfig lastCheckBox;
    Showbox::Models::RadioButtonConfig lastRadioButton;
    Showbox::Models::ComboBoxConfig lastComboBox;
    Showbox::Models::ListConfig lastList;
    Showbox::Models::WindowConfig lastWindow;
    Showbox::Models::SpinBoxConfig lastSpinBox;
    Showbox::Models::SliderConfig lastSlider;
    Showbox::Models::LineEditConfig lastLineEdit;
    Showbox::Models::TextEditConfig lastTextEdit;
    Showbox::Models::GroupBoxConfig lastGroupBox;
    Showbox::Models::FrameConfig lastFrame;
    Showbox::Models::CalendarConfig lastCalendar;
    Showbox::Models::SeparatorConfig lastSeparator;
    Showbox::Models::TabWidgetConfig lastTabWidget;
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
    void testParseAddSlider();
    void testParseAddTextBox();
    void testParseAddTextView();
    void testParseHierarchicalLayout();
    void testRuntimeInteraction();
    void testLegacyDialogCommands();
    void testQueryOutputContract();
    void testComplexQueryOutputContract();
    void testStepThreeLevelLayout();
    void testPositionInsertionCursor();
    void testDocumentedWidgetOptions();
};

void TestParser::testDocumentedWidgetOptions()
{
    MockBuilder builder;
    ParserMain parser(&builder);

    parser.processLine("add frame frame_1 horizontal box sunken");
    QCOMPARE(builder.lastFrame.name, QString("frame_1"));
    QCOMPARE(builder.lastFrame.layout.type, Showbox::Models::LayoutConfig::HBox);
    QCOMPARE(builder.lastFrame.shape, static_cast<int>(QFrame::Box));
    QCOMPARE(builder.lastFrame.shadow, static_cast<int>(QFrame::Sunken));
    parser.processLine("end frame");

    parser.processLine("add separator separator_1 vertical raised");
    QCOMPARE(builder.lastSeparator.name, QString("separator_1"));
    QCOMPARE(builder.lastSeparator.orientation, static_cast<int>(Qt::Vertical));
    QCOMPARE(builder.lastSeparator.shadow, static_cast<int>(QFrame::Raised));
    parser.processLine("add separator vertical sunken");
    QCOMPARE(builder.lastSeparator.name, QString());
    QCOMPARE(builder.lastSeparator.orientation, static_cast<int>(Qt::Vertical));
    QCOMPARE(builder.lastSeparator.shadow, static_cast<int>(QFrame::Sunken));

    parser.processLine(
        "add calendar Date calendar_1 date 2026-08-16 format dd/MM/yyyy selection");
    QCOMPARE(builder.lastCalendar.name, QString("calendar_1"));
    QCOMPARE(builder.lastCalendar.date, QString("2026-08-16"));
    QCOMPARE(builder.lastCalendar.format, QString("dd/MM/yyyy"));
    QVERIFY(builder.lastCalendar.selection);

    CLIBuilder realBuilder;
    ParserMain realParser(&realBuilder);
    realParser.setRootWidget(realBuilder.window());

    realParser.processLine(
        "add textbox User: textbox_1 text Alice placeholder Name password");
    QWidget *textBox = realBuilder.window()->findChild<QWidget *>("textbox_1");
    QVERIFY(textBox);
    auto *textInput = qobject_cast<QLineEdit *>(textBox->focusProxy());
    QVERIFY(textInput);
    QCOMPARE(textInput->text(), QString("Alice"));
    QCOMPARE(textInput->placeholderText(), QString("Name"));
    QCOMPARE(textInput->echoMode(), QLineEdit::Password);
    const auto textBoxLabels = textBox->findChildren<QLabel *>(
        QString(), Qt::FindDirectChildrenOnly);
    QCOMPARE(textBoxLabels.size(), 1);
    QCOMPARE(textBoxLabels.first()->text(), QString("User:"));
    realParser.processLine("set textbox_1 text Bob");
    realParser.processLine("set textbox_1 title Account:");
    realParser.processLine("set textbox_1 readonly");
    QCOMPARE(textInput->text(), QString("Bob"));
    QCOMPARE(textBoxLabels.first()->text(), QString("Account:"));
    QVERIFY(textInput->isReadOnly());
    realParser.processLine("unset textbox_1 readonly");
    realParser.processLine("unset textbox_1 password");
    realParser.processLine("unset textbox_1 placeholder");
    QVERIFY(!textInput->isReadOnly());
    QCOMPARE(textInput->echoMode(), QLineEdit::Normal);
    QVERIFY(textInput->placeholderText().isEmpty());

    std::ostringstream textCaptured;
    std::streambuf *textPrevious = std::cout.rdbuf(textCaptured.rdbuf());
    realParser.processLine("query textbox_1");
    std::cout.rdbuf(textPrevious);
    QCOMPARE(QString::fromStdString(textCaptured.str()),
             QString("textbox_1=Bob\n"));
    realParser.processLine("clear textbox_1");
    QCOMPARE(textInput->text(), QString());

    realParser.processLine("add dropdownlist Country: country_1 selection");
    realParser.processLine("add item Brazil");
    realParser.processLine("add item Portugal current");
    realParser.processLine("end dropdownlist");
    QWidget *comboContainer =
        realBuilder.window()->findChild<QWidget *>("country_1");
    QVERIFY(comboContainer);
    auto *country = qobject_cast<QComboBox *>(comboContainer->focusProxy());
    QVERIFY(country);
    QCOMPARE(country->count(), 2);
    QCOMPARE(country->currentText(), QString("Portugal"));
    QCOMPARE(comboContainer->findChild<QLabel *>()->text(), QString("Country:"));
    realParser.processLine("set country_1 title Nation:");
    QCOMPARE(comboContainer->findChild<QLabel *>()->text(), QString("Nation:"));

    realParser.processLine("add listbox Files: files_1 selection");
    realParser.processLine("add item One");
    realParser.processLine("add item Two current");
    realParser.processLine("end listbox");
    QWidget *selectionListContainer =
        realBuilder.window()->findChild<QWidget *>("files_1");
    QVERIFY(selectionListContainer);
    auto *files =
        qobject_cast<QListWidget *>(selectionListContainer->focusProxy());
    QVERIFY(files);
    QCOMPARE(files->selectionMode(), QAbstractItemView::SingleSelection);
    QCOMPARE(files->currentItem()->text(), QString("Two"));
    realParser.processLine("set files_1#0 current");
    QCOMPARE(files->currentItem()->text(), QString("One"));
    realParser.processLine("set files_1#0 title First");
    QCOMPARE(files->item(0)->text(), QString("First"));
    realParser.processLine("set files_1#0 iconsize 28");
    QCOMPARE(files->iconSize(), QSize(28, 28));

    textCaptured.str("");
    textCaptured.clear();
    textPrevious = std::cout.rdbuf(textCaptured.rdbuf());
    realParser.processLine("query country_1");
    realParser.processLine("query files_1");
    std::cout.rdbuf(textPrevious);
    QCOMPARE(QString::fromStdString(textCaptured.str()),
             QString("country_1=Portugal\nfiles_1=First\n"));
    realParser.processLine("set files_1 activation");
    textCaptured.str("");
    textCaptured.clear();
    textPrevious = std::cout.rdbuf(textCaptured.rdbuf());
    files->itemActivated(files->item(0));
    std::cout.rdbuf(textPrevious);
    QCOMPARE(QString::fromStdString(textCaptured.str()),
             QString("files_1=First\n"));
    realParser.processLine("unset files_1#0 title");
    QVERIFY(files->item(0)->text().isEmpty());

    realParser.processLine(
        "add calendar Date calendar_formatted date 2026-08-16 format dd/MM/yyyy");
    std::ostringstream captured;
    std::streambuf *previous = std::cout.rdbuf(captured.rdbuf());
    realParser.processLine("query calendar_formatted");
    std::cout.rdbuf(previous);
    QCOMPARE(QString::fromStdString(captured.str()),
             QString("calendar_formatted=16/08/2026\n"));
    realParser.processLine("set calendar_formatted minimum 2026-08-01");
    realParser.processLine("set calendar_formatted maximum 2026-08-31");
    realParser.processLine("unset calendar_formatted navigation");
    QCOMPARE(realBuilder.window()
                 ->findChild<QCalendarWidget *>("calendar_formatted")
                 ->minimumDate(),
             QDate(2026, 8, 1));
    QCOMPARE(realBuilder.window()
                 ->findChild<QCalendarWidget *>("calendar_formatted")
                 ->maximumDate(),
             QDate(2026, 8, 31));
    QVERIFY(!realBuilder.window()
                 ->findChild<QCalendarWidget *>("calendar_formatted")
                 ->isNavigationBarVisible());

    realParser.processLine(
        "add chart Sales chart_1 data North:10;South:20");
    auto *chart = realBuilder.window()->findChild<CustomChartWidget *>("chart_1");
    QVERIFY(chart);
    QCOMPARE(chart->chart()->series().size(), 1);
    auto *series = qobject_cast<QPieSeries *>(chart->chart()->series().first());
    QVERIFY(series);
    QCOMPARE(series->count(), 2);

    realParser.processLine("set chart_1 data East:30;West:40");
    series = qobject_cast<QPieSeries *>(chart->chart()->series().first());
    QVERIFY(series);
    QCOMPARE(series->count(), 2);
    realParser.processLine("set chart_1 append Central:50");
    QCOMPARE(series->count(), 3);

    realParser.processLine("set chart_1 axis horizontal");
    QVERIFY(qobject_cast<QHorizontalBarSeries *>(
        chart->chart()->series().first()));
    realParser.processLine("set chart_1 axis vertical");
    QVERIFY(qobject_cast<QLineSeries *>(chart->chart()->series().first()));

    QTemporaryDir chartOutput;
    QVERIFY(chartOutput.isValid());
    const QString chartPath = chartOutput.filePath("chart.png");
    realParser.processLine(
        QString("set chart_1 export \"%1\"").arg(chartPath));
    QVERIFY(QFileInfo(chartPath).isFile());
    QVERIFY(QFileInfo(chartPath).size() > 0);

    captured.str("");
    captured.clear();
    previous = std::cout.rdbuf(captured.rdbuf());
    chart->itemClicked("East", 30.0);
    std::cout.rdbuf(previous);
    QCOMPARE(QString::fromStdString(captured.str()),
             QString("chart_1.slice[\"East\"]=30\n"));

    realParser.processLine("unset chart_1 data");
    QVERIFY(chart->chart()->series().isEmpty());

    realParser.processLine(
        "add table Name;Value table_advanced readonly selection search");
    realParser.processLine("end table");
    auto *customTable =
        realBuilder.window()->findChild<CustomTableWidget *>("table_advanced");
    QVERIFY(customTable);
    QCOMPARE(customTable->table()->editTriggers(),
             QAbstractItemView::NoEditTriggers);
    const auto searchFields = customTable->findChildren<QLineEdit *>();
    QVERIFY(!searchFields.isEmpty());
    QVERIFY(!searchFields.first()->isHidden());

    realParser.processLine("set table_advanced headers Key;Amount");
    QCOMPARE(customTable->table()->horizontalHeaderItem(0)->text(),
             QString("Key"));
    realParser.processLine("unset table_advanced readonly");
    QCOMPARE(customTable->table()->editTriggers(),
             QAbstractItemView::AllEditTriggers);
    realParser.processLine("unset table_advanced search");
    QVERIFY(searchFields.first()->isHidden());

    QTemporaryFile csv;
    QVERIFY(csv.open());
    QCOMPARE(csv.write("North;10\nSouth;20\n"), qint64(18));
    QVERIFY(csv.flush());
    realParser.processLine(
        QString("set table_advanced file \"%1\"").arg(csv.fileName()));
    QCOMPARE(customTable->table()->rowCount(), 2);
    realParser.processLine("set table_advanced add_line East;30");
    QCOMPARE(customTable->table()->rowCount(), 3);
    QCOMPARE(customTable->table()->item(2, 0)->text(), QString("East"));
    realParser.processLine("set table_advanced del_line 1");
    QCOMPARE(customTable->table()->rowCount(), 2);
    QCOMPARE(customTable->table()->item(1, 0)->text(), QString("East"));
    realParser.processLine("set table_advanced del_line");
    QCOMPARE(customTable->table()->rowCount(), 1);
    QCOMPARE(customTable->table()->item(0, 0)->text(), QString("East"));

    captured.str("");
    captured.clear();
    previous = std::cout.rdbuf(captured.rdbuf());
    customTable->cellEdited(0, 1, "11");
    customTable->rowSelected(0);
    std::cout.rdbuf(previous);
    QCOMPARE(QString::fromStdString(captured.str()),
             QString("table_advanced[0][1]=11\ntable_advanced_selection=0\n"));

    realParser.processLine("add progressbar progress_1 minimum 10 maximum 90");
    auto *progress =
        realBuilder.window()->findChild<QProgressBar *>("progress_1");
    QVERIFY(progress);
    QCOMPARE(progress->minimum(), 10);
    QCOMPARE(progress->maximum(), 90);
    realParser.processLine("set progress_1 busy");
    QCOMPARE(progress->minimum(), 0);
    QCOMPARE(progress->maximum(), 0);
    realParser.processLine("unset progress_1 busy");
    QCOMPARE(progress->minimum(), 0);
    QCOMPARE(progress->maximum(), 100);
    realParser.processLine("set progress_1 vertical");
    QCOMPARE(progress->orientation(), Qt::Vertical);
    realParser.processLine("set progress_1 format %v/%m");
    QCOMPARE(progress->format(), QString("%v/%m"));
    realParser.processLine("unset progress_1 format");
    QCOMPARE(progress->format(), QString("%p%"));

    QTemporaryFile textFile;
    QVERIFY(textFile.open());
    QCOMPARE(textFile.write("Showbox text view"), qint64(17));
    QVERIFY(textFile.flush());
    realParser.processLine(
        QString("add textview text_view file \"%1\"").arg(textFile.fileName()));
    auto *textView =
        realBuilder.window()->findChild<QTextEdit *>("text_view");
    QVERIFY(textView);
    QVERIFY(textView->isReadOnly());
    QCOMPARE(textView->toPlainText(), QString("Showbox text view"));

    realParser.processLine("add pushbutton Action action_1");
    auto *action =
        realBuilder.window()->findChild<QPushButton *>("action_1");
    QVERIFY(action);
    realParser.processLine("set action_1 apply");
    realParser.processLine("set action_1 exit");
    realParser.processLine("set action_1 default");
    realParser.processLine("set action_1 iconsize 24");
    QVERIFY(action->property("showboxApply").toBool());
    QVERIFY(action->property("showboxExit").toBool());
    QVERIFY(action->isDefault());
    QCOMPARE(action->iconSize(), QSize(24, 24));
    realParser.processLine("unset action_1 apply");
    realParser.processLine("unset action_1 exit");
    realParser.processLine("unset action_1 default");
    QVERIFY(!action->property("showboxApply").toBool());
    QVERIFY(!action->property("showboxExit").toBool());
    QVERIFY(!action->isDefault());

    realParser.processLine("add frame styled_frame box raised");
    auto *styledFrame =
        realBuilder.window()->findChild<QFrame *>("styled_frame");
    QVERIFY(styledFrame);
    QCOMPARE(styledFrame->frameShape(), QFrame::Box);
    QCOMPARE(styledFrame->frameShadow(), QFrame::Raised);
    realParser.processLine("set styled_frame panel");
    realParser.processLine("set styled_frame sunken");
    realParser.processLine("set styled_frame vertical");
    QCOMPARE(styledFrame->frameShape(), QFrame::Panel);
    QCOMPARE(styledFrame->frameShadow(), QFrame::Sunken);
    QCOMPARE(qobject_cast<QBoxLayout *>(styledFrame->layout())->direction(),
             QBoxLayout::TopToBottom);
    realParser.processLine("add stretch 3");
    auto *styledLayout = qobject_cast<QBoxLayout *>(styledFrame->layout());
    QCOMPARE(styledLayout->stretch(styledLayout->count() - 1), 3);
    realParser.processLine("end frame");

    realParser.processLine("add groupbox Group group_1 vertical checkable checked");
    auto *group = realBuilder.window()->findChild<QGroupBox *>("group_1");
    QVERIFY(group);
    realParser.processLine("set group_1 title Renamed");
    QCOMPARE(group->title(), QString("Renamed"));
    realParser.processLine("unset group_1 title");
    QVERIFY(group->title().isEmpty());
    realParser.processLine("end groupbox");

    realParser.processLine("add label placeholder.gif runtime_image");
    auto *runtimeImage =
        realBuilder.window()->findChild<QLabel *>("runtime_image");
    QVERIFY(runtimeImage);
    realParser.processLine("set runtime_image animation placeholder.gif");
    QVERIFY(runtimeImage->movie());
    realParser.processLine("unset runtime_image animation");
    QVERIFY(runtimeImage->text().isEmpty());

    realParser.processLine("unset text_view file");
    QVERIFY(textView->toPlainText().isEmpty());
}

void TestParser::testStepThreeLevelLayout()
{
    CLIBuilder builder;
    ParserMain parser(&builder);
    parser.setRootWidget(builder.window());

    parser.processLine("add label First label_first");
    parser.processLine("step horizontal");
    parser.processLine("add label Second label_second");

    QLayout *root = builder.window()->layout();
    QCOMPARE(root->count(), 1);
    QWidget *firstRow = root->itemAt(0)->widget();
    QVERIFY(firstRow);
    QCOMPARE(firstRow->layout()->count(), 2);
    QCOMPARE(firstRow->layout()->itemAt(0)->widget()->layout()
                 ->itemAt(0)->widget()->objectName(),
             QString("label_first"));
    QCOMPARE(firstRow->layout()->itemAt(1)->widget()->layout()
                 ->itemAt(0)->widget()->objectName(),
             QString("label_second"));

    parser.processLine("step vertical");
    parser.processLine("add label Third label_third");
    QCOMPARE(root->count(), 2);
    QWidget *secondRow = root->itemAt(1)->widget();
    QVERIFY(secondRow);
    QCOMPARE(secondRow->layout()->count(), 1);
    QCOMPARE(secondRow->layout()->itemAt(0)->widget()->layout()
                 ->itemAt(0)->widget()->objectName(),
             QString("label_third"));
}

void TestParser::testPositionInsertionCursor()
{
    CLIBuilder builder;
    ParserMain parser(&builder);
    parser.setRootWidget(builder.window());

    parser.processLine("add label A label_a");
    parser.processLine("add label B label_b");
    parser.processLine("position label_a");
    parser.processLine("add label C label_c");
    parser.processLine("add label D label_d");

    QLayout *rootLayout = builder.window()->layout();
    QCOMPARE(rootLayout->itemAt(0)->widget()->objectName(), QString("label_c"));
    QCOMPARE(rootLayout->itemAt(1)->widget()->objectName(), QString("label_d"));
    QCOMPARE(rootLayout->itemAt(2)->widget()->objectName(), QString("label_a"));
    QCOMPARE(rootLayout->itemAt(3)->widget()->objectName(), QString("label_b"));

    parser.processLine("position behind label_a");
    parser.processLine("add label E label_e");
    QCOMPARE(rootLayout->itemAt(3)->widget()->objectName(), QString("label_e"));

    parser.processLine("add frame frame_1");
    parser.processLine("end frame");
    parser.processLine("position onto frame_1");
    parser.processLine("add label Inside label_inside");
    QWidget *frame = builder.window()->findChild<QWidget *>("frame_1");
    QVERIFY(frame);
    QCOMPARE(frame->layout()->itemAt(0)->widget()->objectName(),
             QString("label_inside"));

    parser.processLine("add listbox Items list_1");
    parser.processLine("add item Alpha");
    parser.processLine("add item Beta");
    parser.processLine("end listbox");
    parser.processLine("position list_1#0");
    parser.processLine("add item BeforeAlpha");
    parser.processLine("position behind list_1:Alpha");
    parser.processLine("add item AfterAlpha");
    QWidget *listContainer = builder.window()->findChild<QWidget *>("list_1");
    QVERIFY(listContainer);
    auto *list = qobject_cast<QListWidget *>(listContainer->focusProxy());
    QVERIFY(list);
    QCOMPARE(list->count(), 4);
    QCOMPARE(list->item(0)->text(), QString("BeforeAlpha"));
    QCOMPARE(list->item(1)->text(), QString("Alpha"));
    QCOMPARE(list->item(2)->text(), QString("AfterAlpha"));
    QCOMPARE(list->item(3)->text(), QString("Beta"));
    parser.processLine("clear list_1");
    QCOMPARE(list->count(), 0);

    parser.processLine("add table Col1;Col2 table_1");
    parser.processLine("add item A;1");
    parser.processLine("add item B;2");
    parser.processLine("end table");
    parser.processLine("position table_1#1");
    parser.processLine("add item Inserted;9");
    auto *table = builder.window()->findChild<QTableWidget *>("table_1");
    QVERIFY(table);
    QCOMPARE(table->rowCount(), 3);
    QCOMPARE(table->item(0, 0)->text(), QString("A"));
    QCOMPARE(table->item(1, 0)->text(), QString("Inserted"));
    QCOMPARE(table->item(2, 0)->text(), QString("B"));
    parser.processLine("clear table_1");
    QCOMPARE(table->rowCount(), 0);
    QVERIFY(builder.window()->findChild<CustomTableWidget *>("table_1"));

    parser.processLine("add tabs tabs_1");
    parser.processLine("add page PageA page_a");
    parser.processLine("end page");
    parser.processLine("add page PageB page_b");
    parser.processLine("end page");
    parser.processLine("end tabs");
    parser.processLine("position page_a");
    parser.processLine("add page BeforeA page_before_a current");
    parser.processLine("end page");
    parser.processLine("position behind page_a");
    parser.processLine("add page AfterA page_after_a");
    auto *tabs = builder.window()->findChild<QTabWidget *>("tabs_1");
    QVERIFY(tabs);
    QCOMPARE(tabs->count(), 4);
    QCOMPARE(tabs->widget(0)->objectName(), QString("page_before_a"));
    QCOMPARE(tabs->widget(1)->objectName(), QString("page_a"));
    QCOMPARE(tabs->widget(2)->objectName(), QString("page_after_a"));
    QCOMPARE(tabs->widget(3)->objectName(), QString("page_b"));
    QCOMPARE(tabs->currentWidget()->objectName(), QString("page_before_a"));
    parser.processLine("set current page_b");
    QCOMPARE(tabs->currentWidget()->objectName(), QString("page_b"));
    parser.processLine("set page_b title RenamedB");
    QCOMPARE(tabs->tabText(tabs->indexOf(tabs->widget(3))),
             QString("RenamedB"));
    parser.processLine("set tabs_1 bottom");
    QCOMPARE(tabs->tabPosition(), QTabWidget::South);
    parser.processLine("disable page_b");
    QVERIFY(!tabs->isTabEnabled(tabs->indexOf(tabs->widget(3))));
    parser.processLine("enable page_b");
    QVERIFY(tabs->isTabEnabled(tabs->indexOf(tabs->widget(3))));
}

void TestParser::testLegacyDialogCommands()
{
    MockBuilder builder;
    ParserMain parser(&builder);
    QWidget root;
    root.setObjectName("showbox");
    parser.setRootWidget(&root);

    QSignalSpy showSpy(&parser, &ParserMain::showRequested);
    parser.processLine("set title \"Legacy title\"");
    QCOMPARE(root.windowTitle(), QString("Legacy title"));

    parser.processLine("show");
    QCOMPARE(showSpy.count(), 1);
}

void TestParser::testQueryOutputContract()
{
    CLIBuilder builder;
    ParserMain parser(&builder);
    parser.setRootWidget(builder.window());
    parser.processLine("add checkbox \"Last alphabetically\" z_value checked");
    parser.processLine("add checkbox \"First alphabetically\" a_value");

    std::ostringstream captured;
    std::streambuf *previous = std::cout.rdbuf(captured.rdbuf());
    parser.processLine("query");
    std::cout.rdbuf(previous);

    QCOMPARE(QString::fromStdString(captured.str()),
             QString("z_value=1\na_value=0\n"));
}

void TestParser::testComplexQueryOutputContract()
{
    CLIBuilder builder;
    ParserMain parser(&builder);
    parser.setRootWidget(builder.window());
    parser.processLine("add table Key;Value report_table");
    parser.processLine("add item Alpha;1");
    parser.processLine("end table");
    parser.processLine("add chart Report report_chart");
    parser.processLine("set report_chart data Zebra:2;Alpha:1");
    parser.processLine("add progressbar ignored_progress value 50");

    std::ostringstream captured;
    std::streambuf *previous = std::cout.rdbuf(captured.rdbuf());
    parser.processLine("query");
    std::cout.rdbuf(previous);

    QCOMPARE(QString::fromStdString(captured.str()),
             QString("report_table[0][0]=Alpha\n"
                     "report_table[0][1]=1\n"
                     "report_chart.slice[\"Zebra\"]=2.000000\n"
                     "report_chart.slice[\"Alpha\"]=1.000000\n"));
}

void TestParser::testRuntimeInteraction()
{
    MockBuilder builder;
    ParserMain parser(&builder);
    
    // 1. Create a checkbox
    parser.processLine("add checkbox \"Turbo\" chk1");
    QCheckBox* cb = qobject_cast<QCheckBox*>(builder.createdWidgets.last());
    QVERIFY(cb != nullptr);
    QCOMPARE(cb->isChecked(), false);
    
    // 2. Set it
    parser.processLine("set checked chk1");
    QCOMPARE(cb->isChecked(), true);
    
    // 3. Unset it
    parser.processLine("unset checked chk1");
    QCOMPARE(cb->isChecked(), false);
    
}

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

    parser.processLine("add label animation.gif animated_label animation");
    QCOMPARE(builder.lastLabel.name, QString("animated_label"));
    QCOMPARE(builder.lastLabel.iconPath, QString("animation.gif"));
    QVERIFY(builder.lastLabel.animation);
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
    QCOMPARE(builder.lastComboBox.title, QString("Choice"));
}

void TestParser::testParseAddListBox()
{
    MockBuilder builder;
    ParserMain parser(&builder);
    parser.processLine("add listbox \"Select\" list1 selection");
    QVERIFY(builder.called);
    QCOMPARE(builder.lastList.name, QString("list1"));
    QCOMPARE(builder.lastList.title, QString("Select"));
    QVERIFY(builder.lastList.selection);
    QVERIFY(!builder.lastList.multipleSelection);
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

void TestParser::testParseAddSlider()
{
    {
        MockBuilder builder;
        ParserMain parser(&builder);
        parser.processLine("add slider \"Volume\" sl1 vertical value 75");
        QVERIFY(builder.called);
        QCOMPARE(builder.lastSlider.name, QString("sl1"));
        QCOMPARE(builder.lastSlider.orientation, 2); // Vertical
        QCOMPARE(builder.lastSlider.value, 75);
    }

    // Stable documented syntax has no title: options begin immediately after
    // the widget name.
    {
        MockBuilder builder;
        ParserMain parser(&builder);
        parser.processLine(
            "add slider sld_volume minimum 0 maximum 100 vertical value 75");
        QVERIFY(builder.called);
        QCOMPARE(builder.lastSlider.name, QString("sld_volume"));
        QCOMPARE(builder.lastSlider.min, 0);
        QCOMPARE(builder.lastSlider.max, 100);
        QCOMPARE(builder.lastSlider.orientation, 2);
        QCOMPARE(builder.lastSlider.value, 75);
    }

    // Keep accepting the positional legacy range, including zero.
    {
        MockBuilder builder;
        ParserMain parser(&builder);
        parser.processLine("add slider legacy_slider 0 50 horizontal");
        QVERIFY(builder.called);
        QCOMPARE(builder.lastSlider.name, QString("legacy_slider"));
        QCOMPARE(builder.lastSlider.min, 0);
        QCOMPARE(builder.lastSlider.max, 50);
        QCOMPARE(builder.lastSlider.orientation, 1);
    }
}

void TestParser::testParseAddTextBox()
{
    MockBuilder builder;
    ParserMain parser(&builder);
    parser.processLine("add textbox \"Initial Value\" tb1 password placeholder \"Enter secret\"");
    QVERIFY(builder.called);
    QCOMPARE(builder.lastLineEdit.title, QString("Initial Value"));
    QCOMPARE(builder.lastLineEdit.text, QString());
    QCOMPARE(builder.lastLineEdit.passwordMode, true);
    QCOMPARE(builder.lastLineEdit.placeholder, QString("Enter secret"));
}

void TestParser::testParseAddTextView()
{
    MockBuilder builder;
    ParserMain parser(&builder);
    parser.processLine("add textview \"Long content...\" tv1 readonly");
    QVERIFY(builder.called);
    QCOMPARE(builder.lastTextEdit.text, QString("Long content..."));
    QCOMPARE(builder.lastTextEdit.readOnly, true);
}

void TestParser::testParseHierarchicalLayout()
{
    MockBuilder builder;
    ParserMain parser(&builder);
    
    // 1. Create Window (Root)
    parser.processLine("add window \"Main\" win");
    QVERIFY(builder.called);
    
    // 2. Add GroupBox (should be added to Window)
    parser.processLine("add groupbox \"Group\" grp1");
    QCOMPARE(builder.lastGroupBox.title, QString("Group"));
    
    // 3. Add Button (should be added to GroupBox)
    parser.processLine("add button \"Inside\" btn1");
    QCOMPARE(builder.lastButton.text, QString("Inside"));
    
    // 4. End GroupBox context
    parser.processLine("end");
    
    // 5. Add Button (should be added to Window now)
    parser.processLine("add button \"Outside\" btn2");
    QCOMPARE(builder.lastButton.text, QString("Outside"));
}

QTEST_MAIN(TestParser)
#include "tst_parser.moc"

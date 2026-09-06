#include "ShowboxBuilder.h"
#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QFile>
#include <QLineEdit>
#include <QComboBox>
#include <QListWidget>
#include <QTableWidget>
#include <custom_table_widget.h>
#include <QMovie>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QRadioButton>
#include <QCalendarWidget>
#include <QDate>
#include <QFrame>
#include <QIcon>
#include <QSpinBox>
#include <QSlider>
#include <QTextEdit>
#include <QTextStream>
#include <QGroupBox>
#include <QTabWidget>
#include "push_button_widget.h"
#include "custom_chart_widget.h"

ShowboxBuilder::ShowboxBuilder(QObject *parent) : QObject(parent)
{
}

QWidget* ShowboxBuilder::buildWindow(const Showbox::Models::WindowConfig& config)
{
    auto *window = new QDialog();
    window->setWindowTitle(config.title);
    window->resize(config.width, config.height);
    
    // Default layout for new windows to allow immediate adding of widgets
    if (!window->layout()) {
        new QVBoxLayout(window);
    }
    
    return window;
}

QWidget* ShowboxBuilder::buildButton(const Showbox::Models::ButtonConfig& config)
{
    // For now using standard QPushButton, later can use PushButtonWidget if it adds value
    auto *btn = new QPushButton(config.text);
    btn->setObjectName(config.name);
    btn->setCheckable(config.checkable);
    btn->setChecked(config.checked);
    btn->setDefault(config.isDefault);
    if (!config.iconPath.isEmpty())
        btn->setIcon(QIcon::fromTheme(config.iconPath, QIcon(config.iconPath)));
    return btn;
}

QWidget* ShowboxBuilder::buildLabel(const Showbox::Models::LabelConfig& config)
{
    auto *lbl = new QLabel(config.text);
    lbl->setObjectName(config.name);
    lbl->setWordWrap(config.wordWrap);
    if (config.animation && !config.iconPath.isEmpty()) {
        auto *movie = new QMovie(config.iconPath, QByteArray(), lbl);
        lbl->setMovie(movie);
        movie->start();
    } else if (!config.iconPath.isEmpty()) {
        const QPixmap picture(config.iconPath);
        if (!picture.isNull()) lbl->setPixmap(picture);
    }
    return lbl;
}

QWidget* ShowboxBuilder::buildLineEdit(const Showbox::Models::LineEditConfig& config)
{
    auto *container = new QWidget();
    container->setObjectName(config.name);
    container->setProperty("showboxTextBox", true);
    auto *layout = new QHBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);
    auto *label = new QLabel(config.title, container);
    label->setProperty("showboxTitleLabel", true);
    auto *le = new QLineEdit(config.text);
    le->setObjectName(config.name + "_input");
    le->setPlaceholderText(config.placeholder);
    if (config.passwordMode) {
        le->setEchoMode(QLineEdit::Password);
    }
    label->setBuddy(le);
    container->setFocusProxy(le);
    layout->addWidget(label);
    layout->addWidget(le);
    return container;
}

QWidget* ShowboxBuilder::buildComboBox(const Showbox::Models::ComboBoxConfig& config)
{
    auto *container = new QWidget();
    container->setObjectName(config.name);
    container->setProperty("showboxComboBox", true);
    auto *layout = new QHBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);
    auto *label = new QLabel(config.title, container);
    label->setProperty("showboxTitleLabel", true);
    auto *cb = new QComboBox(container);
    cb->setObjectName(config.name + "_input");
    cb->addItems(config.items);
    cb->setEditable(config.editable);
    if (config.currentIndex >= 0 && config.currentIndex < cb->count()) {
        cb->setCurrentIndex(config.currentIndex);
    }
    label->setBuddy(cb);
    container->setFocusProxy(cb);
    layout->addWidget(label);
    layout->addWidget(cb);
    return container;
}

QWidget* ShowboxBuilder::buildList(const Showbox::Models::ListConfig& config)
{
    auto *container = new QWidget();
    container->setObjectName(config.name);
    container->setProperty("showboxListBox", true);
    auto *layout = new QVBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);
    auto *label = new QLabel(config.title, container);
    label->setProperty("showboxTitleLabel", true);
    auto *lw = new QListWidget(container);
    lw->setObjectName(config.name + "_input");
    lw->addItems(config.items);
    if (config.multipleSelection) {
        lw->setSelectionMode(QAbstractItemView::MultiSelection);
    }
    label->setBuddy(lw);
    container->setFocusProxy(lw);
    layout->addWidget(label);
    layout->addWidget(lw);
    return container;
}

QWidget* ShowboxBuilder::buildTable(const Showbox::Models::TableConfig& config)
{
    auto *widget = new CustomTableWidget();
    auto *tw = widget->table();
    widget->setObjectName(config.name);
    tw->setObjectName(config.name);
    tw->setColumnCount(config.headers.size());
    tw->setHorizontalHeaderLabels(config.headers);
    tw->setRowCount(config.rows.size());
    if (config.readOnly)
        tw->setEditTriggers(QAbstractItemView::NoEditTriggers);
    if (config.selection)
        tw->setSelectionBehavior(QAbstractItemView::SelectRows);
    for (int r = 0; r < config.rows.size(); ++r) {
        for (int c = 0; c < config.rows[r].size() && c < config.headers.size(); ++c) {
            tw->setItem(r, c, new QTableWidgetItem(config.rows[r][c]));
        }
    }
    widget->setSearchVisible(config.search);
    if (!config.file.isEmpty())
        widget->loadFromFile(config.file);
    return widget;
}

QWidget* ShowboxBuilder::buildProgressBar(const Showbox::Models::ProgressBarConfig& config)
{
    auto *pb = new QProgressBar();
    pb->setObjectName(config.name);
    pb->setMinimum(config.minimum);
    pb->setMaximum(config.busy ? 0 : config.maximum);
    pb->setValue(config.value);
    pb->setFormat(config.format);
    pb->setOrientation(static_cast<Qt::Orientation>(config.orientation));
    return pb;
}

QWidget* ShowboxBuilder::buildChart(const Showbox::Models::ChartConfig& config)
{
    auto *chart = new CustomChartWidget();
    chart->setObjectName(config.name);
    chart->setChartTitle(config.title);
    
    // Populate chart data from config
    for (auto it = config.data.begin(); it != config.data.end(); ++it) {
        chart->addPoint(it.key(), it.value());
    }
    
    return chart;
}

QWidget* ShowboxBuilder::buildCheckBox(const Showbox::Models::CheckBoxConfig& config)
{
    auto *cb = new QCheckBox(config.text);
    cb->setObjectName(config.name);
    cb->setChecked(config.checked);
    return cb;
}

QWidget* ShowboxBuilder::buildRadioButton(const Showbox::Models::RadioButtonConfig& config)
{
    auto *rb = new QRadioButton(config.text);
    rb->setObjectName(config.name);
    rb->setChecked(config.checked);
    return rb;
}

QWidget* ShowboxBuilder::buildCalendar(const Showbox::Models::CalendarConfig& config)
{
    auto *cw = new QCalendarWidget();
    cw->setObjectName(config.name);
    const QDate selected = QDate::fromString(config.date, Qt::ISODate);
    const QDate minimum = QDate::fromString(config.minimum, Qt::ISODate);
    const QDate maximum = QDate::fromString(config.maximum, Qt::ISODate);
    if (selected.isValid()) cw->setSelectedDate(selected);
    if (minimum.isValid()) cw->setMinimumDate(minimum);
    if (maximum.isValid()) cw->setMaximumDate(maximum);
    cw->setNavigationBarVisible(config.navigation);
    cw->setProperty("showboxDateFormat", config.format);
    return cw;
}

QWidget* ShowboxBuilder::buildSeparator(const Showbox::Models::SeparatorConfig& config)
{
    auto *line = new QFrame();
    line->setObjectName(config.name);
    if (config.orientation == Qt::Horizontal) {
        line->setFrameShape(QFrame::HLine);
    } else {
        line->setFrameShape(QFrame::VLine);
    }
    line->setFrameShadow(static_cast<QFrame::Shadow>(config.shadow));
    return line;
}

QWidget* ShowboxBuilder::buildSpinBox(const Showbox::Models::SpinBoxConfig& config)
{
    auto *sb = new QSpinBox();
    sb->setObjectName(config.name);
    sb->setMinimum(config.min);
    sb->setMaximum(config.max);
    sb->setValue(config.value);
    sb->setSingleStep(config.step);
    sb->setPrefix(config.prefix);
    sb->setSuffix(config.suffix);
    return sb;
}

QWidget* ShowboxBuilder::buildSlider(const Showbox::Models::SliderConfig& config)
{
    auto *sl = new QSlider();
    sl->setObjectName(config.name);
    sl->setMinimum(config.min);
    sl->setMaximum(config.max);
    sl->setValue(config.value);
    sl->setOrientation(static_cast<Qt::Orientation>(config.orientation));
    return sl;
}

QWidget* ShowboxBuilder::buildTextEdit(const Showbox::Models::TextEditConfig& config)
{
    auto *te = new QTextEdit();
    te->setObjectName(config.name);
    if (config.richText) {
        te->setHtml(config.text);
    } else {
        te->setPlainText(config.text);
    }
    if (!config.file.isEmpty()) {
        QFile file(config.file);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
            te->setPlainText(QTextStream(&file).readAll());
    }
    te->setReadOnly(config.readOnly);
    return te;
}

QWidget* ShowboxBuilder::buildGroupBox(const Showbox::Models::GroupBoxConfig& config)
{
    auto *gb = new QGroupBox(config.title);
    gb->setObjectName(config.name);
    gb->setCheckable(config.checkable);
    gb->setChecked(config.checked);
    
    // Build and set layout if valid
    QLayout* layout = buildLayout(config.layout);
    if (layout) {
        gb->setLayout(layout);
    }
    
    return gb;
}

QWidget* ShowboxBuilder::buildFrame(const Showbox::Models::FrameConfig& config)
{
    auto *frame = new QFrame();
    frame->setObjectName(config.name);
    frame->setFrameShape(static_cast<QFrame::Shape>(config.shape));
    frame->setFrameShadow(static_cast<QFrame::Shadow>(config.shadow));
    
    // Build and set layout if valid
    QLayout* layout = buildLayout(config.layout);
    if (layout) {
        frame->setLayout(layout);
    }
    
    return frame;
}

QWidget* ShowboxBuilder::buildTabWidget(const Showbox::Models::TabWidgetConfig& config)
{
    auto *tabWidget = new QTabWidget();
    tabWidget->setObjectName(config.name);
    tabWidget->setTabPosition(static_cast<QTabWidget::TabPosition>(config.position));
    
    for (const auto& pageConfig : config.pages) {
        auto *page = new QWidget();
        page->setObjectName(pageConfig.name);
        
        QLayout* layout = buildLayout(pageConfig.layout);
        if (layout) {
            page->setLayout(layout);
        }
        
        tabWidget->addTab(page, pageConfig.title);
    }
    
    return tabWidget;
}

QLayout* ShowboxBuilder::buildLayout(const Showbox::Models::LayoutConfig& config)
{
    QLayout *layout = nullptr;
    switch (config.type) {
        case Showbox::Models::LayoutConfig::VBox:
            layout = new QVBoxLayout();
            break;
        case Showbox::Models::LayoutConfig::HBox:
            layout = new QHBoxLayout();
            break;
        default:
            layout = new QVBoxLayout();
            break;
    }
    
    if (layout) {
        layout->setSpacing(config.spacing);
        layout->setContentsMargins(config.margin, config.margin, config.margin, config.margin);
    }
    
    return layout;
}

PushButtonWidget* ShowboxBuilder::buildPushButton(const QString &title, const QString &name)
{
    auto *button = new PushButtonWidget(title);
    button->setObjectName(name);
    return button;
}

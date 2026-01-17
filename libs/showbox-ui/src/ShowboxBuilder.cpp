#include "ShowboxBuilder.h"
#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QListWidget>
#include <QTableWidget>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QRadioButton>
#include <QCalendarWidget>
#include <QFrame>
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
    return btn;
}

QWidget* ShowboxBuilder::buildLabel(const Showbox::Models::LabelConfig& config)
{
    auto *lbl = new QLabel(config.text);
    lbl->setObjectName(config.name);
    lbl->setWordWrap(config.wordWrap);
    return lbl;
}

QWidget* ShowboxBuilder::buildLineEdit(const Showbox::Models::LineEditConfig& config)
{
    auto *le = new QLineEdit(config.text);
    le->setObjectName(config.name);
    le->setPlaceholderText(config.placeholder);
    if (config.passwordMode) {
        le->setEchoMode(QLineEdit::Password);
    }
    return le;
}

QWidget* ShowboxBuilder::buildComboBox(const Showbox::Models::ComboBoxConfig& config)
{
    auto *cb = new QComboBox();
    cb->setObjectName(config.name);
    cb->addItems(config.items);
    if (config.currentIndex >= 0 && config.currentIndex < cb->count()) {
        cb->setCurrentIndex(config.currentIndex);
    }
    return cb;
}

QWidget* ShowboxBuilder::buildList(const Showbox::Models::ListConfig& config)
{
    auto *lw = new QListWidget();
    lw->setObjectName(config.name);
    lw->addItems(config.items);
    if (config.multipleSelection) {
        lw->setSelectionMode(QAbstractItemView::MultiSelection);
    }
    return lw;
}

QWidget* ShowboxBuilder::buildTable(const Showbox::Models::TableConfig& config)
{
    auto *tw = new QTableWidget();
    tw->setObjectName(config.name);
    tw->setColumnCount(config.headers.size());
    tw->setHorizontalHeaderLabels(config.headers);
    tw->setRowCount(config.rows.size());
    for (int r = 0; r < config.rows.size(); ++r) {
        for (int c = 0; c < config.rows[r].size() && c < config.headers.size(); ++c) {
            tw->setItem(r, c, new QTableWidgetItem(config.rows[r][c]));
        }
    }
    return tw;
}

QWidget* ShowboxBuilder::buildProgressBar(const Showbox::Models::ProgressBarConfig& config)
{
    auto *pb = new QProgressBar();
    pb->setObjectName(config.name);
    pb->setMinimum(config.minimum);
    pb->setMaximum(config.maximum);
    pb->setValue(config.value);
    pb->setFormat(config.format);
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
    line->setFrameShadow(QFrame::Sunken);
    return line;
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
    // Implementation for legacy support using the new config-based methods
    Showbox::Models::ButtonConfig config;
    config.text = title;
    config.name = name;
    
    // Note: buildButton currently returns QPushButton, but legacy expects PushButtonWidget.
    // This highlights that we might need to migrate the implementation details later.
    // For now, this stub exists to satisfy the interface.
    return nullptr; 
}

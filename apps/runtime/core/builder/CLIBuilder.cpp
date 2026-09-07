#include "CLIBuilder.h"

#include <QPushButton>
#include <push_button_widget.h>

CLIBuilder::CLIBuilder(QObject *parent)
    : QObject(parent), m_window(new QDialog()), m_layout(new QVBoxLayout(m_window)),
      m_factory(this) {
    m_window->setObjectName("showbox");
    m_window->setWindowTitle("Showbox");
}

void CLIBuilder::show() {
    m_window->show();
}

QWidget *CLIBuilder::attach(QWidget *widget) {
    if (widget && !widget->parentWidget()) {
        m_layout->addWidget(widget);
    }
    return widget;
}

PushButtonWidget *CLIBuilder::buildPushButton(const QString &title,
                                              const QString &name) {
    auto *button = m_factory.buildPushButton(title, name);
    attach(button);
    return button;
}

QWidget *CLIBuilder::buildWindow(const Showbox::Models::WindowConfig &config) {
    m_window->setObjectName(config.name.isEmpty() ? "showbox" : config.name);
    m_window->setWindowTitle(config.title);
    m_window->resize(config.width, config.height);
    return m_window;
}

#define SHOWBOX_DELEGATE(method, ConfigType)                                      \
    QWidget *CLIBuilder::method(const Showbox::Models::ConfigType &config) {      \
        return attach(m_factory.method(config));                                  \
    }

SHOWBOX_DELEGATE(buildButton, ButtonConfig)
SHOWBOX_DELEGATE(buildLabel, LabelConfig)
SHOWBOX_DELEGATE(buildLineEdit, LineEditConfig)
SHOWBOX_DELEGATE(buildComboBox, ComboBoxConfig)
SHOWBOX_DELEGATE(buildList, ListConfig)
SHOWBOX_DELEGATE(buildTable, TableConfig)
SHOWBOX_DELEGATE(buildProgressBar, ProgressBarConfig)
SHOWBOX_DELEGATE(buildChart, ChartConfig)
SHOWBOX_DELEGATE(buildCheckBox, CheckBoxConfig)
SHOWBOX_DELEGATE(buildRadioButton, RadioButtonConfig)
SHOWBOX_DELEGATE(buildCalendar, CalendarConfig)
SHOWBOX_DELEGATE(buildSeparator, SeparatorConfig)
SHOWBOX_DELEGATE(buildSpinBox, SpinBoxConfig)
SHOWBOX_DELEGATE(buildSlider, SliderConfig)
SHOWBOX_DELEGATE(buildTextEdit, TextEditConfig)
SHOWBOX_DELEGATE(buildGroupBox, GroupBoxConfig)
SHOWBOX_DELEGATE(buildFrame, FrameConfig)
SHOWBOX_DELEGATE(buildTabWidget, TabWidgetConfig)

#undef SHOWBOX_DELEGATE

QLayout *CLIBuilder::buildLayout(const Showbox::Models::LayoutConfig &config) {
    return m_factory.buildLayout(config);
}

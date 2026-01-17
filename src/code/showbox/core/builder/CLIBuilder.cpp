#include "CLIBuilder.h"
#include <push_button_widget.h>
#include <widget_factory.h>

CLIBuilder::CLIBuilder(QObject *parent) : QObject(parent)
{
    m_window = new QDialog();
    m_window->setWindowTitle("Showbox Target Architecture");
    m_layout = new QVBoxLayout(m_window);
}

PushButtonWidget* CLIBuilder::buildPushButton(const QString &title, const QString &name)
{
    QWidget *w = WidgetFactory::create("pushbutton");
    auto *btn = qobject_cast<PushButtonWidget*>(w);
    if (btn) {
        btn->setText(title);
        btn->setObjectName(name);
        m_layout->addWidget(btn);
        m_window->show();
    }
    return btn;
}

QWidget* CLIBuilder::buildWindow(const Showbox::Models::WindowConfig& config) {
    Q_UNUSED(config);
    return nullptr;
}

QWidget* CLIBuilder::buildButton(const Showbox::Models::ButtonConfig& config) {
    Q_UNUSED(config);
    return nullptr;
}

QWidget* CLIBuilder::buildLabel(const Showbox::Models::LabelConfig& config) {

    Q_UNUSED(config);

    return nullptr;

}



QWidget* CLIBuilder::buildLineEdit(const Showbox::Models::LineEditConfig& config) {



    Q_UNUSED(config);



    return nullptr;



}







QWidget* CLIBuilder::buildComboBox(const Showbox::Models::ComboBoxConfig& config) {



    Q_UNUSED(config);



    return nullptr;



}







QWidget* CLIBuilder::buildList(const Showbox::Models::ListConfig& config) {



    Q_UNUSED(config);



    return nullptr;



}







QWidget* CLIBuilder::buildTable(const Showbox::Models::TableConfig& config) {







    Q_UNUSED(config);







    return nullptr;







}















QWidget* CLIBuilder::buildProgressBar(const Showbox::Models::ProgressBarConfig& config) {







    Q_UNUSED(config);







    return nullptr;







}















QWidget* CLIBuilder::buildChart(const Showbox::Models::ChartConfig& config) {















    Q_UNUSED(config);















    return nullptr;















}































QWidget* CLIBuilder::buildCheckBox(const Showbox::Models::CheckBoxConfig& config) {















    Q_UNUSED(config);















    return nullptr;















}































QWidget* CLIBuilder::buildRadioButton(const Showbox::Models::RadioButtonConfig& config) {















    Q_UNUSED(config);















    return nullptr;















}































QWidget* CLIBuilder::buildCalendar(const Showbox::Models::CalendarConfig& config) {















    Q_UNUSED(config);















    return nullptr;















}































QWidget* CLIBuilder::buildSeparator(const Showbox::Models::SeparatorConfig& config) {















    Q_UNUSED(config);















    return nullptr;















}































QLayout* CLIBuilder::buildLayout(const Showbox::Models::LayoutConfig& config) {















    Q_UNUSED(config);

    return nullptr;

}

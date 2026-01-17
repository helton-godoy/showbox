#ifndef SHOWBOXBUILDER_H
#define SHOWBOXBUILDER_H

#include "IShowboxBuilder.h"
#include <QObject>

class ShowboxBuilder : public QObject, public IShowboxBuilder
{
    Q_OBJECT
public:
    explicit ShowboxBuilder(QObject *parent = nullptr);

    QWidget* buildWindow(const Showbox::Models::WindowConfig& config) override;
    QWidget* buildButton(const Showbox::Models::ButtonConfig& config) override;
    QWidget* buildLabel(const Showbox::Models::LabelConfig& config) override;
    QWidget* buildLineEdit(const Showbox::Models::LineEditConfig& config) override;
    QWidget* buildComboBox(const Showbox::Models::ComboBoxConfig& config) override;
    QWidget* buildList(const Showbox::Models::ListConfig& config) override;
    QWidget* buildTable(const Showbox::Models::TableConfig& config) override;
    QWidget* buildProgressBar(const Showbox::Models::ProgressBarConfig& config) override;
    QWidget* buildChart(const Showbox::Models::ChartConfig& config) override;
    QWidget* buildCheckBox(const Showbox::Models::CheckBoxConfig& config) override;
    QWidget* buildRadioButton(const Showbox::Models::RadioButtonConfig& config) override;
    QWidget* buildCalendar(const Showbox::Models::CalendarConfig& config) override;
    QWidget* buildSeparator(const Showbox::Models::SeparatorConfig& config) override;
    QLayout* buildLayout(const Showbox::Models::LayoutConfig& config) override;

    // Legacy support
    PushButtonWidget* buildPushButton(const QString &title, const QString &name) override;
};

#endif // SHOWBOXBUILDER_H

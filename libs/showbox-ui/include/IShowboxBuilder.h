#ifndef ISHOWBOXBUILDER_H
#define ISHOWBOXBUILDER_H

#include <QWidget>
#include <QLayout>
#include "WidgetConfigs.h"

// Forward declaration if needed for legacy support
class PushButtonWidget;

class IShowboxBuilder
{
public:
    virtual ~IShowboxBuilder() = default;

    // --- New Architecture Methods (Phase 1 & 2) ---
    
    virtual QWidget* buildWindow(const Showbox::Models::WindowConfig& config) = 0;
    virtual QWidget* buildButton(const Showbox::Models::ButtonConfig& config) = 0;
    virtual QWidget* buildLabel(const Showbox::Models::LabelConfig& config) = 0;
    virtual QWidget* buildLineEdit(const Showbox::Models::LineEditConfig& config) = 0;
    virtual QWidget* buildComboBox(const Showbox::Models::ComboBoxConfig& config) = 0;
    virtual QWidget* buildList(const Showbox::Models::ListConfig& config) = 0;
    virtual QWidget* buildTable(const Showbox::Models::TableConfig& config) = 0;
    virtual QWidget* buildProgressBar(const Showbox::Models::ProgressBarConfig& config) = 0;
    virtual QWidget* buildChart(const Showbox::Models::ChartConfig& config) = 0;
    virtual QWidget* buildCheckBox(const Showbox::Models::CheckBoxConfig& config) = 0;
    virtual QWidget* buildRadioButton(const Showbox::Models::RadioButtonConfig& config) = 0;
    virtual QWidget* buildCalendar(const Showbox::Models::CalendarConfig& config) = 0;
    virtual QWidget* buildSeparator(const Showbox::Models::SeparatorConfig& config) = 0;
    virtual QLayout* buildLayout(const Showbox::Models::LayoutConfig& config) = 0;

    // --- Legacy Methods (To be deprecated/removed) ---
    virtual PushButtonWidget* buildPushButton(const QString &title, const QString &name) = 0;
};

#endif // ISHOWBOXBUILDER_H
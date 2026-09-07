#ifndef CLIBUILDER_H
#define CLIBUILDER_H

#include <IShowboxBuilder.h>
#include <ShowboxBuilder.h>
#include <QDialog>
#include <QObject>
#include <QVBoxLayout>

class CLIBuilder final : public QObject, public IShowboxBuilder {
    Q_OBJECT

public:
    explicit CLIBuilder(QObject *parent = nullptr);

    QWidget *window() const { return m_window; }
    void show();

    PushButtonWidget *buildPushButton(const QString &title,
                                      const QString &name) override;
    QWidget *buildWindow(const Showbox::Models::WindowConfig &config) override;
    QWidget *buildButton(const Showbox::Models::ButtonConfig &config) override;
    QWidget *buildLabel(const Showbox::Models::LabelConfig &config) override;
    QWidget *buildLineEdit(const Showbox::Models::LineEditConfig &config) override;
    QWidget *buildComboBox(const Showbox::Models::ComboBoxConfig &config) override;
    QWidget *buildList(const Showbox::Models::ListConfig &config) override;
    QWidget *buildTable(const Showbox::Models::TableConfig &config) override;
    QWidget *buildProgressBar(const Showbox::Models::ProgressBarConfig &config) override;
    QWidget *buildChart(const Showbox::Models::ChartConfig &config) override;
    QWidget *buildCheckBox(const Showbox::Models::CheckBoxConfig &config) override;
    QWidget *buildRadioButton(const Showbox::Models::RadioButtonConfig &config) override;
    QWidget *buildCalendar(const Showbox::Models::CalendarConfig &config) override;
    QWidget *buildSeparator(const Showbox::Models::SeparatorConfig &config) override;
    QWidget *buildSpinBox(const Showbox::Models::SpinBoxConfig &config) override;
    QWidget *buildSlider(const Showbox::Models::SliderConfig &config) override;
    QWidget *buildTextEdit(const Showbox::Models::TextEditConfig &config) override;
    QWidget *buildGroupBox(const Showbox::Models::GroupBoxConfig &config) override;
    QWidget *buildFrame(const Showbox::Models::FrameConfig &config) override;
    QWidget *buildTabWidget(const Showbox::Models::TabWidgetConfig &config) override;
    QLayout *buildLayout(const Showbox::Models::LayoutConfig &config) override;

private:
    QWidget *attach(QWidget *widget);

    QDialog *m_window;
    QVBoxLayout *m_layout;
    ShowboxBuilder m_factory;
};

#endif // CLIBUILDER_H

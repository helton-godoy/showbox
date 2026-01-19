#ifndef CLIBUILDER_H
#define CLIBUILDER_H

#include <IShowboxBuilder.h>
#include <QDialog>
#include <QObject>
#include <QVBoxLayout>
#include <QVariant>

class CLIBuilder : public QObject, public IShowboxBuilder {
  Q_OBJECT
public:
  explicit CLIBuilder(QObject *parent = nullptr);

  void show() {
    if (m_window)
      m_window->show();
  }

  // Legacy
  PushButtonWidget *buildPushButton(const QString &title,
                                    const QString &name) override;

  // New Architecture
  QWidget *buildWindow(const Showbox::Models::WindowConfig &config) override;
  QWidget *buildButton(const Showbox::Models::ButtonConfig &config) override;
  QWidget *buildLabel(const Showbox::Models::LabelConfig &config) override;
  QWidget *
  buildLineEdit(const Showbox::Models::LineEditConfig &config) override;
  QWidget *
  buildComboBox(const Showbox::Models::ComboBoxConfig &config) override;
  QWidget *buildList(const Showbox::Models::ListConfig &config) override;
  QWidget *buildTable(const Showbox::Models::TableConfig &config) override;
  QWidget *
  buildProgressBar(const Showbox::Models::ProgressBarConfig &config) override;
  QWidget *buildChart(const Showbox::Models::ChartConfig &config) override;
  QWidget *
  buildCheckBox(const Showbox::Models::CheckBoxConfig &config) override;
  QWidget *
  buildRadioButton(const Showbox::Models::RadioButtonConfig &config) override;
  QWidget *
  buildCalendar(const Showbox::Models::CalendarConfig &config) override;
  QWidget *
  buildSeparator(const Showbox::Models::SeparatorConfig &config) override;
  QWidget *buildSpinBox(const Showbox::Models::SpinBoxConfig &config) override;
  QWidget *buildSlider(const Showbox::Models::SliderConfig &config) override;
  QWidget *
  buildTextEdit(const Showbox::Models::TextEditConfig &config) override;
  QWidget *
  buildGroupBox(const Showbox::Models::GroupBoxConfig &config) override;
  QWidget *buildFrame(const Showbox::Models::FrameConfig &config) override;
  QWidget *
  buildTabWidget(const Showbox::Models::TabWidgetConfig &config) override;
  QLayout *buildLayout(const Showbox::Models::LayoutConfig &config) override;

private:
  QDialog *m_window;
  QVBoxLayout *m_layout;
  QMap<QWidget *, QString> m_widgetCallbacks; // widget -> callback name

  // Para criação thread-safe de botões
  QWidget *m_lastCreatedWidget = nullptr;

private slots:
  void createButtonInMainThread(const QString &text, const QString &name,
                                bool checkable, bool checked,
                                const QString &callback,
                                const QString &iconPath);

  // Basic Widgets
  void createLabelInMainThread(const QString &text, const QString &name,
                               bool wordWrap, const QString &iconPath);
  void createLineEditInMainThread(const QString &text, const QString &name,
                                  const QString &placeholder, bool password);
  void createTextEditInMainThread(const QString &text, const QString &name,
                                  bool readOnly);
  void createCheckBoxInMainThread(const QString &text, const QString &name,
                                  bool checked);
  void createRadioButtonInMainThread(const QString &text, const QString &name,
                                     bool checked);
  void createSeparatorInMainThread(const QString &name, int orientation);

  // Numeric/Progress
  void createSpinBoxInMainThread(const QString &name, int value, int min,
                                 int max, int step);
  void createSliderInMainThread(const QString &name, int value, int min,
                                int max, int orientation);
  void createProgressBarInMainThread(const QString &name, int value, int min,
                                     int max, const QString &format);

  // Selection
  void createComboBoxInMainThread(const QString &name,
                                  const QStringList &items);
  void createListInMainThread(const QString &name, const QStringList &items,
                              bool multiple);

  // Complex
  void createTableInMainThread(const QString &name, const QStringList &headers,
                               const QVariant &rowsData);
  void createCalendarInMainThread(const QString &name);
  void createChartInMainThread(const QString &name, const QString &title,
                               int type, const QVariant &dataMap);

  // Containers
  void createGroupBoxInMainThread(const QString &name, const QString &title,
                                  int layoutType);
  void createFrameInMainThread(const QString &name, int layoutType);
  void createTabWidgetInMainThread(const QString &name);
};

#endif // CLIBUILDER_H

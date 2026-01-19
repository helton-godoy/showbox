#include "CLIBuilder.h"
#include <iostream>
#include <push_button_widget.h>
#include <widget_factory.h>

// Qt Widgets Includes
#include <QCalendarWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QFrame>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QSlider>
#include <QSpinBox>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextEdit>

// Charts Includes
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QPieSeries>
#include <QtCharts/QValueAxis>

// using namespace QtCharts;

CLIBuilder::CLIBuilder(QObject *parent) : QObject(parent) {
  m_window = new QDialog();
  m_window->setWindowTitle("Showbox Target Architecture");
  m_layout = new QVBoxLayout(m_window);
}

// Legacy Implementation
PushButtonWidget *CLIBuilder::buildPushButton(const QString &title,
                                              const QString &name) {
  QWidget *w = WidgetFactory::create("pushbutton");
  auto *btn = qobject_cast<PushButtonWidget *>(w);
  if (btn) {
    btn->setText(title);
    btn->setObjectName(name);
    m_layout->addWidget(btn);
  }
  return btn;
}

// New Architecture

// Window
QWidget *CLIBuilder::buildWindow(const Showbox::Models::WindowConfig &config) {
  m_window->setWindowTitle(config.title);
  m_window->resize(config.width, config.height);
  m_window->setObjectName(config.name);
  return m_window;
}

// Button
QWidget *CLIBuilder::buildButton(const Showbox::Models::ButtonConfig &config) {
  QString callbackName;
  if (config.actions.hasActions()) {
    auto clickedActions = config.actions.actionsFor("clicked");
    for (const auto &action : clickedActions) {
      if (action.type == Showbox::Models::ActionConfig::Callback) {
        callbackName = action.command;
        break;
      }
    }
  }

  m_lastCreatedWidget = nullptr;
  QMetaObject::invokeMethod(
      this, "createButtonInMainThread", Qt::BlockingQueuedConnection,
      Q_ARG(QString, config.text), Q_ARG(QString, config.name),
      Q_ARG(bool, config.checkable), Q_ARG(bool, config.checked),
      Q_ARG(QString, callbackName), Q_ARG(QString, config.iconPath));
  return m_lastCreatedWidget;
}

void CLIBuilder::createButtonInMainThread(const QString &text,
                                          const QString &name, bool checkable,
                                          bool checked, const QString &callback,
                                          const QString &iconPath) {
  QPushButton *btn = new QPushButton(text);
  btn->setObjectName(name);
  btn->setCheckable(checkable);
  btn->setChecked(checked);

  if (!iconPath.isEmpty()) {
    btn->setIcon(QIcon(iconPath));
  }

  if (!callback.isEmpty()) {
    m_widgetCallbacks[btn] = callback;
    connect(btn, &QPushButton::clicked, this, [this, btn]() {
      QString cb = m_widgetCallbacks.value(btn);
      if (!cb.isEmpty()) {
        std::cout << "CALLBACK:" << cb.toStdString() << std::endl;
        std::cout.flush();
      }
    });
  }
  m_layout->addWidget(btn);
  m_lastCreatedWidget = btn;
}

// Label
QWidget *CLIBuilder::buildLabel(const Showbox::Models::LabelConfig &config) {
  m_lastCreatedWidget = nullptr;
  QMetaObject::invokeMethod(
      this, "createLabelInMainThread", Qt::BlockingQueuedConnection,
      Q_ARG(QString, config.text), Q_ARG(QString, config.name),
      Q_ARG(bool, config.wordWrap), Q_ARG(QString, config.iconPath));
  return m_lastCreatedWidget;
}

void CLIBuilder::createLabelInMainThread(const QString &text,
                                         const QString &name, bool wordWrap,
                                         const QString &iconPath) {
  QLabel *lbl = new QLabel(text);
  lbl->setObjectName(name);
  lbl->setWordWrap(wordWrap);
  if (!iconPath.isEmpty()) {
    QPixmap pix(iconPath);
    if (!pix.isNull()) {
      lbl->setPixmap(pix);
    }
  }
  m_layout->addWidget(lbl);
  m_lastCreatedWidget = lbl;
}

// LineEdit
QWidget *
CLIBuilder::buildLineEdit(const Showbox::Models::LineEditConfig &config) {
  m_lastCreatedWidget = nullptr;
  QMetaObject::invokeMethod(
      this, "createLineEditInMainThread", Qt::BlockingQueuedConnection,
      Q_ARG(QString, config.text), Q_ARG(QString, config.name),
      Q_ARG(QString, config.placeholder), Q_ARG(bool, config.passwordMode));
  return m_lastCreatedWidget;
}

void CLIBuilder::createLineEditInMainThread(const QString &text,
                                            const QString &name,
                                            const QString &placeholder,
                                            bool password) {
  QLineEdit *le = new QLineEdit(text);
  le->setObjectName(name);
  le->setPlaceholderText(placeholder);
  if (password)
    le->setEchoMode(QLineEdit::Password);
  m_layout->addWidget(le);
  m_lastCreatedWidget = le;
}

// CheckBox
QWidget *
CLIBuilder::buildCheckBox(const Showbox::Models::CheckBoxConfig &config) {
  m_lastCreatedWidget = nullptr;
  QMetaObject::invokeMethod(
      this, "createCheckBoxInMainThread", Qt::BlockingQueuedConnection,
      Q_ARG(QString, config.text), Q_ARG(QString, config.name),
      Q_ARG(bool, config.checked));
  return m_lastCreatedWidget;
}

void CLIBuilder::createCheckBoxInMainThread(const QString &text,
                                            const QString &name, bool checked) {
  QCheckBox *cb = new QCheckBox(text);
  cb->setObjectName(name);
  cb->setChecked(checked);
  m_layout->addWidget(cb);
  m_lastCreatedWidget = cb;
}

// RadioButton
QWidget *
CLIBuilder::buildRadioButton(const Showbox::Models::RadioButtonConfig &config) {
  m_lastCreatedWidget = nullptr;
  QMetaObject::invokeMethod(
      this, "createRadioButtonInMainThread", Qt::BlockingQueuedConnection,
      Q_ARG(QString, config.text), Q_ARG(QString, config.name),
      Q_ARG(bool, config.checked));
  return m_lastCreatedWidget;
}

void CLIBuilder::createRadioButtonInMainThread(const QString &text,
                                               const QString &name,
                                               bool checked) {
  QRadioButton *rb = new QRadioButton(text);
  rb->setObjectName(name);
  rb->setChecked(checked);
  m_layout->addWidget(rb);
  m_lastCreatedWidget = rb;
}

// Separator
QWidget *
CLIBuilder::buildSeparator(const Showbox::Models::SeparatorConfig &config) {
  m_lastCreatedWidget = nullptr;
  QMetaObject::invokeMethod(
      this, "createSeparatorInMainThread", Qt::BlockingQueuedConnection,
      Q_ARG(QString, config.name), Q_ARG(int, config.orientation));
  return m_lastCreatedWidget;
}

void CLIBuilder::createSeparatorInMainThread(const QString &name,
                                             int orientation) {
  QFrame *line = new QFrame();
  line->setObjectName(name);
  line->setFrameShape(orientation == 2 ? QFrame::VLine : QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);
  m_layout->addWidget(line);
  m_lastCreatedWidget = line;
}

// TextEdit
QWidget *
CLIBuilder::buildTextEdit(const Showbox::Models::TextEditConfig &config) {
  m_lastCreatedWidget = nullptr;
  QMetaObject::invokeMethod(
      this, "createTextEditInMainThread", Qt::BlockingQueuedConnection,
      Q_ARG(QString, config.text), Q_ARG(QString, config.name),
      Q_ARG(bool, config.readOnly));
  return m_lastCreatedWidget;
}

void CLIBuilder::createTextEditInMainThread(const QString &text,
                                            const QString &name,
                                            bool readOnly) {
  QTextEdit *te = new QTextEdit();
  te->setObjectName(name);
  te->setPlainText(text);
  te->setReadOnly(readOnly);
  m_layout->addWidget(te);
  m_lastCreatedWidget = te;
}

// ComboBox
QWidget *
CLIBuilder::buildComboBox(const Showbox::Models::ComboBoxConfig &config) {
  m_lastCreatedWidget = nullptr;
  QMetaObject::invokeMethod(
      this, "createComboBoxInMainThread", Qt::BlockingQueuedConnection,
      Q_ARG(QString, config.name), Q_ARG(QStringList, config.items));
  return m_lastCreatedWidget;
}

void CLIBuilder::createComboBoxInMainThread(const QString &name,
                                            const QStringList &items) {
  QComboBox *cb = new QComboBox();
  cb->setObjectName(name);
  cb->addItems(items);
  m_layout->addWidget(cb);
  m_lastCreatedWidget = cb;
}

// List
QWidget *CLIBuilder::buildList(const Showbox::Models::ListConfig &config) {
  m_lastCreatedWidget = nullptr;
  QMetaObject::invokeMethod(
      this, "createListInMainThread", Qt::BlockingQueuedConnection,
      Q_ARG(QString, config.name), Q_ARG(QStringList, config.items),
      Q_ARG(bool, config.multipleSelection));
  return m_lastCreatedWidget;
}

void CLIBuilder::createListInMainThread(const QString &name,
                                        const QStringList &items,
                                        bool multiple) {
  QListWidget *lw = new QListWidget();
  lw->setObjectName(name);
  lw->addItems(items);
  if (multiple)
    lw->setSelectionMode(QAbstractItemView::MultiSelection);
  m_layout->addWidget(lw);
  m_lastCreatedWidget = lw;
}

// SpinBox
QWidget *
CLIBuilder::buildSpinBox(const Showbox::Models::SpinBoxConfig &config) {
  m_lastCreatedWidget = nullptr;
  QMetaObject::invokeMethod(
      this, "createSpinBoxInMainThread", Qt::BlockingQueuedConnection,
      Q_ARG(QString, config.name), Q_ARG(int, config.value),
      Q_ARG(int, config.min), Q_ARG(int, config.max), Q_ARG(int, config.step));
  return m_lastCreatedWidget;
}

void CLIBuilder::createSpinBoxInMainThread(const QString &name, int value,
                                           int min, int max, int step) {
  QSpinBox *sb = new QSpinBox();
  sb->setObjectName(name);
  sb->setRange(min, max);
  sb->setValue(value);
  sb->setSingleStep(step);
  m_layout->addWidget(sb);
  m_lastCreatedWidget = sb;
}

// Slider
QWidget *CLIBuilder::buildSlider(const Showbox::Models::SliderConfig &config) {
  m_lastCreatedWidget = nullptr;
  QMetaObject::invokeMethod(
      this, "createSliderInMainThread", Qt::BlockingQueuedConnection,
      Q_ARG(QString, config.name), Q_ARG(int, config.value),
      Q_ARG(int, config.min), Q_ARG(int, config.max),
      Q_ARG(int, config.orientation));
  return m_lastCreatedWidget;
}

void CLIBuilder::createSliderInMainThread(const QString &name, int value,
                                          int min, int max, int orientation) {
  QSlider *sl = new QSlider(orientation == 2 ? Qt::Vertical : Qt::Horizontal);
  sl->setObjectName(name);
  sl->setRange(min, max);
  sl->setValue(value);
  m_layout->addWidget(sl);
  m_lastCreatedWidget = sl;
}

// ProgressBar
QWidget *
CLIBuilder::buildProgressBar(const Showbox::Models::ProgressBarConfig &config) {
  m_lastCreatedWidget = nullptr;
  QMetaObject::invokeMethod(
      this, "createProgressBarInMainThread", Qt::BlockingQueuedConnection,
      Q_ARG(QString, config.name), Q_ARG(int, config.value),
      Q_ARG(int, config.minimum), Q_ARG(int, config.maximum),
      Q_ARG(QString, config.format));
  return m_lastCreatedWidget;
}

void CLIBuilder::createProgressBarInMainThread(const QString &name, int value,
                                               int min, int max,
                                               const QString &format) {
  QProgressBar *pb = new QProgressBar();
  pb->setObjectName(name);
  pb->setRange(min, max);
  pb->setValue(value);
  if (!format.isEmpty())
    pb->setFormat(format);
  m_layout->addWidget(pb);
  m_lastCreatedWidget = pb;
}

// Calendar
QWidget *
CLIBuilder::buildCalendar(const Showbox::Models::CalendarConfig &config) {
  m_lastCreatedWidget = nullptr;
  QMetaObject::invokeMethod(this, "createCalendarInMainThread",
                            Qt::BlockingQueuedConnection,
                            Q_ARG(QString, config.name));
  return m_lastCreatedWidget;
}

void CLIBuilder::createCalendarInMainThread(const QString &name) {
  QCalendarWidget *cw = new QCalendarWidget();
  cw->setObjectName(name);
  cw->setGridVisible(true);
  m_layout->addWidget(cw);
  m_lastCreatedWidget = cw;
}

// Table
QWidget *CLIBuilder::buildTable(const Showbox::Models::TableConfig &config) {
  // Converter rows para QVariant
  QVariantList rowsData;
  for (const auto &row : config.rows) {
    rowsData.append(QVariant(row));
  }

  m_lastCreatedWidget = nullptr;
  QMetaObject::invokeMethod(
      this, "createTableInMainThread", Qt::BlockingQueuedConnection,
      Q_ARG(QString, config.name), Q_ARG(QStringList, config.headers),
      Q_ARG(QVariant, QVariant(rowsData)));
  return m_lastCreatedWidget;
}

void CLIBuilder::createTableInMainThread(const QString &name,
                                         const QStringList &headers,
                                         const QVariant &rowsData) {
  QTableWidget *tw = new QTableWidget();
  tw->setObjectName(name);
  tw->setColumnCount(headers.size());
  tw->setHorizontalHeaderLabels(headers);

  QVariantList rows = rowsData.toList();
  tw->setRowCount(rows.size());

  for (int i = 0; i < rows.size(); ++i) {
    QStringList rowContent = rows[i].toStringList();
    for (int j = 0; j < rowContent.size(); ++j) {
      if (j < headers.size()) {
        tw->setItem(i, j, new QTableWidgetItem(rowContent[j]));
      }
    }
  }
  m_layout->addWidget(tw);
  m_lastCreatedWidget = tw;
}

// Chart
QWidget *CLIBuilder::buildChart(const Showbox::Models::ChartConfig &config) {
  QVariantMap dataMap;
  for (auto it = config.data.begin(); it != config.data.end(); ++it) {
    dataMap.insert(it.key(), it.value());
  }

  m_lastCreatedWidget = nullptr;
  QMetaObject::invokeMethod(
      this, "createChartInMainThread", Qt::BlockingQueuedConnection,
      Q_ARG(QString, config.name), Q_ARG(QString, config.title),
      Q_ARG(int, (int)config.type), Q_ARG(QVariant, QVariant(dataMap)));
  return m_lastCreatedWidget;
}

void CLIBuilder::createChartInMainThread(const QString &name,
                                         const QString &title, int type,
                                         const QVariant &dataMap) {
  QChart *chart = new QChart();
  chart->setTitle(title);

  QVariantMap data = dataMap.toMap();

  if (type == 2) { // PieChart
    QPieSeries *series = new QPieSeries();
    for (auto it = data.begin(); it != data.end(); ++it) {
      series->append(it.key(), it.value().toDouble());
    }
    chart->addSeries(series);
  } else { // Bar or Line
    QBarSeries *barSeries = new QBarSeries();
    QBarSet *set = new QBarSet("Data");
    QStringList categories;

    for (auto it = data.begin(); it != data.end(); ++it) {
      set->append(it.value().toDouble());
      categories << it.key();
    }
    barSeries->append(set);
    chart->addSeries(barSeries);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    barSeries->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignLeft);
    barSeries->attachAxis(axisY);
  }

  chart->createDefaultAxes();
  QChartView *chartView = new QChartView(chart);
  chartView->setObjectName(name);
  chartView->setRenderHint(QPainter::Antialiasing);
  chartView->setMinimumHeight(300);

  m_layout->addWidget(chartView);
  m_lastCreatedWidget = chartView;
}

// Containers (GroupBox)
QWidget *
CLIBuilder::buildGroupBox(const Showbox::Models::GroupBoxConfig &config) {
  m_lastCreatedWidget = nullptr;
  QMetaObject::invokeMethod(
      this, "createGroupBoxInMainThread", Qt::BlockingQueuedConnection,
      Q_ARG(QString, config.name), Q_ARG(QString, config.title),
      Q_ARG(int, (int)config.layout.type));
  return m_lastCreatedWidget;
}

void CLIBuilder::createGroupBoxInMainThread(const QString &name,
                                            const QString &title,
                                            int layoutType) {
  QGroupBox *gb = new QGroupBox(title);
  gb->setObjectName(name);
  // Layout handling for container content is done by ParserMain logic
  if (layoutType == 1)
    gb->setLayout(new QHBoxLayout());
  else
    gb->setLayout(new QVBoxLayout());

  m_layout->addWidget(gb);
  m_lastCreatedWidget = gb;
}

// Frame
QWidget *CLIBuilder::buildFrame(const Showbox::Models::FrameConfig &config) {
  m_lastCreatedWidget = nullptr;
  QMetaObject::invokeMethod(
      this, "createFrameInMainThread", Qt::BlockingQueuedConnection,
      Q_ARG(QString, config.name), Q_ARG(int, (int)config.layout.type));
  return m_lastCreatedWidget;
}

void CLIBuilder::createFrameInMainThread(const QString &name, int layoutType) {
  QFrame *fr = new QFrame();
  fr->setObjectName(name);
  fr->setFrameShape(QFrame::StyledPanel);
  if (layoutType == 1)
    fr->setLayout(new QHBoxLayout());
  else
    fr->setLayout(new QVBoxLayout());

  m_layout->addWidget(fr);
  m_lastCreatedWidget = fr;
}

// TabWidget
QWidget *
CLIBuilder::buildTabWidget(const Showbox::Models::TabWidgetConfig &config) {
  m_lastCreatedWidget = nullptr;
  QMetaObject::invokeMethod(this, "createTabWidgetInMainThread",
                            Qt::BlockingQueuedConnection,
                            Q_ARG(QString, config.name));
  return m_lastCreatedWidget;
}

void CLIBuilder::createTabWidgetInMainThread(const QString &name) {
  QTabWidget *tw = new QTabWidget();
  tw->setObjectName(name);
  m_layout->addWidget(tw);
  m_lastCreatedWidget = tw;
}

// Layout (Partial, handled mostly by ParserMain stack logic)
QLayout *CLIBuilder::buildLayout(const Showbox::Models::LayoutConfig &config) {
  Q_UNUSED(config);
  return nullptr;
}

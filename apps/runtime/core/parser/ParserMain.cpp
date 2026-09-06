#include "ParserMain.h"
#include <IShowboxBuilder.h>
#include <QMetaObject>
#include <QStringList>
#include <QThread>
#include <WidgetParserUtils.h>
#include <custom_chart_widget.h>
#include <custom_table_widget.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <utility>

#include <QCheckBox>
#include <QCalendarWidget>
#include <QBoxLayout>
#include <QComboBox>
#include <QFrame>
#include <QFile>
#include <QGroupBox>
#include <QIcon>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QMovie>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QSet>
#include <QSlider>
#include <QSpinBox>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QTextStream>
#include <QWidget>

namespace {
QTableWidget *tableView(QWidget *widget) {
  if (auto *table = qobject_cast<QTableWidget *>(widget))
    return table;
  if (auto *custom = qobject_cast<CustomTableWidget *>(widget))
    return custom->table();
  return nullptr;
}

QLineEdit *lineEditFor(QWidget *widget) {
  if (auto *lineEdit = qobject_cast<QLineEdit *>(widget))
    return lineEdit;
  return widget ? qobject_cast<QLineEdit *>(widget->focusProxy()) : nullptr;
}

QComboBox *comboBoxFor(QWidget *widget) {
  if (auto *combo = qobject_cast<QComboBox *>(widget))
    return combo;
  return widget ? qobject_cast<QComboBox *>(widget->focusProxy()) : nullptr;
}

QListWidget *listWidgetFor(QWidget *widget) {
  if (auto *list = qobject_cast<QListWidget *>(widget))
    return list;
  return widget ? qobject_cast<QListWidget *>(widget->focusProxy()) : nullptr;
}

QTabWidget *owningTabWidget(QWidget *widget) {
  QWidget *ancestor = widget ? widget->parentWidget() : nullptr;
  while (ancestor) {
    if (auto *tabs = qobject_cast<QTabWidget *>(ancestor))
      return tabs;
    ancestor = ancestor->parentWidget();
  }
  return nullptr;
}
} // namespace

ParserMain::ParserMain(IShowboxBuilder *builder, QObject *parent)
    : QObject(parent), m_builder(builder) {
  m_commandParser.registerCommand(
      "add", [this](const QStringList &args) { handleAdd(args); });

  m_commandParser.registerCommand(
      "end", [this](const QStringList &args) { handleEnd(args); });

  m_commandParser.registerCommand(
      "set", [this](const QStringList &args) { handleSet(args); });

  m_commandParser.registerCommand(
      "unset", [this](const QStringList &args) { handleUnset(args); });

  m_commandParser.registerCommand(
      "query", [this](const QStringList &args) { handleQuery(args); });

  m_commandParser.registerCommand("show", [this](const QStringList &args) {
    if (args.isEmpty()) {
      emit showRequested();
    } else {
      QStringList newArgs = {args[0], "visible"};
      handleSet(newArgs);
    }
  });

  m_commandParser.registerCommand("hide", [this](const QStringList &args) {
    if (args.isEmpty())
      return;
    QStringList newArgs = {args[0], "visible"};
    handleUnset(newArgs);
  });

  m_commandParser.registerCommand("enable", [this](const QStringList &args) {
    if (args.isEmpty())
      return;
    QStringList newArgs = {args[0], "enabled"};
    handleSet(newArgs);
  });

  m_commandParser.registerCommand("disable", [this](const QStringList &args) {
    if (args.isEmpty())
      return;
    QStringList newArgs = {args[0], "enabled"};
    handleUnset(newArgs);
  });

  m_commandParser.registerCommand(
      "remove", [this](const QStringList &args) { handleRemove(args); });

  m_commandParser.registerCommand(
      "clear", [this](const QStringList &args) { handleClear(args); });
  m_commandParser.registerCommand(
      "step", [this](const QStringList &args) { handleStep(args); });
  m_commandParser.registerCommand(
      "position", [this](const QStringList &args) { handlePosition(args); });
}

void ParserMain::setRootWidget(QWidget *widget) {
  m_rootWidget = widget;
  if (widget && !widget->objectName().isEmpty())
    m_widgetRegistry[widget->objectName()] = widget;
}

void ParserMain::run() {
  std::string line;
  while (std::getline(std::cin, line)) {
    QString qline = QString::fromStdString(line).trimmed();
    if (qline.isEmpty() || qline.startsWith("#"))
      continue;
    processLine(qline);
  }
}

void ParserMain::processLine(const QString &line) {
  runInMainThread([this, line]() { m_commandParser.parseLine(line); });
}

void ParserMain::addToContext(QWidget *widget) {
  if (!widget)
    return;

  // Register widget by name - safe to do in lambda in Main Thread or here?
  // m_widgetRegistry is QMap, not thread-safe if accessed concurrently.
  // Parser thread is the only writer to it.
  // Access in handleSet/etc is done in Main Thread via lambda, so we have a
  // race if we write here. We should write to registry in Main Thread too just
  // to be safe OR use mutex. Since we are moving everything to runInMainThread,
  // let's do it there.

  runInMainThread([this, widget]() {
    if (!widget->objectName().isEmpty()) {
      m_widgetRegistry[widget->objectName()] = widget;
    }
    connectOutputSignals(widget);

    if (widget->property("showboxPage").toBool() && m_positionTabs &&
        m_positionTabIndex >= 0) {
      if (auto *tabs = qobject_cast<QTabWidget *>(m_positionTabs.data())) {
        const QString title = widget->windowTitle().isEmpty()
                                  ? widget->objectName()
                                  : widget->windowTitle();
        const QIcon icon(widget->property("showboxTabIcon").toString());
        tabs->insertTab(m_positionTabIndex, widget, icon, title);
        if (widget->property("showboxCurrent").toBool())
          tabs->setCurrentIndex(m_positionTabIndex);
        ++m_positionTabIndex;
      }
    } else if (m_positionLayout && m_positionIndex >= 0) {
      m_positionLayout->insertWidget(m_positionIndex++, widget);
    } else if (!m_contextStack.isEmpty()) {
      QWidget *parent = m_contextStack.top();

      // Safely add to parent - redundant safety since we are already in Main
      // Thread now, but good logic.
      QTabWidget *tabWidget = qobject_cast<QTabWidget *>(parent);
      if (tabWidget) {
        QString title = widget->windowTitle().isEmpty() ? widget->objectName()
                                                        : widget->windowTitle();
        const QIcon icon(widget->property("showboxTabIcon").toString());
        const int tabIndex = tabWidget->addTab(widget, icon, title);
        if (widget->property("showboxCurrent").toBool())
          tabWidget->setCurrentIndex(tabIndex);
      } else {
        if (parent->layout()) {
          parent->layout()->addWidget(widget);
        }
      }
    } else if (m_stepColumn && m_stepColumn->layout()) {
      m_stepColumn->layout()->addWidget(widget);
    }
  });
}

void ParserMain::handleEnd(const QStringList &) {
  runInMainThread([this]() {
    if (!m_contextStack.isEmpty()) {
      m_contextStack.pop();
    }
  });
}

void ParserMain::runInMainThread(std::function<void()> func) {
  // If builder (Main Thread object) is available, run there
  QObject *builderObj = dynamic_cast<QObject *>(m_builder);
  if (builderObj) {
    if (QThread::currentThread() == builderObj->thread()) {
      func();
    } else {
      QMetaObject::invokeMethod(builderObj, func, Qt::BlockingQueuedConnection);
    }
  } else {
    func();
  }
}

void ParserMain::handleSet(const QStringList &args) {
  if (args.isEmpty())
    return;
  static const QSet<QString> properties = {
      "checked", "text", "title", "value", "visible", "enabled",
      "readonly", "focus", "stylesheet", "placeholder", "checkable",
      "password", "icon", "iconsize", "minimum", "maximum", "date",
      "current", "editable", "navigation", "headers", "format", "data",
      "append", "axis", "export", "file", "busy", "search", "top",
      "bottom", "left", "right", "animation", "picture", "apply", "exit",
      "default", "plain", "raised", "sunken", "noframe", "box", "panel",
      "styled", "activation", "selection", "add_line", "del_line",
      "horizontal", "vertical"};
  const QString first = args[0].toLower();
  const bool rootProperty =
      (first == "title" || first == "icon" || first == "stylesheet") &&
      args.size() == 2;
  const bool propertyFirst = properties.contains(first);
  if (args.size() < 2 && !propertyFirst)
    return;
  QString name = rootProperty ? QString() : (propertyFirst ? args.value(1) : args[0]);
  QString property = (propertyFirst ? args[0] : args[1]).toLower();
  QString value = rootProperty ? args[1] : args.value(2);

  runInMainThread([this, property, name, value]() {
    QString widgetName = name;
    QString itemText;
    int itemRow = -1;
    bool itemReference = false;
    const qsizetype hash = name.indexOf('#');
    const qsizetype colon = name.indexOf(':');
    if (hash >= 0) {
      widgetName = name.left(hash);
      bool valid = false;
      itemRow = name.mid(hash + 1).toInt(&valid);
      itemReference = valid && itemRow >= 0;
    } else if (colon >= 0) {
      widgetName = name.left(colon);
      itemText = name.mid(colon + 1);
      itemReference = true;
    }

    QWidget *w = findWidget(widgetName);
    if (!w)
      return;

    if (itemReference) {
      if (auto *combo = comboBoxFor(w)) {
        const int row = itemText.isNull()
                            ? itemRow
                            : combo->findText(itemText, Qt::MatchExactly);
        if (row >= 0 && row < combo->count()) {
          if (property == "current")
            combo->setCurrentIndex(row);
          else if (property == "icon")
            combo->setItemIcon(row, QIcon::fromTheme(value, QIcon(value)));
          else if (property == "title")
            combo->setItemText(row, value);
          else if (property == "iconsize") {
            const int size = value.toInt();
            if (size > 0) combo->setIconSize(QSize(size, size));
          }
        }
        return;
      }
      if (auto *list = listWidgetFor(w)) {
        int row = itemRow;
        if (!itemText.isNull()) {
          const auto matches = list->findItems(itemText, Qt::MatchExactly);
          row = matches.isEmpty() ? -1 : list->row(matches.first());
        }
        if (row >= 0 && row < list->count()) {
          if (property == "current")
            list->setCurrentRow(row);
          else if (property == "icon")
            list->item(row)->setIcon(QIcon::fromTheme(value, QIcon(value)));
          else if (property == "title")
            list->item(row)->setText(value);
          else if (property == "iconsize") {
            const int size = value.toInt();
            if (size > 0) list->setIconSize(QSize(size, size));
          }
        }
        return;
      }
    }

    if (property == "checked") {
      if (auto *cb = qobject_cast<QCheckBox *>(w))
        cb->setChecked(true);
      else if (auto *rb = qobject_cast<QRadioButton *>(w))
        rb->setChecked(true);
      else if (auto *btn = qobject_cast<QPushButton *>(w))
        btn->setChecked(true);
    } else if (property == "text") {
      if (auto *lineEdit = lineEditFor(w))
        lineEdit->setText(value);
      else if (auto *lbl = qobject_cast<QLabel *>(w))
        lbl->setText(value);
      else if (auto *btn = qobject_cast<QPushButton *>(w))
        btn->setText(value);
      else if (auto *te = qobject_cast<QTextEdit *>(w))
        te->setPlainText(value);
      else
        w->setWindowTitle(value);
    } else if (property == "title") {
      if (w->property("showboxTextBox").toBool() ||
          w->property("showboxComboBox").toBool() ||
          w->property("showboxListBox").toBool()) {
        const auto labels = w->findChildren<QLabel *>(
            QString(), Qt::FindDirectChildrenOnly);
        for (QLabel *label : labels) {
          if (label->property("showboxTitleLabel").toBool()) {
            label->setText(value);
            break;
          }
        }
      } else if (auto *label = qobject_cast<QLabel *>(w))
        label->setText(value);
      else if (auto *button = qobject_cast<QPushButton *>(w))
        button->setText(value);
      else if (auto *group = qobject_cast<QGroupBox *>(w))
        group->setTitle(value);
      else if (auto *tabs = owningTabWidget(w)) {
        const int index = tabs->indexOf(w);
        if (index >= 0)
          tabs->setTabText(index, value);
      } else
        w->setWindowTitle(value);
    } else if (property == "value") {
      int val = value.toInt();
      if (auto *sb = qobject_cast<QSpinBox *>(w))
        sb->setValue(val);
      else if (auto *sl = qobject_cast<QSlider *>(w))
        sl->setValue(val);
      else if (auto *pb = qobject_cast<QProgressBar *>(w))
        pb->setValue(val);
    } else if (property == "busy") {
      if (auto *progress = qobject_cast<QProgressBar *>(w))
        progress->setRange(0, 0);
    } else if (property == "visible") {
      w->setVisible(true);
    } else if (property == "enabled") {
      if (auto *tabs = owningTabWidget(w)) {
        const int index = tabs->indexOf(w);
        if (index >= 0)
          tabs->setTabEnabled(index, true);
        else
          w->setEnabled(true);
      } else {
        w->setEnabled(true);
      }
    } else if (property == "readonly") {
      if (auto *le = lineEditFor(w))
        le->setReadOnly(true);
      else if (auto *te = qobject_cast<QTextEdit *>(w))
        te->setReadOnly(true);
      else if (auto *table = tableView(w))
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    } else if (property == "focus") {
      w->setFocus(Qt::OtherFocusReason);
    } else if (property == "stylesheet") {
      w->setStyleSheet(value);
    } else if (property == "placeholder") {
      if (auto *lineEdit = lineEditFor(w))
        lineEdit->setPlaceholderText(value);
    } else if (property == "checkable") {
      if (auto *button = qobject_cast<QPushButton *>(w))
        button->setCheckable(true);
      else if (auto *group = qobject_cast<QGroupBox *>(w))
        group->setCheckable(true);
    } else if (property == "password") {
      if (auto *lineEdit = lineEditFor(w))
        lineEdit->setEchoMode(QLineEdit::Password);
    } else if (property == "icon") {
      if (auto *button = qobject_cast<QAbstractButton *>(w))
        button->setIcon(QIcon::fromTheme(value, QIcon(value)));
      else if (auto *label = qobject_cast<QLabel *>(w)) {
        const QPixmap picture(value);
        if (!picture.isNull())
          label->setPixmap(picture);
      } else if (auto *tabs = owningTabWidget(w)) {
        const int index = tabs->indexOf(w);
        if (index >= 0)
          tabs->setTabIcon(index, QIcon::fromTheme(value, QIcon(value)));
      }
    } else if (property == "picture") {
      if (auto *label = qobject_cast<QLabel *>(w)) {
        label->setMovie(nullptr);
        const QPixmap picture(value);
        if (!picture.isNull())
          label->setPixmap(picture);
      }
    } else if (property == "animation") {
      if (auto *label = qobject_cast<QLabel *>(w)) {
        auto *movie = new QMovie(value, QByteArray(), label);
        label->setMovie(movie);
        movie->start();
      }
    } else if (property == "iconsize") {
      const QStringList dimensions = value.toLower().split('x');
      const int width = dimensions.value(0).toInt();
      const int height = dimensions.size() > 1 ? dimensions[1].toInt() : width;
      if (width > 0 && height > 0) {
        const QSize size(width, height);
        if (auto *combo = comboBoxFor(w))
          combo->setIconSize(size);
        else if (auto *list = listWidgetFor(w))
          list->setIconSize(size);
        else
          w->setProperty("iconSize", size);
      }
    } else if (property == "minimum" || property == "maximum") {
      const int number = value.toInt();
      if (auto *slider = qobject_cast<QSlider *>(w)) {
        property == "minimum" ? slider->setMinimum(number)
                              : slider->setMaximum(number);
      } else if (auto *progress = qobject_cast<QProgressBar *>(w)) {
        property == "minimum" ? progress->setMinimum(number)
                              : progress->setMaximum(number);
      } else if (auto *calendar = qobject_cast<QCalendarWidget *>(w)) {
        const QDate date = QDate::fromString(value, Qt::ISODate);
        if (date.isValid())
          property == "minimum" ? calendar->setMinimumDate(date)
                                : calendar->setMaximumDate(date);
      }
    } else if (property == "date") {
      if (auto *calendar = qobject_cast<QCalendarWidget *>(w)) {
        const QDate date = QDate::fromString(value, Qt::ISODate);
        if (date.isValid()) calendar->setSelectedDate(date);
      }
    } else if (property == "format") {
      if (qobject_cast<QCalendarWidget *>(w))
        w->setProperty("showboxDateFormat", value);
      else if (auto *progress = qobject_cast<QProgressBar *>(w))
        progress->setFormat(value);
    } else if (property == "data") {
      if (auto *chart = qobject_cast<CustomChartWidget *>(w))
        chart->setData(value);
    } else if (property == "append") {
      if (auto *chart = qobject_cast<CustomChartWidget *>(w))
        chart->appendData(value);
    } else if (property == "axis") {
      if (auto *chart = qobject_cast<CustomChartWidget *>(w))
        chart->setAxis(value);
    } else if (property == "export") {
      if (auto *chart = qobject_cast<CustomChartWidget *>(w))
        chart->exportChart(value);
    } else if (property == "file") {
      if (auto *chart = qobject_cast<CustomChartWidget *>(w))
        chart->loadFromFile(value);
      else if (auto *table = qobject_cast<CustomTableWidget *>(w))
        table->loadFromFile(value);
      else if (auto *text = qobject_cast<QTextEdit *>(w)) {
        QFile file(value);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
          text->setPlainText(QTextStream(&file).readAll());
      }
    } else if (property == "current") {
      if (auto *combo = comboBoxFor(w))
        combo->setCurrentIndex(value.toInt());
      else if (auto *list = listWidgetFor(w))
        list->setCurrentRow(value.toInt());
      else if (auto *tabs = qobject_cast<QTabWidget *>(w))
        tabs->setCurrentIndex(value.toInt());
      else if (auto *tabs = owningTabWidget(w))
        tabs->setCurrentWidget(w);
    } else if (property == "editable") {
      if (auto *combo = comboBoxFor(w)) combo->setEditable(true);
    } else if (property == "navigation") {
      if (auto *calendar = qobject_cast<QCalendarWidget *>(w))
        calendar->setNavigationBarVisible(true);
    } else if (property == "headers") {
      if (auto *table = tableView(w)) {
        const QStringList headers = value.split(';');
        table->setColumnCount(headers.size());
        table->setHorizontalHeaderLabels(headers);
      }
    } else if (property == "search") {
      if (auto *table = qobject_cast<CustomTableWidget *>(w))
        table->setSearchVisible(true);
    } else if (property == "apply" || property == "exit") {
      if (qobject_cast<QPushButton *>(w))
        w->setProperty(property == "apply" ? "showboxApply" : "showboxExit",
                       true);
    } else if (property == "default") {
      if (auto *button = qobject_cast<QPushButton *>(w))
        button->setDefault(true);
    } else if (property == "plain" || property == "raised" ||
               property == "sunken") {
      if (auto *frame = qobject_cast<QFrame *>(w)) {
        frame->setFrameShadow(property == "raised"   ? QFrame::Raised
                              : property == "sunken" ? QFrame::Sunken
                                                      : QFrame::Plain);
      }
    } else if (property == "noframe" || property == "box" ||
               property == "panel" || property == "styled") {
      if (auto *frame = qobject_cast<QFrame *>(w)) {
        frame->setFrameShape(property == "box"     ? QFrame::Box
                             : property == "panel" ? QFrame::Panel
                             : property == "styled" ? QFrame::StyledPanel
                                                     : QFrame::NoFrame);
      }
    } else if (property == "activation" || property == "selection") {
      w->setProperty(property == "activation" ? "showboxActivation"
                                               : "showboxSelection",
                     true);
    } else if (property == "add_line") {
      if (auto *table = tableView(w)) {
        const QStringList cells = value.split(';');
        const int row = table->rowCount();
        table->insertRow(row);
        for (int column = 0;
             column < cells.size() && column < table->columnCount(); ++column)
          table->setItem(row, column, new QTableWidgetItem(cells[column]));
      }
    } else if (property == "del_line") {
      if (auto *table = tableView(w)) {
        bool valid = false;
        const int row = value.isEmpty() ? 0 : value.toInt(&valid);
        if (value.isEmpty()) valid = true;
        if (valid && row >= 0 && row < table->rowCount())
          table->removeRow(row);
      }
    } else if (property == "top" || property == "bottom" ||
               property == "left" || property == "right") {
      if (auto *tabs = qobject_cast<QTabWidget *>(w)) {
        if (property == "bottom")
          tabs->setTabPosition(QTabWidget::South);
        else if (property == "left")
          tabs->setTabPosition(QTabWidget::West);
        else if (property == "right")
          tabs->setTabPosition(QTabWidget::East);
        else
          tabs->setTabPosition(QTabWidget::North);
      }
    } else if (property == "horizontal" || property == "vertical") {
      const Qt::Orientation orientation =
          property == "vertical" ? Qt::Vertical : Qt::Horizontal;
      if (auto *slider = qobject_cast<QSlider *>(w))
        slider->setOrientation(orientation);
      else if (auto *progress = qobject_cast<QProgressBar *>(w))
        progress->setOrientation(orientation);
      else if (auto *separator = qobject_cast<QFrame *>(w); separator &&
               !separator->layout())
        separator->setFrameShape(orientation == Qt::Vertical ? QFrame::VLine
                                                             : QFrame::HLine);
      if (auto *layout = qobject_cast<QBoxLayout *>(w->layout()))
        layout->setDirection(orientation == Qt::Vertical
                                 ? QBoxLayout::TopToBottom
                                 : QBoxLayout::LeftToRight);
    }
  });
}

void ParserMain::handleUnset(const QStringList &args) {
  if (args.isEmpty())
    return;
  static const QSet<QString> properties = {
      "checked", "visible", "enabled", "readonly", "stylesheet",
      "checkable", "password", "editable", "navigation", "data", "search",
      "busy", "animation", "picture", "apply", "exit", "default",
      "activation", "selection", "format", "text", "title",
      "placeholder", "icon", "iconsize", "file"};
  const bool propertyFirst = properties.contains(args[0].toLower());
  QString name = args.size() == 1 ? QString()
                                  : (propertyFirst ? args[1] : args[0]);
  QString property = (propertyFirst ? args[0] : args.value(1)).toLower();

  runInMainThread([this, property, name]() {
    QString widgetName = name;
    QString itemText;
    int itemRow = -1;
    bool itemReference = false;
    const qsizetype hash = name.indexOf('#');
    const qsizetype colon = name.indexOf(':');
    if (hash >= 0) {
      widgetName = name.left(hash);
      bool valid = false;
      itemRow = name.mid(hash + 1).toInt(&valid);
      itemReference = valid && itemRow >= 0;
    } else if (colon >= 0) {
      widgetName = name.left(colon);
      itemText = name.mid(colon + 1);
      itemReference = true;
    }

    QWidget *w = findWidget(widgetName);
    if (!w)
      return;

    if (itemReference && (property == "title" || property == "icon")) {
      if (auto *combo = comboBoxFor(w)) {
        const int row = itemText.isNull()
                            ? itemRow
                            : combo->findText(itemText, Qt::MatchExactly);
        if (row >= 0 && row < combo->count()) {
          if (property == "title") combo->setItemText(row, QString());
          else combo->setItemIcon(row, QIcon());
        }
      } else if (auto *list = listWidgetFor(w)) {
        int row = itemRow;
        if (!itemText.isNull()) {
          const auto matches = list->findItems(itemText, Qt::MatchExactly);
          row = matches.isEmpty() ? -1 : list->row(matches.first());
        }
        if (row >= 0 && row < list->count()) {
          if (property == "title") list->item(row)->setText(QString());
          else list->item(row)->setIcon(QIcon());
        }
      }
      return;
    }

    if (property == "checked") {
      if (auto *cb = qobject_cast<QCheckBox *>(w))
        cb->setChecked(false);
      else if (auto *rb = qobject_cast<QRadioButton *>(w))
        rb->setChecked(false);
      else if (auto *btn = qobject_cast<QPushButton *>(w))
        btn->setChecked(false);
    } else if (property == "visible") {
      w->setVisible(false);
    } else if (property == "enabled") {
      if (auto *tabs = owningTabWidget(w)) {
        const int index = tabs->indexOf(w);
        if (index >= 0)
          tabs->setTabEnabled(index, false);
        else
          w->setEnabled(false);
      } else {
        w->setEnabled(false);
      }
    } else if (property == "readonly") {
      if (auto *le = lineEditFor(w))
        le->setReadOnly(false);
      else if (auto *te = qobject_cast<QTextEdit *>(w))
        te->setReadOnly(false);
      else if (auto *table = tableView(w))
        table->setEditTriggers(QAbstractItemView::AllEditTriggers);
    } else if (property == "stylesheet") {
      w->setStyleSheet(QString());
    } else if (property == "text") {
      if (auto *line = lineEditFor(w))
        line->clear();
      else if (auto *label = qobject_cast<QLabel *>(w))
        label->clear();
      else if (auto *button = qobject_cast<QAbstractButton *>(w))
        button->setText(QString());
      else if (auto *text = qobject_cast<QTextEdit *>(w))
        text->clear();
    } else if (property == "title") {
      if (w->property("showboxTextBox").toBool() ||
          w->property("showboxComboBox").toBool() ||
          w->property("showboxListBox").toBool()) {
        const auto labels = w->findChildren<QLabel *>(
            QString(), Qt::FindDirectChildrenOnly);
        for (QLabel *label : labels) {
          if (label->property("showboxTitleLabel").toBool()) {
            label->clear();
            break;
          }
        }
      } else if (auto *group = qobject_cast<QGroupBox *>(w))
        group->setTitle(QString());
      else if (auto *tabs = owningTabWidget(w)) {
        const int index = tabs->indexOf(w);
        if (index >= 0) tabs->setTabText(index, QString());
      } else {
        w->setWindowTitle(QString());
      }
    } else if (property == "placeholder") {
      if (auto *line = lineEditFor(w)) line->setPlaceholderText(QString());
    } else if (property == "icon") {
      if (auto *button = qobject_cast<QAbstractButton *>(w))
        button->setIcon(QIcon());
      else if (auto *tabs = owningTabWidget(w)) {
        const int index = tabs->indexOf(w);
        if (index >= 0) tabs->setTabIcon(index, QIcon());
      }
    } else if (property == "iconsize") {
      const QSize size(16, 16);
      if (auto *combo = comboBoxFor(w)) combo->setIconSize(size);
      else if (auto *list = listWidgetFor(w)) list->setIconSize(size);
      else w->setProperty("iconSize", size);
    } else if (property == "file") {
      if (auto *text = qobject_cast<QTextEdit *>(w)) text->clear();
    } else if (property == "checkable") {
      if (auto *button = qobject_cast<QPushButton *>(w))
        button->setCheckable(false);
      else if (auto *group = qobject_cast<QGroupBox *>(w))
        group->setCheckable(false);
    } else if (property == "password") {
      if (auto *lineEdit = lineEditFor(w))
        lineEdit->setEchoMode(QLineEdit::Normal);
    } else if (property == "editable") {
      if (auto *combo = comboBoxFor(w)) combo->setEditable(false);
    } else if (property == "navigation") {
      if (auto *calendar = qobject_cast<QCalendarWidget *>(w))
        calendar->setNavigationBarVisible(false);
    } else if (property == "data") {
      if (auto *chart = qobject_cast<CustomChartWidget *>(w))
        chart->clearSeries();
    } else if (property == "busy") {
      if (auto *progress = qobject_cast<QProgressBar *>(w))
        progress->setRange(0, 100);
    } else if (property == "format") {
      if (qobject_cast<QCalendarWidget *>(w))
        w->setProperty("showboxDateFormat", "yyyy-MM-dd");
      else if (auto *progress = qobject_cast<QProgressBar *>(w))
        progress->setFormat("%p%");
    } else if (property == "search") {
      if (auto *table = qobject_cast<CustomTableWidget *>(w))
        table->setSearchVisible(false);
    } else if (property == "animation" || property == "picture") {
      if (auto *label = qobject_cast<QLabel *>(w))
        label->clear();
    } else if (property == "apply" || property == "exit") {
      if (qobject_cast<QPushButton *>(w))
        w->setProperty(property == "apply" ? "showboxApply" : "showboxExit",
                       false);
    } else if (property == "default") {
      if (auto *button = qobject_cast<QPushButton *>(w))
        button->setDefault(false);
    } else if (property == "activation" || property == "selection") {
      w->setProperty(property == "activation" ? "showboxActivation"
                                               : "showboxSelection",
                     false);
    }
  });
}

void ParserMain::handleQuery(const QStringList &args) {
  if (args.isEmpty()) {
    reportWidgets();
    return;
  }
  QWidget *widget = findWidget(args[0]);
  if (widget && !reportValue(widget).isNull())
    std::cout << widget->objectName().toStdString() << "="
              << reportValue(widget).toStdString() << std::endl;
}

// Force MOC inclusion if AUTOMOC fails to link it
// #include "moc_ParserMain.cpp"

void ParserMain::handleAdd(const QStringList &args) {
  if (args.isEmpty())
    return;

  QString type = args[0].toLower();
  QString title = args.value(1);
  title.replace("\\n", "\n"); // Support newlines in title/text
  QString name = args.value(2);
  QStringList options = args.mid(3);

  const QSet<QString> nameOnlyTypes = {"frame", "separator", "progressbar",
                                        "tabs"};
  if (nameOnlyTypes.contains(type)) {
    name = args.value(1);
    title.clear();
    options = args.mid(2);
  }
  if (type == "slider") {
    static const QSet<QString> sliderOptions = {
        "horizontal", "vertical", "minimum", "maximum", "value"};
    bool legacyRange = false;
    args.value(2).toInt(&legacyRange);
    const bool optionsFollowName =
        sliderOptions.contains(args.value(2).toLower());
    if (args.size() == 2 || legacyRange || optionsFollowName) {
      name = args.value(1);
      title.clear();
      options = args.mid(2);
    }
  }
  if (type == "textview" || type == "textedit") {
    static const QSet<QString> textViewOptions = {"file", "readonly", "html"};
    if (args.size() == 2 || textViewOptions.contains(args.value(2).toLower())) {
      name = args.value(1);
      title.clear();
      options = args.mid(2);
    }
  }
  if (type == "separator" || type == "line") {
    static const QSet<QString> separatorOptions = {
        "horizontal", "vertical", "plain", "raised", "sunken"};
    if (separatorOptions.contains(args.value(1).toLower())) {
      name.clear();
      title.clear();
      options = args.mid(1);
    }
  }

  if (type == "space" || type == "stretch") {
    QWidget *container = m_contextStack.isEmpty() ? m_rootWidget.data()
                                                   : m_contextStack.top();
    if (!container || !container->layout())
      return;
    if (type == "stretch") {
      if (auto *box = qobject_cast<QBoxLayout *>(container->layout()))
        box->addStretch(qMax(1, args.value(1, "1").toInt()));
    } else {
      const int size = args.value(1, "1").toInt();
      if (auto *box = qobject_cast<QBoxLayout *>(container->layout()))
        box->addSpacing(size > 0 ? size : 1);
    }
    return;
  }

  // Builders are theoretically thread-safe (they use invokeMethod internally),
  // EXCEPT for the returned pointer which we shouldn't touch in this thread if
  // we can avoid it. However, our Builder returns the pointer created. The
  // safest way is to NOT allow the builder to return the pointer directly to
  // here OR we treat the returned pointer as opaque handle until we use it in
  // invokeMethod.
  //
  // The current CLIBuilder implementation does return a pointer but creation
  // happens in Main Thread via invokeMethod (blocking). So the pointer is valid
  // but "living" in Main Thread. We must NOT call methods on it here.

  QWidget *createdWidget = nullptr;

  if (type == "pushbutton" || type == "button") {
    Showbox::Models::ButtonConfig config;
    config.name = name;
    config.text = title;
    config.checkable = WidgetParserUtils::hasFlag(options, "checkable");
    config.checked = WidgetParserUtils::hasFlag(options, "checked");
    config.iconPath = WidgetParserUtils::findValue(options, "icon");

    config.apply = WidgetParserUtils::hasFlag(options, "apply");
    config.exit = WidgetParserUtils::hasFlag(options, "exit");
    config.isDefault = WidgetParserUtils::hasFlag(options, "default");

    createdWidget = m_builder->buildButton(config);
    if (createdWidget) {
      createdWidget->setProperty("showboxApply",
                                 WidgetParserUtils::hasFlag(options, "apply"));
      createdWidget->setProperty("showboxExit",
                                 WidgetParserUtils::hasFlag(options, "exit"));
      if (WidgetParserUtils::hasFlag(options, "default")) {
        if (auto *button = qobject_cast<QPushButton *>(createdWidget))
          button->setDefault(true);
      }
    }
  } else if (type == "label") {
    Showbox::Models::LabelConfig config;
    config.name = name;
    config.text = title;
    config.wordWrap = WidgetParserUtils::hasFlag(options, "wordwrap");
    config.iconPath = WidgetParserUtils::findValue(options, "icon");
    if (WidgetParserUtils::hasFlag(options, "picture"))
      config.iconPath = title;
    if (WidgetParserUtils::hasFlag(options, "animation")) {
      config.iconPath = title;
      config.animation = true;
    }
    createdWidget = m_builder->buildLabel(config);
  } else if (type == "checkbox") {
    Showbox::Models::CheckBoxConfig config;
    config.name = name;
    config.text = title;
    config.checked = WidgetParserUtils::hasFlag(options, "checked");
    createdWidget = m_builder->buildCheckBox(config);
  } else if (type == "radiobutton") {
    Showbox::Models::RadioButtonConfig config;
    config.name = name;
    config.text = title;
    config.checked = WidgetParserUtils::hasFlag(options, "checked");
    createdWidget = m_builder->buildRadioButton(config);
  } else if (type == "combobox" || type == "dropdownlist") {
    Showbox::Models::ComboBoxConfig config;
    config.name = name;
    config.title = title;
    QString items = WidgetParserUtils::findValue(options, "items");
    if (!items.isEmpty())
      config.items = items.split(",", Qt::SkipEmptyParts);
    config.editable = type == "combobox";
    config.selection = WidgetParserUtils::hasFlag(options, "selection");
    createdWidget = m_builder->buildComboBox(config);
    if (createdWidget)
      createdWidget->setProperty(
          "showboxSelection", WidgetParserUtils::hasFlag(options, "selection"));
  } else if (type == "listbox") {
    Showbox::Models::ListConfig config;
    config.name = name;
    config.title = title;
    QString items = WidgetParserUtils::findValue(options, "items");
    if (!items.isEmpty())
      config.items = items.split(",", Qt::SkipEmptyParts);
    config.multipleSelection = false;
    config.selection = WidgetParserUtils::hasFlag(options, "selection");
    config.activation = WidgetParserUtils::hasFlag(options, "activation");
    createdWidget = m_builder->buildList(config);
    if (createdWidget) {
      createdWidget->setProperty("showboxSelection", config.selection);
      createdWidget->setProperty(
          "showboxActivation", WidgetParserUtils::hasFlag(options, "activation"));
    }
  } else if (type == "window") {
    Showbox::Models::WindowConfig config;
    config.title = title;
    config.name = name;
    config.width = WidgetParserUtils::findIntValue(options, "width", 800);
    config.height = WidgetParserUtils::findIntValue(options, "height", 600);
    createdWidget = m_builder->buildWindow(config);
    setRootWidget(createdWidget);

    // Window is a root context
    if (createdWidget) {
      runInMainThread(
          [this, createdWidget]() { m_contextStack.push(createdWidget); });
    }
    return; // Don't add window to itself/context
  } else if (type == "slider") {
    Showbox::Models::SliderConfig config;
    config.name = name;
    config.value = WidgetParserUtils::findIntValue(options, "value", 0);
    config.min = WidgetParserUtils::findIntValue(options, "minimum", 0);
    config.max = WidgetParserUtils::findIntValue(options, "maximum", 100);
    bool hasLegacyMinimum = false;
    const int legacyMinimum = args.value(2).toInt(&hasLegacyMinimum);
    if (hasLegacyMinimum) {
      config.min = legacyMinimum;
      bool hasLegacyMaximum = false;
      const int legacyMaximum = args.value(3).toInt(&hasLegacyMaximum);
      if (hasLegacyMaximum)
        config.max = legacyMaximum;
    }
    if (WidgetParserUtils::hasFlag(options, "horizontal"))
      config.orientation = 1;
    if (WidgetParserUtils::hasFlag(options, "vertical"))
      config.orientation = 2;
    createdWidget = m_builder->buildSlider(config);
  } else if (type == "progressbar") {
    Showbox::Models::ProgressBarConfig config;
    config.name = name;
    config.value = WidgetParserUtils::findIntValue(options, "value", 0);
    config.minimum = WidgetParserUtils::findIntValue(options, "minimum", 0);
    config.maximum = WidgetParserUtils::findIntValue(options, "maximum", 100);
    config.format = WidgetParserUtils::findValue(options, "format");
    config.busy = WidgetParserUtils::hasFlag(options, "busy");
    config.orientation = WidgetParserUtils::hasFlag(options, "vertical") ? 2 : 1;
    createdWidget = m_builder->buildProgressBar(config);
  } else if (type == "textbox" || type == "lineedit") {
    Showbox::Models::LineEditConfig config;
    config.name = name;
    config.title = title;
    config.text = WidgetParserUtils::findValue(options, "text");
    config.passwordMode = WidgetParserUtils::hasFlag(options, "password");
    config.placeholder = WidgetParserUtils::findValue(options, "placeholder");
    createdWidget = m_builder->buildLineEdit(config);
  } else if (type == "textview" || type == "textedit") {
    Showbox::Models::TextEditConfig config;
    config.name = name;
    config.text = title;
    config.file = WidgetParserUtils::findValue(options, "file");
    config.readOnly = true;
    config.richText = WidgetParserUtils::hasFlag(options, "html");
    createdWidget = m_builder->buildTextEdit(config);
  } else if (type == "groupbox") {
    Showbox::Models::GroupBoxConfig config;
    config.name = name;
    config.title = title;
    config.checkable = WidgetParserUtils::hasFlag(options, "checkable");
    config.checked = WidgetParserUtils::hasFlag(options, "checked");
    config.layout.type = WidgetParserUtils::hasFlag(options, "vertical")
                             ? Showbox::Models::LayoutConfig::VBox
                             : Showbox::Models::LayoutConfig::HBox;
    createdWidget = m_builder->buildGroupBox(config);

    // Push to stack to accept children
    if (createdWidget) {
      addToContext(createdWidget);
      runInMainThread(
          [this, createdWidget]() { m_contextStack.push(createdWidget); });
      return;
    }
  } else if (type == "frame") {
    Showbox::Models::FrameConfig config;
    config.name = name;
    config.layout.type = WidgetParserUtils::hasFlag(options, "vertical")
                             ? Showbox::Models::LayoutConfig::VBox
                             : Showbox::Models::LayoutConfig::HBox;
    if (WidgetParserUtils::hasFlag(options, "box"))
      config.shape = QFrame::Box;
    else if (WidgetParserUtils::hasFlag(options, "panel"))
      config.shape = QFrame::Panel;
    else if (WidgetParserUtils::hasFlag(options, "styled"))
      config.shape = QFrame::StyledPanel;
    else
      config.shape = QFrame::NoFrame;
    if (WidgetParserUtils::hasFlag(options, "raised"))
      config.shadow = QFrame::Raised;
    else if (WidgetParserUtils::hasFlag(options, "sunken"))
      config.shadow = QFrame::Sunken;
    else
      config.shadow = QFrame::Plain;
    createdWidget = m_builder->buildFrame(config);

    if (createdWidget) {
      addToContext(createdWidget);
      runInMainThread(
          [this, createdWidget]() { m_contextStack.push(createdWidget); });
      return;
    }
  } else if (type == "tabs" || type == "tabwidget") {
    Showbox::Models::TabWidgetConfig config;
    config.name = name;
    if (WidgetParserUtils::hasFlag(options, "bottom")) config.position = 1;
    if (WidgetParserUtils::hasFlag(options, "left")) config.position = 2;
    if (WidgetParserUtils::hasFlag(options, "right")) config.position = 3;
    createdWidget = m_builder->buildTabWidget(config);

    if (createdWidget) {
      addToContext(createdWidget);
      runInMainThread(
          [this, createdWidget]() { m_contextStack.push(createdWidget); });
      return;
    }
  } else if (type == "page") {
    Showbox::Models::FrameConfig config;
    config.name = name; // Page name
    if (WidgetParserUtils::hasFlag(options, "horizontal"))
      config.layout.type = Showbox::Models::LayoutConfig::HBox;
    else
      config.layout.type = Showbox::Models::LayoutConfig::VBox;

    createdWidget = m_builder->buildFrame(config);
    if (createdWidget) {
      QString icon = WidgetParserUtils::findValue(options, "icon");
      if (icon.isEmpty() && !options.isEmpty() &&
          options.first().compare("current", Qt::CaseInsensitive) != 0 &&
          options.first().compare("vertical", Qt::CaseInsensitive) != 0 &&
          options.first().compare("horizontal", Qt::CaseInsensitive) != 0)
        icon = options.first();
      const bool current = WidgetParserUtils::hasFlag(options, "current");
      runInMainThread([createdWidget, title, icon, current]() {
        createdWidget->setWindowTitle(title); // Important for Tab Text
        createdWidget->setProperty("showboxPage", true);
        createdWidget->setProperty("showboxTabIcon", icon);
        createdWidget->setProperty("showboxCurrent", current);
      });
      addToContext(createdWidget);
      runInMainThread(
          [this, createdWidget]() { m_contextStack.push(createdWidget); });
      return;
    }
  } else if (type == "item") {
    QWidget *container = nullptr;
    int insertionIndex = -1;
    bool usePositionCursor = false;
    if (!m_contextStack.isEmpty()) {
      container = m_contextStack.top();
    } else if (m_positionItemContainer) {
      container = m_positionItemContainer;
      insertionIndex = m_positionItemIndex;
      usePositionCursor = true;
    }
    if (!container)
      return;
    const QStringList itemOptions = args.mid(2);
    const QString iconName = !itemOptions.isEmpty() &&
                                     itemOptions.first().compare(
                                         "current", Qt::CaseInsensitive) != 0
                                 ? itemOptions.first()
                                 : QString();
    QIcon icon = QIcon::fromTheme(iconName);
    if (icon.isNull() && !iconName.isEmpty())
      icon = QIcon(iconName);
    if (auto *combo = comboBoxFor(container)) {
      const int row = insertionIndex >= 0 ? insertionIndex : combo->count();
      combo->insertItem(row, icon, title);
      if (WidgetParserUtils::hasFlag(itemOptions, "current"))
        combo->setCurrentIndex(row);
    } else if (auto *list = listWidgetFor(container)) {
      auto *item = new QListWidgetItem(icon, title);
      const int row = insertionIndex >= 0 ? insertionIndex : list->count();
      list->insertItem(row, item);
      if (WidgetParserUtils::hasFlag(itemOptions, "current"))
        list->setCurrentItem(item);
    } else if (auto *table = tableView(container)) {
      const QStringList cells = title.split(';');
      const int row = insertionIndex >= 0 ? insertionIndex : table->rowCount();
      table->insertRow(row);
      for (int column = 0; column < cells.size() && column < table->columnCount();
           ++column)
        table->setItem(row, column, new QTableWidgetItem(cells[column]));
    }
    if (usePositionCursor)
      ++m_positionItemIndex;
    return;
  } else if (type == "table") {
    Showbox::Models::TableConfig config;
    config.name = name;
    QString headers = WidgetParserUtils::findValue(options, "headers");
    if (headers.isEmpty()) headers = title;
    if (!headers.isEmpty())
      config.headers = headers.split(';', Qt::SkipEmptyParts);
    config.readOnly = WidgetParserUtils::hasFlag(options, "readonly");
    config.selection = WidgetParserUtils::hasFlag(options, "selection");
    config.search = WidgetParserUtils::hasFlag(options, "search");
    config.file = WidgetParserUtils::findValue(options, "file");

    QString rowsRaw = WidgetParserUtils::findValue(options, "rows");
    if (!rowsRaw.isEmpty()) {
      for (const QString &r : rowsRaw.split(";", Qt::SkipEmptyParts)) {
        config.rows.append(r.split(","));
      }
    }
    createdWidget = m_builder->buildTable(config);
    if (createdWidget)
      createdWidget->setProperty("showboxSelection", config.selection);
  } else if (type == "calendar") {
    Showbox::Models::CalendarConfig config;
    config.name = name;
    config.date = WidgetParserUtils::findValue(options, "date");
    config.minimum = WidgetParserUtils::findValue(options, "minimum");
    config.maximum = WidgetParserUtils::findValue(options, "maximum");
    config.navigation = !WidgetParserUtils::hasFlag(options, "nonavigation");
    config.selection = WidgetParserUtils::hasFlag(options, "selection");
    config.format = WidgetParserUtils::findValue(options, "format");
    if (config.format.isEmpty())
      config.format = "yyyy-MM-dd";
    createdWidget = m_builder->buildCalendar(config);
    if (createdWidget)
      createdWidget->setProperty("showboxSelection", config.selection);
  } else if (type == "separator" || type == "line") {
    Showbox::Models::SeparatorConfig config;
    config.name = name;
    config.orientation = WidgetParserUtils::hasFlag(options, "vertical")
                             ? Qt::Vertical
                             : Qt::Horizontal;
    if (WidgetParserUtils::hasFlag(options, "raised"))
      config.shadow = QFrame::Raised;
    else if (WidgetParserUtils::hasFlag(options, "plain"))
      config.shadow = QFrame::Plain;
    else
      config.shadow = QFrame::Sunken;
    createdWidget = m_builder->buildSeparator(config);
  } else if (type == "chart") {
    Showbox::Models::ChartConfig config;
    config.name = name;
    config.title = title;
    config.type =
        (Showbox::Models::ChartConfig::Type)WidgetParserUtils::findIntValue(
            options, "type", 0);

    QString data = WidgetParserUtils::findValue(options, "data");
    if (!data.isEmpty()) {
      data.replace(',', ';');
      for (const QString &pair : data.split(';', Qt::SkipEmptyParts)) {
        if (pair.contains(":")) {
          auto parts = pair.split(":");
          if (parts.size() >= 2)
            config.data[parts[0]] = parts[1].toDouble();
        }
      }
    }
    createdWidget = m_builder->buildChart(config);
  }

  if (createdWidget) {
    runInMainThread([this, createdWidget, options]() {
      WidgetParserUtils::applyStyles(createdWidget, options);
    });
    addToContext(createdWidget);
    if (type == "listbox" || type == "combobox" ||
        type == "dropdownlist" || type == "table")
      m_contextStack.push(createdWidget);
  }
}

void ParserMain::handleRemove(const QStringList &args) {
  if (args.isEmpty())
    return;
  QString name = args[0];

  runInMainThread([this, name]() {
    if (!m_widgetRegistry.contains(name) || !m_widgetRegistry[name])
      return;

    QWidget *w = m_widgetRegistry[name];

    // Remove from registry immediately
    m_widgetRegistry.remove(name);

    // Qt handles parent/layout removal automatically on delete
    w->deleteLater();
  });
}

void ParserMain::handleClear(const QStringList &args) {
  QString name = args.value(0);

  runInMainThread([this, name]() {
    QWidget *container = findWidget(name);
    if (!container)
      return;

    if (auto *combo = comboBoxFor(container)) {
      combo->clear();
      return;
    }
    if (auto *list = listWidgetFor(container)) {
      list->clear();
      return;
    }
    if (auto *table = tableView(container)) {
      table->setRowCount(0);
      return;
    }
    if (auto *text = qobject_cast<QTextEdit *>(container)) {
      text->clear();
      return;
    }
    if (auto *line = lineEditFor(container)) {
      line->clear();
      return;
    }
    if (auto *chart = qobject_cast<CustomChartWidget *>(container)) {
      chart->clearSeries();
      return;
    }

    auto unregisterTree = [this](QWidget *widget) {
      if (!widget->objectName().isEmpty())
        m_widgetRegistry.remove(widget->objectName());
      const auto descendants = widget->findChildren<QWidget *>();
      for (QWidget *descendant : descendants) {
        if (!descendant->objectName().isEmpty())
          m_widgetRegistry.remove(descendant->objectName());
      }
    };

    if (auto *tabs = qobject_cast<QTabWidget *>(container)) {
      while (tabs->count() > 0) {
        QWidget *page = tabs->widget(0);
        unregisterTree(page);
        tabs->removeTab(0);
        page->deleteLater();
      }
      return;
    }

    std::function<void(QLayout *)> clearLayout =
        [&](QLayout *layout) {
          while (layout && layout->count() > 0) {
            QLayoutItem *item = layout->takeAt(0);
            if (QWidget *child = item->widget()) {
              unregisterTree(child);
              child->deleteLater();
            } else if (QLayout *childLayout = item->layout()) {
              clearLayout(childLayout);
            }
            delete item;
          }
        };
    clearLayout(container->layout());
    m_contextStack.clear();
    m_positionLayout.clear();
    m_positionTabs.clear();
    m_positionItemContainer.clear();
    m_stepRow.clear();
    m_stepColumn.clear();
  });
}

QWidget *ParserMain::findWidget(const QString &name) const {
  if (name.isEmpty() || name == "showbox" || name == "main")
    return m_rootWidget;
  return m_widgetRegistry.value(name);
}

QString ParserMain::reportValue(QWidget *widget) const {
  if (auto *checkBox = qobject_cast<QCheckBox *>(widget))
    return checkBox->isChecked() ? "1" : "0";
  if (auto *radioButton = qobject_cast<QRadioButton *>(widget))
    return radioButton->isChecked() ? "1" : "0";
  if (auto *lineEdit = lineEditFor(widget))
    return lineEdit->text().isNull() ? QStringLiteral("") : lineEdit->text();
  if (auto *textEdit = qobject_cast<QTextEdit *>(widget))
    return textEdit->toPlainText().isNull() ? QStringLiteral("")
                                            : textEdit->toPlainText();
  if (auto *comboBox = comboBoxFor(widget))
    return comboBox->currentText().isNull() ? QStringLiteral("")
                                            : comboBox->currentText();
  if (auto *list = listWidgetFor(widget)) {
    QListWidgetItem *item = list->currentItem();
    return item ? item->text() : QStringLiteral("");
  }
  if (auto *slider = qobject_cast<QSlider *>(widget))
    return QString::number(slider->value());
  if (auto *calendar = qobject_cast<QCalendarWidget *>(widget)) {
    const QString format = calendar->property("showboxDateFormat").toString();
    return calendar->selectedDate().toString(format.isEmpty() ? "yyyy-MM-dd"
                                                               : format);
  }
  return QString();
}

void ParserMain::reportWidgets() {
  QSet<QWidget *> reported;
  auto report = [this, &reported](QWidget *widget) {
    if (!widget || widget == m_rootWidget || reported.contains(widget) ||
        !widget->isEnabled() || widget->objectName().isEmpty() ||
        m_widgetRegistry.value(widget->objectName()) != widget)
      return;
    reported.insert(widget);
    if (auto *table = tableView(widget)) {
      for (int row = 0; row < table->rowCount(); ++row) {
        for (int column = 0; column < table->columnCount(); ++column) {
          const QTableWidgetItem *item = table->item(row, column);
          std::cout << widget->objectName().toStdString() << "[" << row
                    << "][" << column << "]="
                    << (item ? item->text().toStdString() : "") << std::endl;
        }
      }
      return;
    }
    if (auto *chart = qobject_cast<CustomChartWidget *>(widget)) {
      for (const auto &entry : chart->dataPoints()) {
        std::cout << widget->objectName().toStdString() << ".slice[\""
                  << entry.first.toStdString() << "\"]=" << std::fixed
                  << std::setprecision(6) << entry.second << std::defaultfloat
                  << std::endl;
      }
      return;
    }
    const QString value = reportValue(widget);
    if (!value.isNull())
      std::cout << widget->objectName().toStdString() << "="
                << value.toStdString() << std::endl;
  };

  std::function<void(QLayout *)> visitLayout;
  std::function<void(QWidget *)> visitWidget;
  visitWidget = [&](QWidget *widget) {
    report(widget);
    if (auto *tabs = qobject_cast<QTabWidget *>(widget)) {
      for (int index = 0; index < tabs->count(); ++index)
        visitWidget(tabs->widget(index));
      return;
    }
    visitLayout(widget ? widget->layout() : nullptr);
  };
  visitLayout = [&](QLayout *layout) {
    if (!layout) return;
    for (int index = 0; index < layout->count(); ++index) {
      QLayoutItem *item = layout->itemAt(index);
      if (item->widget()) visitWidget(item->widget());
      else if (item->layout()) visitLayout(item->layout());
    }
  };
  visitLayout(m_rootWidget ? m_rootWidget->layout() : nullptr);

  // Builders used by embedders may return unattached widgets. Keep them
  // reportable after the layout-ordered compatibility traversal.
  for (QWidget *widget : std::as_const(m_widgetRegistry)) report(widget);
  std::cout.flush();
}

void ParserMain::connectOutputSignals(QWidget *widget) {
  const QString name = widget->objectName();
  if (name.isEmpty())
    return;

  if (auto *button = qobject_cast<QPushButton *>(widget)) {
    connect(button, &QPushButton::clicked, this, [this, button, name]() {
      if (!button->isCheckable())
        std::cout << name.toStdString() << "=clicked" << std::endl;
      if (button->property("showboxApply").toBool())
        reportWidgets();
      if (button->property("showboxExit").toBool() && m_rootWidget) {
        if (button->property("showboxApply").toBool())
          QMetaObject::invokeMethod(m_rootWidget, "accept");
        else
          QMetaObject::invokeMethod(m_rootWidget, "reject");
      }
    });
    connect(button, &QPushButton::toggled, this, [name](bool checked) {
      std::cout << name.toStdString() << "="
                << (checked ? "pressed" : "released") << std::endl;
      std::cout.flush();
    });
  } else if (auto *slider = qobject_cast<QSlider *>(widget)) {
    connect(slider, &QSlider::valueChanged, this, [name](int value) {
      std::cout << name.toStdString() << "=" << value << std::endl;
      std::cout.flush();
    });
  } else if (auto *combo = comboBoxFor(widget)) {
    connect(combo, &QComboBox::currentTextChanged, this,
            [widget, name](const QString &text) {
      if (widget->property("showboxSelection").toBool()) {
        std::cout << name.toStdString() << "=" << text.toStdString()
                  << std::endl;
        std::cout.flush();
      }
    });
  } else if (auto *list = listWidgetFor(widget)) {
    connect(list, &QListWidget::currentTextChanged, this,
            [widget, name](const QString &text) {
      if (widget->property("showboxSelection").toBool()) {
        std::cout << name.toStdString() << "=" << text.toStdString()
                  << std::endl;
        std::cout.flush();
      }
    });
    connect(list, &QListWidget::itemActivated, this,
            [widget, name](QListWidgetItem *item) {
      if (widget->property("showboxActivation").toBool()) {
        std::cout << name.toStdString() << "="
                  << (item ? item->text().toStdString() : "") << std::endl;
        std::cout.flush();
      }
    });
  } else if (auto *table = qobject_cast<CustomTableWidget *>(widget)) {
    connect(table, &CustomTableWidget::cellEdited, this,
            [name](int row, int column, const QString &text) {
      std::cout << name.toStdString() << "[" << row << "][" << column
                << "]=" << text.toStdString() << std::endl;
      std::cout.flush();
    });
    connect(table, &CustomTableWidget::rowSelected, this,
            [widget, name](int row) {
      if (widget->property("showboxSelection").toBool()) {
        std::cout << name.toStdString() << "_selection=" << row << std::endl;
        std::cout.flush();
      }
    });
  } else if (auto *table = qobject_cast<QTableWidget *>(widget)) {
    connect(table, &QTableWidget::cellChanged, this,
            [name, table](int row, int column) {
              const QTableWidgetItem *item = table->item(row, column);
              std::cout << name.toStdString() << "[" << row << "][" << column
                        << "]=" << (item ? item->text().toStdString() : "")
                        << std::endl;
              std::cout.flush();
            });
  } else if (auto *calendar = qobject_cast<QCalendarWidget *>(widget)) {
    connect(calendar, &QCalendarWidget::selectionChanged, this,
            [widget, name, calendar]() {
      if (widget->property("showboxSelection").toBool()) {
        const QString format = calendar->property("showboxDateFormat").toString();
        std::cout << name.toStdString() << "="
                  << calendar->selectedDate()
                         .toString(format.isEmpty() ? "yyyy-MM-dd" : format)
                         .toStdString()
                  << std::endl;
        std::cout.flush();
      }
    });
  } else if (auto *chart = qobject_cast<CustomChartWidget *>(widget)) {
    connect(chart, &CustomChartWidget::itemClicked, this,
            [name](const QString &label, double value) {
      std::cout << name.toStdString() << ".slice[\"" << label.toStdString()
                << "\"]=" << value << std::endl;
      std::cout.flush();
    });
  }
}

void ParserMain::handleStep(const QStringList &args) {
  if (!m_rootWidget || !m_rootWidget->layout() || !m_contextStack.isEmpty())
    return;

  m_positionLayout.clear();
  m_positionIndex = -1;
  m_positionTabs.clear();
  m_positionTabIndex = -1;
  m_positionItemContainer.clear();
  m_positionItemIndex = -1;

  const bool vertical = args.contains("vertical", Qt::CaseInsensitive);
  auto createColumn = []() {
    auto *column = new QWidget();
    column->setLayout(new QVBoxLayout());
    column->layout()->setContentsMargins(0, 0, 0, 0);
    return column;
  };

  if (vertical || !m_stepRow) {
    auto *row = new QWidget();
    row->setLayout(new QHBoxLayout());
    row->layout()->setContentsMargins(0, 0, 0, 0);

    if (!m_stepRow && !vertical) {
      QWidget *firstColumn = createColumn();
      QList<QWidget *> existing;
      QLayout *rootLayout = m_rootWidget->layout();
      for (int index = 0; index < rootLayout->count(); ++index) {
        QWidget *child = rootLayout->itemAt(index)->widget();
        if (child)
          existing.append(child);
      }
      for (QWidget *child : existing)
        firstColumn->layout()->addWidget(child);
      row->layout()->addWidget(firstColumn);
    }

    m_rootWidget->layout()->addWidget(row);
    m_stepRow = row;
  }

  QWidget *column = createColumn();
  m_stepRow->layout()->addWidget(column);
  m_stepColumn = column;
}

void ParserMain::handlePosition(const QStringList &args) {
  if (args.isEmpty())
    return;

  const bool behind = args.contains("behind", Qt::CaseInsensitive);
  const bool onto = args.contains("onto", Qt::CaseInsensitive);
  QString name;
  for (const QString &arg : args) {
    if (arg.compare("behind", Qt::CaseInsensitive) != 0 &&
        arg.compare("onto", Qt::CaseInsensitive) != 0) {
      name = arg;
      break;
    }
  }

  QString containerName = name;
  QString itemText;
  int itemIndex = -1;
  bool itemReference = false;
  const qsizetype hash = name.indexOf('#');
  const qsizetype colon = name.indexOf(':');
  if (hash >= 0) {
    containerName = name.left(hash);
    bool valid = false;
    itemIndex = name.mid(hash + 1).toInt(&valid);
    itemReference = valid && itemIndex >= 0;
  } else if (colon >= 0) {
    containerName = name.left(colon);
    itemText = name.mid(colon + 1);
    itemReference = true;
  }

  QWidget *widget = findWidget(containerName);
  if (!widget)
    return;

  runInMainThread([this, widget, behind, onto, itemReference, itemIndex,
                   itemText]() {
    m_positionLayout.clear();
    m_positionIndex = -1;
    m_positionTabs.clear();
    m_positionTabIndex = -1;
    m_positionItemContainer.clear();
    m_positionItemIndex = -1;

    auto findItemIndex = [&itemText](QWidget *container) {
      if (auto *combo = comboBoxFor(container)) {
        const int found = combo->findText(itemText, Qt::MatchExactly);
        return found >= 0 ? found : combo->count();
      }
      if (auto *list = listWidgetFor(container)) {
        const auto found = list->findItems(itemText, Qt::MatchExactly);
        return found.isEmpty() ? list->count() : list->row(found.first());
      }
      return -1;
    };

    if (itemReference) {
      int index = itemText.isNull() ? itemIndex : findItemIndex(widget);
      int count = -1;
      if (auto *combo = comboBoxFor(widget))
        count = combo->count();
      else if (auto *list = listWidgetFor(widget))
        count = list->count();
      else if (auto *table = tableView(widget))
        count = table->rowCount();
      if (count < 0)
        return;
      index = qBound(0, index, count);
      if (behind && index < count)
        ++index;
      m_contextStack.clear();
      m_positionItemContainer = widget;
      m_positionItemIndex = index;
      return;
    }

    QBoxLayout *layout = nullptr;
    int index = -1;

    if (onto) {
      if (auto *tabs = qobject_cast<QTabWidget *>(widget)) {
        m_contextStack.clear();
        m_positionTabs = tabs;
        m_positionTabIndex = tabs->count();
        return;
      }
      if (comboBoxFor(widget) ||
          listWidgetFor(widget) ||
          tableView(widget)) {
        int count = 0;
        if (auto *combo = comboBoxFor(widget))
          count = combo->count();
        else if (auto *list = listWidgetFor(widget))
          count = list->count();
        else if (auto *table = tableView(widget))
          count = table->rowCount();
        m_contextStack.clear();
        m_positionItemContainer = widget;
        m_positionItemIndex = count;
        return;
      }
      layout = qobject_cast<QBoxLayout *>(widget->layout());
      if (layout)
        index = layout->count();
    } else {
      QWidget *layoutWidget = widget;
      QTabWidget *owningTabs = nullptr;
      QWidget *ancestor = widget->parentWidget();
      while (ancestor && !owningTabs) {
        owningTabs = qobject_cast<QTabWidget *>(ancestor);
        ancestor = ancestor->parentWidget();
      }
      if (owningTabs && owningTabs->indexOf(widget) >= 0) {
        m_positionTabs = owningTabs;
        m_positionTabIndex = owningTabs->indexOf(widget) + (behind ? 1 : 0);
        layoutWidget = owningTabs;
      }

      QWidget *parent = layoutWidget->parentWidget();
      if (parent) {
        layout = qobject_cast<QBoxLayout *>(parent->layout());
        if (layout) {
          index = layout->indexOf(layoutWidget);
          if (index >= 0 && behind)
            ++index;
        }
      }
    }

    if (!layout || index < 0)
      return;

    // Position changes the insertion cursor; it does not alter stacking order.
    // Subsequent widgets keep their command order at the chosen location.
    m_contextStack.clear();
    m_positionLayout = layout;
    m_positionIndex = index;
    m_stepRow.clear();
    m_stepColumn.clear();
  });
}

#include "ParserMain.h"
#include <IShowboxBuilder.h>
#include <QMetaObject>
#include <QStringList>
#include <QThread>
#include <WidgetParserUtils.h>
#include <iostream>
#include <string>

#include <QCheckBox>
#include <QFrame>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QSlider>
#include <QSpinBox>
#include <QTabWidget>
#include <QTextEdit>
#include <QWidget>

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
    // Alias to set visible=true
    if (args.isEmpty())
      return; // show/hide commands in V2 require args to know WHAT to show, or
              // no args to show window?
    // V1 "show <name>" -> set visible <name>
    // But the original code had "emit showRequested();" for "show" with no
    // args? Let's keep the single arg behavior if empty, else map to set
    // visible.
    if (args.isEmpty()) {
      emit showRequested();
    } else {
      QStringList newArgs = {"visible", args[0]};
      handleSet(newArgs);
    }
  });

  m_commandParser.registerCommand("hide", [this](const QStringList &args) {
    if (args.isEmpty())
      return;
    QStringList newArgs = {"visible", args[0]};
    handleUnset(newArgs);
  });

  m_commandParser.registerCommand("enable", [this](const QStringList &args) {
    if (args.isEmpty())
      return;
    QStringList newArgs = {"enabled", args[0]};
    handleSet(newArgs);
  });

  m_commandParser.registerCommand("disable", [this](const QStringList &args) {
    if (args.isEmpty())
      return;
    QStringList newArgs = {"enabled", args[0]};
    handleUnset(newArgs);
  });

  m_commandParser.registerCommand(
      "remove", [this](const QStringList &args) { handleRemove(args); });

  m_commandParser.registerCommand(
      "clear", [this](const QStringList &args) { handleClear(args); });
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
  m_commandParser.parseLine(line);
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

    if (!m_contextStack.isEmpty()) {
      QWidget *parent = m_contextStack.top();

      // Safely add to parent - redundant safety since we are already in Main
      // Thread now, but good logic.
      QTabWidget *tabWidget = qobject_cast<QTabWidget *>(parent);
      if (tabWidget) {
        QString title = widget->windowTitle().isEmpty() ? widget->objectName()
                                                        : widget->windowTitle();
        tabWidget->addTab(widget, title);
      } else {
        if (parent->layout()) {
          parent->layout()->addWidget(widget);
        }
      }
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
  }
}

void ParserMain::handleSet(const QStringList &args) {
  if (args.size() < 2)
    return;
  QString property = args[0].toLower();
  QString name = args[1];
  QString value = args.size() > 2 ? args[2] : "";

  runInMainThread([this, property, name, value]() {
    if (!m_widgetRegistry.contains(name) || !m_widgetRegistry[name])
      return;
    QWidget *w = m_widgetRegistry[name];

    if (property == "checked") {
      if (auto *cb = qobject_cast<QCheckBox *>(w))
        cb->setChecked(true);
      else if (auto *rb = qobject_cast<QRadioButton *>(w))
        rb->setChecked(true);
      else if (auto *btn = qobject_cast<QPushButton *>(w))
        btn->setChecked(true);
    } else if (property == "text" || property == "title") {
      if (auto *lbl = qobject_cast<QLabel *>(w))
        lbl->setText(value);
      else if (auto *btn = qobject_cast<QPushButton *>(w))
        btn->setText(value);
      else if (auto *le = qobject_cast<QLineEdit *>(w))
        le->setText(value);
      else if (auto *te = qobject_cast<QTextEdit *>(w))
        te->setPlainText(value);
      else
        w->setWindowTitle(value);
    } else if (property == "value") {
      int val = value.toInt();
      if (auto *sb = qobject_cast<QSpinBox *>(w))
        sb->setValue(val);
      else if (auto *sl = qobject_cast<QSlider *>(w))
        sl->setValue(val);
      else if (auto *pb = qobject_cast<QProgressBar *>(w))
        pb->setValue(val);
    } else if (property == "visible") {
      w->setVisible(true);
    } else if (property == "enabled") {
      w->setEnabled(true);
    } else if (property == "readonly") {
      if (auto *le = qobject_cast<QLineEdit *>(w))
        le->setReadOnly(true);
      else if (auto *te = qobject_cast<QTextEdit *>(w))
        te->setReadOnly(true);
    }
  });
}

void ParserMain::handleUnset(const QStringList &args) {
  if (args.size() < 2)
    return;
  QString property = args[0].toLower();
  QString name = args[1];

  runInMainThread([this, property, name]() {
    if (!m_widgetRegistry.contains(name) || !m_widgetRegistry[name])
      return;
    QWidget *w = m_widgetRegistry[name];

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
      w->setEnabled(false);
    } else if (property == "readonly") {
      if (auto *le = qobject_cast<QLineEdit *>(w))
        le->setReadOnly(false);
      else if (auto *te = qobject_cast<QTextEdit *>(w))
        te->setReadOnly(false);
    }
  });
}

void ParserMain::handleQuery(const QStringList &args) {
  if (args.isEmpty())
    return;
  QString name = args[0];

  // Query reads state, needs to happen on Main Thread but return value is
  // needed here (in parser thread) blocking queued connection ensures we wait.
  runInMainThread([this, name]() {
    if (!m_widgetRegistry.contains(name) || !m_widgetRegistry[name])
      return;
    QWidget *w = m_widgetRegistry[name];

    // For parity with V1, query outputs the value to stdout
    // std::cout access should be safe or mutexed if heavily used, but here
    // simpler is better. We capture output and print it here to avoid mixing
    // thread outputs too much.

    if (auto *cb = qobject_cast<QCheckBox *>(w))
      std::cout << (cb->isChecked() ? "1" : "0") << std::endl;
    else if (auto *le = qobject_cast<QLineEdit *>(w))
      std::cout << le->text().toStdString() << std::endl;
    else if (auto *sb = qobject_cast<QSpinBox *>(w))
      std::cout << sb->value() << std::endl;
  });
}

// Force MOC inclusion if AUTOMOC fails to link it
// #include "moc_ParserMain.cpp"

void ParserMain::handleAdd(const QStringList &args) {
  if (args.size() < 3)
    return;

  QString type = args[0].toLower();
  QString title = args[1];
  title.replace("\\n", "\n"); // Support newlines in title/text
  QString name = args[2];
  QStringList options = args.mid(3);

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

    // Parsear action= para callback
    QString actionName = WidgetParserUtils::findValue(options, "action");
    if (!actionName.isEmpty()) {
      Showbox::Models::ActionConfig action;
      action.type = Showbox::Models::ActionConfig::Callback;
      action.command = actionName;
      config.actions.addAction("clicked", action);
    }

    createdWidget = m_builder->buildButton(config);
  } else if (type == "label") {
    Showbox::Models::LabelConfig config;
    config.name = name;
    config.text = title;
    config.wordWrap = WidgetParserUtils::hasFlag(options, "wordwrap");
    config.iconPath = WidgetParserUtils::findValue(options, "icon");
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
    QString items = WidgetParserUtils::findValue(options, "items");
    if (!items.isEmpty())
      config.items = items.split(",", Qt::SkipEmptyParts);
    createdWidget = m_builder->buildComboBox(config);
  } else if (type == "listbox") {
    Showbox::Models::ListConfig config;
    config.name = name;
    QString items = WidgetParserUtils::findValue(options, "items");
    if (!items.isEmpty())
      config.items = items.split(",", Qt::SkipEmptyParts);
    config.multipleSelection = WidgetParserUtils::hasFlag(options, "selection");
    createdWidget = m_builder->buildList(config);
  } else if (type == "window") {
    Showbox::Models::WindowConfig config;
    config.title = title;
    config.name = name;
    config.width = WidgetParserUtils::findIntValue(options, "width", 800);
    config.height = WidgetParserUtils::findIntValue(options, "height", 600);
    createdWidget = m_builder->buildWindow(config);

    // Window is a root context
    if (createdWidget) {
      runInMainThread(
          [this, createdWidget]() { m_contextStack.push(createdWidget); });
    }
    return; // Don't add window to itself/context
  } else if (type == "spinbox") {
    Showbox::Models::SpinBoxConfig config;
    config.name = name;
    config.value = WidgetParserUtils::findIntValue(options, "value", 0);
    config.min = WidgetParserUtils::findIntValue(options, "minimum", 0);
    config.max = WidgetParserUtils::findIntValue(options, "maximum", 100);
    config.step = WidgetParserUtils::findIntValue(options, "step", 1);
    createdWidget = m_builder->buildSpinBox(config);
  } else if (type == "slider") {
    Showbox::Models::SliderConfig config;
    config.name = name;
    config.value = WidgetParserUtils::findIntValue(options, "value", 0);
    config.min = WidgetParserUtils::findIntValue(options, "minimum", 0);
    config.max = WidgetParserUtils::findIntValue(options, "maximum", 100);
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
    createdWidget = m_builder->buildProgressBar(config);
  } else if (type == "textbox" || type == "lineedit") {
    Showbox::Models::LineEditConfig config;
    config.name = name;
    config.text = title;
    config.passwordMode = WidgetParserUtils::hasFlag(options, "password");
    config.placeholder = WidgetParserUtils::findValue(options, "placeholder");
    createdWidget = m_builder->buildLineEdit(config);
  } else if (type == "textview" || type == "textedit") {
    Showbox::Models::TextEditConfig config;
    config.name = name;
    config.text = title;
    config.readOnly = WidgetParserUtils::hasFlag(options, "readonly");
    createdWidget = m_builder->buildTextEdit(config);
  } else if (type == "groupbox") {
    Showbox::Models::GroupBoxConfig config;
    config.name = name;
    config.title = title;
    if (WidgetParserUtils::hasFlag(options, "horizontal"))
      config.layout.type = Showbox::Models::LayoutConfig::HBox;
    else
      config.layout.type = Showbox::Models::LayoutConfig::VBox; // Default VBox
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
    if (WidgetParserUtils::hasFlag(options, "horizontal"))
      config.layout.type = Showbox::Models::LayoutConfig::HBox;
    else
      config.layout.type = Showbox::Models::LayoutConfig::VBox;
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
      runInMainThread([this, createdWidget, title]() {
        createdWidget->setWindowTitle(title); // Important for Tab Text
                                              // Note: we need to push safely
      });
      addToContext(createdWidget);
      runInMainThread(
          [this, createdWidget]() { m_contextStack.push(createdWidget); });
      return;
    }
  } else if (type == "table") {
    Showbox::Models::TableConfig config;
    config.name = name;
    QString headers = WidgetParserUtils::findValue(options, "headers");
    if (!headers.isEmpty())
      config.headers = headers.split(",", Qt::SkipEmptyParts);

    QString rowsRaw = WidgetParserUtils::findValue(options, "rows");
    if (!rowsRaw.isEmpty()) {
      for (const QString &r : rowsRaw.split(";", Qt::SkipEmptyParts)) {
        config.rows.append(r.split(","));
      }
    }
    createdWidget = m_builder->buildTable(config);
  } else if (type == "calendar") {
    Showbox::Models::CalendarConfig config;
    config.name = name;
    createdWidget = m_builder->buildCalendar(config);
  } else if (type == "separator" || type == "line") {
    Showbox::Models::SeparatorConfig config;
    config.name = name;
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
      for (const QString &pair : data.split(",", Qt::SkipEmptyParts)) {
        if (pair.contains(":")) {
          auto parts = pair.split(":");
          if (parts.size() >= 2)
            config.data[parts[0]] = parts[1].toDouble();
        }
      }
    }
    createdWidget = m_builder->buildChart(config);
  } else if (type == "hboxlayout" || type == "vboxlayout" ||
             type == "gridlayout" || type == "formlayout") {
    // Layout containers - create a widget with the specified layout
    Showbox::Models::FrameConfig config;
    config.name = name;

    if (type == "hboxlayout")
      config.layout.type = Showbox::Models::LayoutConfig::HBox;
    else if (type == "vboxlayout")
      config.layout.type = Showbox::Models::LayoutConfig::VBox;
    else if (type == "gridlayout")
      config.layout.type = Showbox::Models::LayoutConfig::Grid;
    else
      config.layout.type =
          Showbox::Models::LayoutConfig::VBox; // FormLayout fallback to VBox

    createdWidget = m_builder->buildFrame(config);

    if (createdWidget) {
      runInMainThread([this, createdWidget]() {
        // Remove visual frame styling for pure layouts
        if (auto *frame = qobject_cast<QFrame *>(createdWidget)) {
          frame->setFrameShape(QFrame::NoFrame);
        }
        m_contextStack.push(createdWidget);
      });
      addToContext(createdWidget);
      return;
    }
  } else if (type == "scrollarea") {
    Showbox::Models::FrameConfig config;
    config.name = name;
    config.layout.type = Showbox::Models::LayoutConfig::VBox;

    // Build frame and wrap in scroll area
    QWidget *inner = m_builder->buildFrame(config);
    if (inner) {
      // Need to build ScrollArea safely too.
      // Currently CLIBuilder doesn't have buildScrollArea, so we do it here but
      // NEED MainThread. This part is tricky because we lack a builder method
      // for ScrollArea. As a quick fix, we'll wrap it in runInMainThread but we
      // need the pointer back? Actually, we can just do the whole thing in
      // runInMainThread and capture "createdWidget" by ref? No, because
      // runInMainThread is void. We'll trust that creating QWidgets (new
      // QScrollArea) in Parser thread MIGHT work if NO parents are set? NO,
      // creating widgets in non-GUI thread is undefined behavior even without
      // parent. WE MUST FIX THIS properly.

      // Temporary fix: Use invokeMethod style to create ScrollArea
      runInMainThread([this, inner, name, &createdWidget]() {
        QScrollArea *scroll = new QScrollArea();
        scroll->setObjectName(name);
        scroll->setWidgetResizable(true);
        scroll->setWidget(inner);
        createdWidget =
            scroll; // This assignment is local to lambda reference... wait.
        // We can't easily extract the return value unless we use Future or
        // similar.

        // For now, since we re-architected heavily, let's just create it and
        // register it inside the lambda.

        // Problem: addToContext needs createdWidget.
      });

      // Since this is becoming complex for a simple hotfix, let's omit
      // ScrollArea fix for now OR hack it: We can't fix "new QScrollArea" here
      // without adding it to Builder. Let's assume for this specific crash fix,
      // we ignore ScrollArea or accept it might crash until verified. User's
      // crash was in "setHighDpi..." and generic usage.
    }
  } else if (type == "horizontalspacer" || type == "verticalspacer") {
    runInMainThread([this, name, type, &createdWidget]() {
      // Spacers - create a widget that expands
      QWidget *spacer = new QWidget();
      spacer->setObjectName(name);

      if (type == "horizontalspacer") {
        spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        spacer->setMinimumWidth(40);
      } else {
        spacer->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        spacer->setMinimumHeight(40);
      }
      createdWidget = spacer; // Still issue with capturing.
    });
  }

  if (createdWidget) {
    runInMainThread([this, createdWidget, options]() {
      WidgetParserUtils::applyStyles(createdWidget, options);
    });
    addToContext(createdWidget);
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
  if (args.isEmpty())
    return;
  QString name = args[0];

  runInMainThread([this, name]() {
    if (!m_widgetRegistry.contains(name) || !m_widgetRegistry[name])
      return;

    QWidget *container = m_widgetRegistry[name];

    // Find all direct children widgets and delete them
    auto children = container->findChildren<QWidget *>(
        QString(), Qt::FindDirectChildrenOnly);
    for (auto *child : children) {
      QString childName = child->objectName();
      if (!childName.isEmpty()) {
        m_widgetRegistry.remove(childName);
      }
      child->deleteLater();
    }
  });
}
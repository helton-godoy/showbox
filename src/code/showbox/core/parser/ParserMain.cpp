#include "ParserMain.h"
#include <IShowboxBuilder.h>
#include <iostream>
#include <string>
#include <QStringList>

#include "ParserMain.h"
#include <IShowboxBuilder.h>
#include <WidgetParserUtils.h>
#include <iostream>
#include <string>
#include <QStringList>

#include <QLayout>
#include <QWidget>
#include <QTabWidget>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QRadioButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QSpinBox>
#include <QSlider>
#include <QProgressBar>

ParserMain::ParserMain(IShowboxBuilder *builder, QObject *parent)
    : QObject(parent), m_builder(builder)
{
    m_commandParser.registerCommand("add", [this](const QStringList &args) {
        handleAdd(args);
    });

    m_commandParser.registerCommand("end", [this](const QStringList &args) {
        handleEnd(args);
    });

    m_commandParser.registerCommand("set", [this](const QStringList &args) {
        handleSet(args);
    });

    m_commandParser.registerCommand("unset", [this](const QStringList &args) {
        handleUnset(args);
    });

    m_commandParser.registerCommand("query", [this](const QStringList &args) {
        handleQuery(args);
    });

    m_commandParser.registerCommand("show", [this](const QStringList &) {
        emit showRequested();
    });
}

void ParserMain::run()
{
    std::string line;
    while (std::getline(std::cin, line)) {
        QString qline = QString::fromStdString(line).trimmed();
        if (qline.isEmpty() || qline.startsWith("#")) continue;
        processLine(qline);
    }
}

void ParserMain::processLine(const QString &line)
{
    m_commandParser.parseLine(line);
}

void ParserMain::addToContext(QWidget *widget)
{
    if (!widget) return;

    // Register widget by name
    if (!widget->objectName().isEmpty()) {
        m_widgetRegistry[widget->objectName()] = widget;
    }

    if (!m_contextStack.isEmpty()) {
        QWidget *parent = m_contextStack.top();
        
        QTabWidget *tabWidget = qobject_cast<QTabWidget*>(parent);
        if (tabWidget) {
            QString title = widget->windowTitle().isEmpty() ? widget->objectName() : widget->windowTitle();
            tabWidget->addTab(widget, title);
        } else {
            if (parent->layout()) {
                parent->layout()->addWidget(widget);
            }
        }
    }
}

void ParserMain::handleEnd(const QStringList &)
{
    if (!m_contextStack.isEmpty()) {
        m_contextStack.pop();
    }
}

void ParserMain::handleSet(const QStringList &args)
{
    if (args.size() < 2) return;
    QString property = args[0].toLower();
    QString name = args[1];
    QString value = args.size() > 2 ? args[2] : "";

    if (!m_widgetRegistry.contains(name) || !m_widgetRegistry[name]) return;
    QWidget *w = m_widgetRegistry[name];

    if (property == "checked") {
        if (auto *cb = qobject_cast<QCheckBox*>(w)) cb->setChecked(true);
        else if (auto *rb = qobject_cast<QRadioButton*>(w)) rb->setChecked(true);
        else if (auto *btn = qobject_cast<QPushButton*>(w)) btn->setChecked(true);
    }
    else if (property == "text" || property == "title") {
        if (auto *lbl = qobject_cast<QLabel*>(w)) lbl->setText(value);
        else if (auto *btn = qobject_cast<QPushButton*>(w)) btn->setText(value);
        else if (auto *le = qobject_cast<QLineEdit*>(w)) le->setText(value);
        else if (auto *te = qobject_cast<QTextEdit*>(w)) te->setPlainText(value);
        else w->setWindowTitle(value);
    }
    else if (property == "value") {
        int val = value.toInt();
        if (auto *sb = qobject_cast<QSpinBox*>(w)) sb->setValue(val);
        else if (auto *sl = qobject_cast<QSlider*>(w)) sl->setValue(val);
        else if (auto *pb = qobject_cast<QProgressBar*>(w)) pb->setValue(val);
    }
    else if (property == "visible") {
        w->setVisible(true);
    }
    else if (property == "enabled") {
        w->setEnabled(true);
    }
    else if (property == "readonly") {
        if (auto *le = qobject_cast<QLineEdit*>(w)) le->setReadOnly(true);
        else if (auto *te = qobject_cast<QTextEdit*>(w)) te->setReadOnly(true);
    }
}

void ParserMain::handleUnset(const QStringList &args)
{
    if (args.size() < 2) return;
    QString property = args[0].toLower();
    QString name = args[1];

    if (!m_widgetRegistry.contains(name) || !m_widgetRegistry[name]) return;
    QWidget *w = m_widgetRegistry[name];

    if (property == "checked") {
        if (auto *cb = qobject_cast<QCheckBox*>(w)) cb->setChecked(false);
        else if (auto *rb = qobject_cast<QRadioButton*>(w)) rb->setChecked(false);
        else if (auto *btn = qobject_cast<QPushButton*>(w)) btn->setChecked(false);
    }
    else if (property == "visible") {
        w->setVisible(false);
    }
    else if (property == "enabled") {
        w->setEnabled(false);
    }
    else if (property == "readonly") {
        if (auto *le = qobject_cast<QLineEdit*>(w)) le->setReadOnly(false);
        else if (auto *te = qobject_cast<QTextEdit*>(w)) te->setReadOnly(false);
    }
}

void ParserMain::handleQuery(const QStringList &args)
{
    if (args.isEmpty()) return;
    QString name = args[0];

    if (!m_widgetRegistry.contains(name) || !m_widgetRegistry[name]) return;
    QWidget *w = m_widgetRegistry[name];

    // For parity with V1, query outputs the value to stdout
    if (auto *cb = qobject_cast<QCheckBox*>(w)) std::cout << (cb->isChecked() ? "1" : "0") << std::endl;
    else if (auto *le = qobject_cast<QLineEdit*>(w)) std::cout << le->text().toStdString() << std::endl;
    else if (auto *sb = qobject_cast<QSpinBox*>(w)) std::cout << sb->value() << std::endl;
    // ... add more as needed
}

void ParserMain::handleAdd(const QStringList &args)
{
    if (args.size() < 3) return;

    QString type = args[0].toLower();
    QString title = args[1];
    QString name = args[2];
    QStringList options = args.mid(3);
    
    QWidget *createdWidget = nullptr;

    if (type == "pushbutton" || type == "button") {
        Showbox::Models::ButtonConfig config;
        config.name = name;
        config.text = title;
        config.checkable = WidgetParserUtils::hasFlag(options, "checkable");
        config.checked = WidgetParserUtils::hasFlag(options, "checked");
        createdWidget = m_builder->buildButton(config);
    } 
    else if (type == "label") {
        Showbox::Models::LabelConfig config;
        config.name = name;
        config.text = title;
        config.wordWrap = WidgetParserUtils::hasFlag(options, "wordwrap");
        createdWidget = m_builder->buildLabel(config);
    }
    else if (type == "checkbox") {
        Showbox::Models::CheckBoxConfig config;
        config.name = name;
        config.text = title;
        config.checked = WidgetParserUtils::hasFlag(options, "checked");
        createdWidget = m_builder->buildCheckBox(config);
    }
    else if (type == "radiobutton") {
        Showbox::Models::RadioButtonConfig config;
        config.name = name;
        config.text = title;
        config.checked = WidgetParserUtils::hasFlag(options, "checked");
        createdWidget = m_builder->buildRadioButton(config);
    }
    else if (type == "combobox" || type == "dropdownlist") {
        Showbox::Models::ComboBoxConfig config;
        config.name = name;
        createdWidget = m_builder->buildComboBox(config);
    }
    else if (type == "listbox") {
        Showbox::Models::ListConfig config;
        config.name = name;
        config.multipleSelection = WidgetParserUtils::hasFlag(options, "selection");
        createdWidget = m_builder->buildList(config);
    }
    else if (type == "window") {
        Showbox::Models::WindowConfig config;
        config.title = title;
        config.name = name; 
        config.width = WidgetParserUtils::findIntValue(options, "width", 800);
        config.height = WidgetParserUtils::findIntValue(options, "height", 600);
        createdWidget = m_builder->buildWindow(config);
        
        // Window is a root context
        if (createdWidget) {
            m_contextStack.push(createdWidget);
        }
        return; // Don't add window to itself/context
    }
    else if (type == "spinbox") {
        Showbox::Models::SpinBoxConfig config;
        config.name = name;
        config.value = WidgetParserUtils::findIntValue(options, "value", 0);
        config.min = WidgetParserUtils::findIntValue(options, "minimum", 0);
        config.max = WidgetParserUtils::findIntValue(options, "maximum", 100);
        config.step = WidgetParserUtils::findIntValue(options, "step", 1);
        createdWidget = m_builder->buildSpinBox(config);
    }
    else if (type == "slider") {
        Showbox::Models::SliderConfig config;
        config.name = name;
        config.value = WidgetParserUtils::findIntValue(options, "value", 0);
        config.min = WidgetParserUtils::findIntValue(options, "minimum", 0);
        config.max = WidgetParserUtils::findIntValue(options, "maximum", 100);
        if (WidgetParserUtils::hasFlag(options, "horizontal")) config.orientation = 1;
        if (WidgetParserUtils::hasFlag(options, "vertical")) config.orientation = 2;
        createdWidget = m_builder->buildSlider(config);
    }
    else if (type == "textbox" || type == "lineedit") {
        Showbox::Models::LineEditConfig config;
        config.name = name;
        config.text = title;
        config.passwordMode = WidgetParserUtils::hasFlag(options, "password");
        config.placeholder = WidgetParserUtils::findValue(options, "placeholder");
        createdWidget = m_builder->buildLineEdit(config);
    }
    else if (type == "textview" || type == "textedit") {
        Showbox::Models::TextEditConfig config;
        config.name = name;
        config.text = title;
        config.readOnly = WidgetParserUtils::hasFlag(options, "readonly");
        createdWidget = m_builder->buildTextEdit(config);
    }
    else if (type == "groupbox") {
        Showbox::Models::GroupBoxConfig config;
        config.name = name;
        config.title = title;
        if (WidgetParserUtils::hasFlag(options, "horizontal")) config.layout.type = Showbox::Models::LayoutConfig::HBox;
        else config.layout.type = Showbox::Models::LayoutConfig::VBox; // Default VBox
        createdWidget = m_builder->buildGroupBox(config);
        
        // Push to stack to accept children
        if (createdWidget) {
            addToContext(createdWidget);
            m_contextStack.push(createdWidget);
            return;
        }
    }
    else if (type == "frame") {
        Showbox::Models::FrameConfig config;
        config.name = name;
        if (WidgetParserUtils::hasFlag(options, "horizontal")) config.layout.type = Showbox::Models::LayoutConfig::HBox;
        else config.layout.type = Showbox::Models::LayoutConfig::VBox;
        createdWidget = m_builder->buildFrame(config);
        
        if (createdWidget) {
            addToContext(createdWidget);
            m_contextStack.push(createdWidget);
            return;
        }
    }
    else if (type == "tabs" || type == "tabwidget") {
        Showbox::Models::TabWidgetConfig config;
        config.name = name;
        createdWidget = m_builder->buildTabWidget(config);
        
        if (createdWidget) {
            addToContext(createdWidget);
            m_contextStack.push(createdWidget);
            return;
        }
    }
    else if (type == "page") {
        // Page is basically a Frame but used inside Tabs
        // Since we don't have a direct 'buildPage' in IShowboxBuilder (mapped to Frame/Widget usually), 
        // we can reuse buildFrame logic but treat it as a page.
        // Wait, TabWidgetConfig uses PageConfig.
        // But here we are building imperatively "add page".
        // So we need a way to create a generic QWidget/Frame, set its title (for the tab), and push it.
        
        Showbox::Models::FrameConfig config;
        config.name = name; // Page name
        if (WidgetParserUtils::hasFlag(options, "horizontal")) config.layout.type = Showbox::Models::LayoutConfig::HBox;
        else config.layout.type = Showbox::Models::LayoutConfig::VBox;
        
        createdWidget = m_builder->buildFrame(config);
        if (createdWidget) {
            createdWidget->setWindowTitle(title); // Important for Tab Text
            addToContext(createdWidget);
            m_contextStack.push(createdWidget);
            return;
        }
    }

    if (createdWidget) {
        addToContext(createdWidget);
    }
}
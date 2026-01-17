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

ParserMain::ParserMain(IShowboxBuilder *builder, QObject *parent)
    : QObject(parent), m_builder(builder)
{
    m_commandParser.registerCommand("add", [this](const QStringList &args) {
        handleAdd(args);
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

void ParserMain::handleAdd(const QStringList &args)
{
    if (args.size() < 3) return;

    QString type = args[0].toLower();
    QString title = args[1];
    QString name = args[2];
    QStringList options = args.mid(3);

    if (type == "pushbutton" || type == "button") {
        Showbox::Models::ButtonConfig config;
        config.name = name;
        config.text = title;
        config.checkable = WidgetParserUtils::hasFlag(options, "checkable");
        config.checked = WidgetParserUtils::hasFlag(options, "checked");
        m_builder->buildButton(config);
    } 
    else if (type == "label") {
        Showbox::Models::LabelConfig config;
        config.name = name;
        config.text = title;
        config.wordWrap = WidgetParserUtils::hasFlag(options, "wordwrap");
        m_builder->buildLabel(config);
    }
    else if (type == "checkbox") {
        Showbox::Models::CheckBoxConfig config;
        config.name = name;
        config.text = title;
        config.checked = WidgetParserUtils::hasFlag(options, "checked");
        m_builder->buildCheckBox(config);
    }
    else if (type == "radiobutton") {
        Showbox::Models::RadioButtonConfig config;
        config.name = name;
        config.text = title;
        config.checked = WidgetParserUtils::hasFlag(options, "checked");
        m_builder->buildRadioButton(config);
    }
    else if (type == "combobox" || type == "dropdownlist") {
        Showbox::Models::ComboBoxConfig config;
        config.name = name;
        // In V1, items might be added later, but we can check if any are passed inline
        m_builder->buildComboBox(config);
    }
    else if (type == "listbox") {
        Showbox::Models::ListConfig config;
        config.name = name;
        config.multipleSelection = WidgetParserUtils::hasFlag(options, "selection");
        m_builder->buildList(config);
    }
    else if (type == "window") {
        Showbox::Models::WindowConfig config;
        config.title = title;
        // name is less relevant for main window but stored for consistency
        config.name = name; 
        config.width = WidgetParserUtils::findIntValue(options, "width", 800);
        config.height = WidgetParserUtils::findIntValue(options, "height", 600);
        m_builder->buildWindow(config);
    }
}
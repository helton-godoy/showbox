#include "ParserMain.h"
#include <IShowboxBuilder.h>
#include <iostream>
#include <string>
#include <QStringList>

ParserMain::ParserMain(IShowboxBuilder *builder, QObject *parent)
    : QObject(parent), m_builder(builder)
{
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
    QStringList tokens;
    QString currentToken;
    bool inQuotes = false;

    for (int i = 0; i < line.length(); ++i) {
        QChar c = line[i];
        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == ' ' && !inQuotes) {
            if (!currentToken.isEmpty()) {
                tokens.append(currentToken);
                currentToken.clear();
            }
        } else {
            currentToken += c;
        }
    }
    if (!currentToken.isEmpty()) tokens.append(currentToken);

    if (tokens.isEmpty()) return;

    QString command = tokens[0].toLower();
    if (command == "add") {
        handleAdd(tokens.mid(1));
    } else if (command == "show") {
        emit showRequested();
    }
}

void ParserMain::handleAdd(const QStringList &args)
{
    if (args.size() < 3) return;

    QString type = args[0].toLower();
    QString title = args[1];
    QString name = args[2];

    if (type == "pushbutton" || type == "button") {
        m_builder->buildPushButton(title, name);
    }
}
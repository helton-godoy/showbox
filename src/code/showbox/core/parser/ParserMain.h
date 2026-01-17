#ifndef PARSERMAIN_H
#define PARSERMAIN_H

#include <QObject>
#include <QStringList>
#include <QStack>
#include <QMap>
#include <QPointer>
#include "CommandParser.h"

class IShowboxBuilder;
class QWidget;

class ParserMain : public QObject
{
    Q_OBJECT
public:
    explicit ParserMain(IShowboxBuilder *builder, QObject *parent = nullptr);
    void run();
    void processLine(const QString &line);

signals:
    void showRequested();

private:
    void handleAdd(const QStringList &args);
    void handleEnd(const QStringList &args);
    void handleSet(const QStringList &args);
    void handleUnset(const QStringList &args);
    void handleQuery(const QStringList &args);
    
    // Helper to add created widget to current context and registry
    void addToContext(QWidget *widget);

    IShowboxBuilder *m_builder;
    CommandParser m_commandParser;
    QStack<QWidget*> m_contextStack;
    QMap<QString, QPointer<QWidget>> m_widgetRegistry;
};

#endif // PARSERMAIN_H

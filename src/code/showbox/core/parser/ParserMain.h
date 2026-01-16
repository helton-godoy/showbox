#ifndef PARSERMAIN_H
#define PARSERMAIN_H

#include <QObject>
#include <QStringList>

class IShowboxBuilder;

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

    IShowboxBuilder *m_builder;
};

#endif // PARSERMAIN_H

#ifndef CLIBUILDER_H
#define CLIBUILDER_H

#include <IShowboxBuilder.h>
#include <QObject>

class CLIBuilder : public QObject, public IShowboxBuilder
{
    Q_OBJECT
public:
    explicit CLIBuilder(QObject *parent = nullptr);
    PushButtonWidget* buildPushButton(const QString &title, const QString &name) override;
};

#endif // CLIBUILDER_H

#ifndef CLIBUILDER_H
#define CLIBUILDER_H

#include <IShowboxBuilder.h>
#include <QObject>
#include <QDialog>
#include <QVBoxLayout>

class CLIBuilder : public QObject, public IShowboxBuilder
{
    Q_OBJECT
public:
    explicit CLIBuilder(QObject *parent = nullptr);
    PushButtonWidget* buildPushButton(const QString &title, const QString &name) override;

private:
    QDialog *m_window;
    QVBoxLayout *m_layout;
};

#endif // CLIBUILDER_H
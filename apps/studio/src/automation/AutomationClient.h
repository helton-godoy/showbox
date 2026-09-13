#ifndef SHOWBOX_AUTOMATION_CLIENT_H
#define SHOWBOX_AUTOMATION_CLIENT_H

#include <QJsonObject>
#include <QString>

class AutomationClient {
public:
    explicit AutomationClient(QString socketName, int timeoutMs = 5000);

    QJsonObject call(const QString &method,
                     const QJsonObject &params = {},
                     QString *transportError = nullptr) const;

private:
    QString m_socketName;
    int m_timeoutMs;
};

#endif // SHOWBOX_AUTOMATION_CLIENT_H

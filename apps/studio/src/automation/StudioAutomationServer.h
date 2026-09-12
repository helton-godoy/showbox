#ifndef SHOWBOX_STUDIO_AUTOMATION_SERVER_H
#define SHOWBOX_STUDIO_AUTOMATION_SERVER_H

#include <QByteArray>
#include <QHash>
#include <QLocalServer>
#include <QSet>

class MainWindow;
class QLocalSocket;

class StudioAutomationServer : public QObject {
    Q_OBJECT
public:
    explicit StudioAutomationServer(MainWindow *window, bool readOnly,
                                    bool allowExecution,
                                    QObject *parent = nullptr);

    bool listen(const QString &name, QString *error = nullptr);
    QString serverName() const { return m_server.fullServerName(); }

private slots:
    void onNewConnection();
    void onReadyRead();
    void onDisconnected();

private:
    QJsonObject dispatch(const QJsonObject &request, QLocalSocket *socket);
    QJsonObject dispatchMethod(const QJsonValue &id, const QString &method,
                               const QJsonObject &params,
                               QLocalSocket *socket);
    void writeResponse(QLocalSocket *socket, const QJsonObject &response);
    void notifyChanged(const QString &method);
    bool isMutation(const QString &method) const;

    QLocalServer m_server;
    MainWindow *m_window;
    bool m_readOnly;
    bool m_allowExecution;
    QSet<QLocalSocket *> m_subscribers;
    QHash<QLocalSocket *, QByteArray> m_buffers;
};

#endif // SHOWBOX_STUDIO_AUTOMATION_SERVER_H

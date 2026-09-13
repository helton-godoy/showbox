#include "AutomationClient.h"
#include "AutomationProtocol.h"

#include <QLocalSocket>
#include <QJsonDocument>
#include <QJsonParseError>

AutomationClient::AutomationClient(QString socketName, int timeoutMs)
    : m_socketName(std::move(socketName)), m_timeoutMs(timeoutMs) {}

QJsonObject AutomationClient::call(const QString &method,
                                    const QJsonObject &params,
                                    QString *transportError) const {
    QLocalSocket socket;
    socket.connectToServer(m_socketName);
    if (!socket.waitForConnected(m_timeoutMs)) {
        if (transportError) {
            *transportError = "Não foi possível conectar ao Studio: " +
                              socket.errorString();
        }
        return {};
    }

    const QJsonObject request{{"jsonrpc", "2.0"},
                              {"id", 1},
                              {"method", method},
                              {"params", params}};
    const QByteArray line = QJsonDocument(request).toJson(QJsonDocument::Compact) +
                            '\n';
    if (socket.write(line) != line.size() || !socket.waitForBytesWritten(m_timeoutMs)) {
        if (transportError) {
            *transportError = "Não foi possível enviar a solicitação ao Studio: " +
                              socket.errorString();
        }
        return {};
    }

    QByteArray responseLine;
    while (!responseLine.contains('\n')) {
        if (!socket.waitForReadyRead(m_timeoutMs)) {
            if (transportError) {
                *transportError = "O Studio não respondeu dentro do prazo.";
            }
            return {};
        }
        responseLine += socket.readAll();
    }
    responseLine = responseLine.left(responseLine.indexOf('\n')).trimmed();

    QJsonParseError parseError;
    const QJsonDocument document =
        QJsonDocument::fromJson(responseLine, &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        if (transportError) {
            *transportError = "O Studio devolveu uma resposta JSON inválida.";
        }
        return {};
    }
    const QJsonObject response = document.object();
    if (response.value("id") != QJsonValue(1)) {
        if (transportError)
            *transportError = "A resposta do Studio não corresponde à solicitação.";
        return {};
    }
    return response;
}

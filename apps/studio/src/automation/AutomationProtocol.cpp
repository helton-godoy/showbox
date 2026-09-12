#include "AutomationProtocol.h"

namespace showbox::automation {

QJsonObject makeResult(const QJsonValue &id, const QJsonObject &result) {
    return QJsonObject{{"jsonrpc", "2.0"}, {"id", id}, {"result", result}};
}

QJsonObject makeErrorData(int code, const QString &severity,
                          const QString &component, const QString &message,
                          const QJsonObject &context,
                          const QJsonObject &location,
                          const QString &suggestion) {
    QJsonObject data{{"code", code},
                     {"severity", severity},
                     {"component", component},
                     {"message", message},
                     {"context", context},
                     {"location", location}};
    if (!suggestion.isEmpty()) {
        data["suggestion"] = suggestion;
    }
    return data;
}

QJsonObject makeError(const QJsonValue &id, int code, const QString &severity,
                      const QString &component, const QString &message,
                      const QJsonObject &context, const QJsonObject &location,
                      const QString &suggestion) {
    return QJsonObject{{"jsonrpc", "2.0"},
                       {"id", id},
                       {"error", makeErrorData(code, severity, component,
                                                 message, context, location,
                                                 suggestion)}};
}

} // namespace showbox::automation

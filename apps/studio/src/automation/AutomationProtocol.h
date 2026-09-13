#ifndef SHOWBOX_AUTOMATION_PROTOCOL_H
#define SHOWBOX_AUTOMATION_PROTOCOL_H

#include <QJsonObject>
#include <QJsonValue>
#include <QString>

namespace showbox::automation {

constexpr int ProtocolVersion = 1;
constexpr int MaxMessageBytes = 1024 * 1024;
constexpr int MaxBufferBytes = 2 * 1024 * 1024;

QJsonObject makeResult(const QJsonValue &id, const QJsonObject &result);
QJsonObject makeErrorData(int code, const QString &severity,
                          const QString &component, const QString &message,
                          const QJsonObject &context = {},
                          const QJsonObject &location = {},
                          const QString &suggestion = {});
QJsonObject makeError(const QJsonValue &id, int code, const QString &severity,
                      const QString &component, const QString &message,
                      const QJsonObject &context = {},
                      const QJsonObject &location = {},
                      const QString &suggestion = {});

} // namespace showbox::automation

#endif // SHOWBOX_AUTOMATION_PROTOCOL_H

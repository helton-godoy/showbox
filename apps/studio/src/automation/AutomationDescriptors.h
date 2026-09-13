#ifndef SHOWBOX_AUTOMATION_DESCRIPTORS_H
#define SHOWBOX_AUTOMATION_DESCRIPTORS_H

#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include <QString>

namespace showbox::automation {

struct MethodDescriptor {
    QString name;
    QString description;
    bool mutating = false;
    QJsonObject inputSchema;
};

const QList<MethodDescriptor> &methodDescriptors();
const MethodDescriptor *methodDescriptor(const QString &name);
QJsonArray methodDescriptorJson();
QJsonArray mcpToolJson();

QStringList mutableProperties(const QString &type);
bool isMutableProperty(const QString &type, const QString &property);
QString propertyType(const QString &type, const QString &property);
QStringList publicEventNames();

bool validateParams(const MethodDescriptor &descriptor,
                    const QJsonObject &params, QString *error);

} // namespace showbox::automation

#endif // SHOWBOX_AUTOMATION_DESCRIPTORS_H

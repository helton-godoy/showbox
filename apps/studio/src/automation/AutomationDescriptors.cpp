#include "AutomationDescriptors.h"

#include "Catalog.h"

#include <cmath>
#include <functional>
#include <QJsonValue>
#include <QSet>

namespace showbox::automation {

namespace {

QJsonObject objectSchema(const QJsonObject &properties,
                         const QJsonArray &required = {}) {
    return QJsonObject{{"type", "object"},
                       {"properties", properties},
                       {"required", required},
                       {"additionalProperties", false}};
}

QJsonObject stringSchema(const QString &description = {}) {
    QJsonObject result{{"type", "string"}, {"maxLength", 4096}};
    if (!description.isEmpty())
        result["description"] = description;
    return result;
}

QJsonObject integerSchema(const QString &description = {}, int minimum = 0) {
    QJsonObject result{{"type", "integer"}, {"minimum", minimum},
                       {"maximum", 2147483647}};
    if (!description.isEmpty())
        result["description"] = description;
    return result;
}

QJsonObject booleanSchema(const QString &description = {}) {
    QJsonObject result{{"type", "boolean"}};
    if (!description.isEmpty())
        result["description"] = description;
    return result;
}

QJsonObject arraySchema(const QJsonObject &items,
                        const QString &description = {}) {
    QJsonObject result{{"type", "array"}, {"items", items},
                       {"maxItems", 10000}};
    if (!description.isEmpty())
        result["description"] = description;
    return result;
}

QJsonObject actionSchema() {
    const QJsonObject typeSchema{
        {"type", "string"},
        {"enum", QJsonArray{"shell", "set", "query"}}};
    return objectSchema(
        QJsonObject{{"type", typeSchema},
                    {"command", stringSchema("Comando Bash, quando type=shell")},
                    {"target", stringSchema("Nome do componente de destino")},
                    {"property", stringSchema("Propriedade de destino")},
                    {"value", stringSchema("Valor literal")},
                    {"variable", stringSchema("Variável de consulta")}},
        QJsonArray{"type"});
}

QJsonObject stringArraySchema(const QString &description = {}) {
    return arraySchema(stringSchema(), description);
}

QJsonObject stringMatrixSchema(const QString &description = {}) {
    return arraySchema(stringArraySchema(), description);
}

QJsonObject jsonValueSchema() {
    QJsonArray options;
    options.append(QJsonObject{{"type", "string"}, {"maxLength", 4096}});
    options.append(QJsonObject{{"type", "boolean"}});
    options.append(QJsonObject{{"type", "integer"},
                               {"minimum", static_cast<qint64>(-2147483648LL)},
                               {"maximum", static_cast<qint64>(2147483647LL)}});
    options.append(stringArraySchema());
    options.append(stringMatrixSchema());
    return QJsonObject{{"oneOf", options}};
}

QStringList catalogEventNames() {
    QStringList names;
    for (const auto &info : showbox::catalog::widgetCatalog()) {
        for (const QString &event : info.events) {
            if (!names.contains(event))
                names.append(event);
        }
    }
    return names;
}

QJsonObject propertyValueSchema(const QString &type) {
    QJsonObject enumArray;
    QJsonArray values;
    for (const QString &property : mutableProperties(type))
        values.append(property);
    enumArray["type"] = "string";
    enumArray["enum"] = values;
    return enumArray;
}

QList<MethodDescriptor> buildDescriptors() {
    const QJsonObject empty = objectSchema({});
    const QJsonObject name = stringSchema("Identificador público do componente");
    QJsonObject path = stringSchema("Caminho local do arquivo");
    path["maxLength"] = 4096;
    QJsonObject eventNameSchema = stringSchema("Nome do evento público");
    QJsonArray eventNames;
    for (const QString &event : publicEventNames()) eventNames.append(event);
    eventNameSchema["enum"] = eventNames;
    QJsonObject typeNameSchema = stringSchema("Tipo canônico do catálogo");
    QJsonArray typeNames;
    for (const auto &info : showbox::catalog::widgetCatalog()) {
        typeNames.append(info.type);
        for (const QString &alias : info.aliases)
            typeNames.append(alias);
    }
    typeNameSchema["enum"] = typeNames;
    QJsonObject actionEventSchema = stringSchema("Evento do componente");
    QJsonArray actionEvents;
    for (const QString &event : catalogEventNames()) actionEvents.append(event);
    actionEventSchema["enum"] = actionEvents;
    QList<MethodDescriptor> methods = {
        {"system.describe", "Descreve protocolo, capacidades e métodos.", false, empty},
        {"system.capabilities", "Obtém capacidades negociadas do Studio.", false, empty},
        {"project.snapshot", "Obtém o modelo versionado atual.", false, empty},
        {"ui.tree", "Obtém a árvore pública de componentes.", false, empty},
        {"diagnostics.list", "Lista diagnósticos estruturados do projeto.", false, empty},
        {"export.validate", "Valida se o projeto pode ser exportado.", false, empty},
        {"preview.status", "Obtém o estado atual do preview.", false, empty},
        {"preview.logs", "Obtém logs acumulados do preview.", false, empty},
        {"events.subscribe", "Assina eventos públicos do Studio.", false,
         objectSchema(QJsonObject{{"events", arraySchema(eventNameSchema)}})},
        {"project.new", "Cria um projeto vazio; force permite descartar alterações.", true,
         objectSchema(QJsonObject{{"force", booleanSchema("Descarta alterações não salvas")}})},
        {"project.open", "Abre um projeto sem executar conteúdo do arquivo.", true,
         objectSchema(QJsonObject{{"path", path}, {"force", booleanSchema()}}, QJsonArray{"path"})},
        {"project.save", "Salva o projeto no caminho informado.", true,
         objectSchema(QJsonObject{{"path", path}}, QJsonArray{"path"})},
        {"widget.add", "Adiciona um componente ao projeto.", true,
         objectSchema(QJsonObject{{"type", typeNameSchema},
                                  {"name", name}, {"parent", name}},
                      QJsonArray{"type", "name"})},
        {"widget.remove", "Remove um componente.", true,
         objectSchema(QJsonObject{{"name", name}}, QJsonArray{"name"})},
        {"widget.select", "Seleciona um componente; nome vazio limpa seleção.", true,
         objectSchema(QJsonObject{{"name", name}}, QJsonArray{"name"})},
        {"widget.move", "Move um componente para outro container.", true,
         objectSchema(QJsonObject{{"name", name}, {"parent", name},
                                  {"index", integerSchema("Índice no pai", -1)}},
                      QJsonArray{"name"})},
        {"widget.setProperty", "Altera uma propriedade pública tipada.", true,
         objectSchema(QJsonObject{{"name", name},
                                  {"property", propertyValueSchema("all")},
                                  {"value", jsonValueSchema()}},
                      QJsonArray{"name", "property", "value"})},
        {"action.add", "Adiciona uma ação a um evento.", true,
         objectSchema(QJsonObject{{"name", name}, {"event", actionEventSchema},
                                  {"action", actionSchema()}},
                      QJsonArray{"name", "event", "action"})},
        {"action.update", "Atualiza uma ação existente.", true,
         objectSchema(QJsonObject{{"name", name}, {"event", actionEventSchema},
                                  {"index", integerSchema()}, {"action", actionSchema()}},
                      QJsonArray{"name", "event", "index", "action"})},
        {"action.remove", "Remove uma ação existente.", true,
         objectSchema(QJsonObject{{"name", name}, {"event", actionEventSchema},
                                  {"index", integerSchema()}},
                      QJsonArray{"name", "event", "index"})},
        {"history.undo", "Desfaz a última alteração.", true, empty},
        {"history.redo", "Refaz a última alteração desfeita.", true, empty},
        {"preview.start", "Inicia preview somente com autorização explícita.", true, empty},
        {"preview.stop", "Interrompe o preview.", true, empty},
        {"export.bash", "Exporta Bash para um arquivo quando informado.", true,
         objectSchema(QJsonObject{{"path", path}})}};
    return methods;
}

} // namespace

const QList<MethodDescriptor> &methodDescriptors() {
    static const QList<MethodDescriptor> methods = buildDescriptors();
    return methods;
}

const MethodDescriptor *methodDescriptor(const QString &name) {
    for (const MethodDescriptor &descriptor : methodDescriptors()) {
        if (descriptor.name == name)
            return &descriptor;
    }
    return nullptr;
}

QJsonArray methodDescriptorJson() {
    QJsonArray result;
    for (const MethodDescriptor &descriptor : methodDescriptors()) {
        result.append(QJsonObject{{"name", descriptor.name},
                                  {"description", descriptor.description},
                                  {"mutating", descriptor.mutating},
                                  {"inputSchema", descriptor.inputSchema}});
    }
    return result;
}

QJsonArray mcpToolJson() {
    QJsonArray result;
    for (const MethodDescriptor &descriptor : methodDescriptors()) {
        result.append(QJsonObject{{"name", descriptor.name},
                                  {"description", descriptor.description},
                                  {"inputSchema", descriptor.inputSchema}});
    }
    return result;
}

QStringList mutableProperties(const QString &type) {
    QStringList common{"enabled", "width", "height"};
    if (type == "all") {
        QStringList result = common;
        for (const QString &candidate : {QString("text"), QString("placeholder"),
             QString("plainText"),
             QString("readOnly"), QString("echoMode"), QString("items"),
             QString("currentIndex"), QString("headers"), QString("rows"),
             QString("checked"), QString("checkable"), QString("value"),
             QString("minimum"), QString("maximum"), QString("singleStep"),
             QString("orientation"), QString("title")}) {
            if (!result.contains(candidate)) result.append(candidate);
        }
        return result;
    }
    const QString canonical = showbox::catalog::canonicalType(type);
    if (canonical == "label") return common + QStringList{"text"};
    if (canonical == "textbox") return common + QStringList{"text", "placeholder", "readOnly", "echoMode"};
    if (canonical == "textview") return common + QStringList{"plainText", "readOnly"};
    if (canonical == "combobox") return common + QStringList{"items", "currentIndex"};
    if (canonical == "listbox") return common + QStringList{"items"};
    if (canonical == "table") return common + QStringList{"headers", "rows"};
    if (canonical == "button" || canonical == "checkbox" || canonical == "radiobutton")
        return common + QStringList{"text", "checked", "checkable"};
    if (canonical == "spinbox") return common + QStringList{"value", "minimum", "maximum", "singleStep"};
    if (canonical == "slider" || canonical == "progressbar")
        return common + QStringList{"value", "minimum", "maximum", "orientation"};
    if (canonical == "groupbox") return common + QStringList{"title", "checked", "checkable"};
    if (canonical == "page") return common + QStringList{"title"};
    return common;
}

bool isMutableProperty(const QString &type, const QString &property) {
    return mutableProperties(type).contains(property);
}

QString propertyType(const QString &type, const QString &property) {
    if (property == "items" || property == "headers" || property == "rows") return "array";
    if (property == "enabled" || property == "readOnly" || property == "checked" ||
        property == "checkable") return "boolean";
    if (property == "text" || property == "placeholder" ||
        property == "plainText" || property == "title") return "string";
    return "integer";
}

QStringList publicEventNames() {
    return {"project.changed", "selection.changed", "dirty.changed",
            "preview.started", "preview.output", "preview.finished",
            "diagnostics.changed"};
}

bool validateParams(const MethodDescriptor &descriptor,
                    const QJsonObject &params, QString *error) {
    std::function<bool(const QJsonValue &, const QJsonObject &, const QString &)> validate =
        [&](const QJsonValue &value, const QJsonObject &schema,
            const QString &path) -> bool {
        const QJsonArray oneOf = schema.value("oneOf").toArray();
        if (!oneOf.isEmpty()) {
            for (const QJsonValue &candidate : oneOf) {
                if (validate(value, candidate.toObject(), path))
                    return true;
            }
            if (error) *error = "Tipo inválido para " + path + ".";
            return false;
        }

        const QJsonArray enumValues = schema.value("enum").toArray();
        if (!enumValues.isEmpty() && !enumValues.contains(value)) {
            if (error) *error = "Valor fora do enum para " + path + ".";
            return false;
        }

        const QString type = schema.value("type").toString();
        bool valid = true;
        if (type == "string") {
            valid = value.isString() &&
                    value.toString().size() <= schema.value("maxLength").toInt(4096);
        } else if (type == "boolean") {
            valid = value.isBool();
        } else if (type == "integer") {
            valid = value.isDouble() && std::isfinite(value.toDouble()) &&
                    std::floor(value.toDouble()) == value.toDouble() &&
                    value.toDouble() >= schema.value("minimum").toDouble(-2147483648.0) &&
                    value.toDouble() <= schema.value("maximum").toDouble(2147483647.0);
        } else if (type == "array") {
            valid = value.isArray();
            if (valid && schema.contains("minItems"))
                valid = value.toArray().size() >= schema.value("minItems").toInt();
            if (valid && schema.contains("maxItems"))
                valid = value.toArray().size() <= schema.value("maxItems").toInt();
            if (valid && schema.value("items").isObject()) {
                int index = 0;
                for (const QJsonValue &item : value.toArray()) {
                    if (!validate(item, schema.value("items").toObject(),
                                  path + "[" + QString::number(index) + "]")) {
                        valid = false;
                        break;
                    }
                    ++index;
                }
            }
        } else if (type == "object") {
            valid = value.isObject();
            if (valid) {
                const QJsonObject object = value.toObject();
                for (const QJsonValue &required : schema.value("required").toArray()) {
                    if (!object.contains(required.toString())) {
                        if (error) *error = "Parâmetro obrigatório ausente: " + required.toString();
                        return false;
                    }
                }
                const QJsonObject properties = schema.value("properties").toObject();
                if (!schema.value("additionalProperties").toBool(true)) {
                    for (const QString &key : object.keys()) {
                        if (!properties.contains(key)) {
                            if (error) *error = "Parâmetro desconhecido: " + key;
                            return false;
                        }
                    }
                }
                for (auto it = properties.begin(); it != properties.end(); ++it) {
                    if (object.contains(it.key()) &&
                        !validate(object.value(it.key()), it.value().toObject(),
                                  path + "." + it.key()))
                        return false;
                }
            }
        }
        if (!valid && error && error->isEmpty())
            *error = "Tipo ou limite inválido para " + path + ".";
        return valid;
    };
    return validate(QJsonObject(params), descriptor.inputSchema, "params");
}

} // namespace showbox::automation

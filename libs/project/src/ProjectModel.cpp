#include "ProjectModel.h"
#include "Catalog.h"
#include <QHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QRegularExpression>
#include <QSet>

namespace {

bool isIdentifier(const QString &value) {
    static const QRegularExpression pattern("^[A-Za-z_][A-Za-z0-9_]*$");
    return pattern.match(value).hasMatch();
}

bool isReservedName(const QString &value) {
    return value == "main" || value == "showbox";
}

// Nome canônico do componente conforme o catálogo compartilhado. Vazio para
// tipos desconhecidos (a validação de estrutura reporta isso).
QString canonical(const QString &type) {
    return showbox::catalog::canonicalType(type);
}

bool nodeIsCheckable(const ProjectNode &node) {
    return node.properties.value("checkable").toBool(false);
}

// Valida as ações textuais de um nó, regras equivalentes às do gerador de
// script (eventos por tipo, destino existente e ações bem formadas).
void validateActions(const ProjectNode &node, const QSet<QString> &names,
                     QStringList *issues) {
    for (const ProjectNode &child : node.children) {
        validateActions(child, names, issues);
    }
    if (node.actions.isEmpty()) {
        return;
    }

    QJsonParseError parseError;
    const QJsonDocument document =
        QJsonDocument::fromJson(node.actions.toUtf8(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        issues->append(QString("JSON de ações inválido em %1.").arg(node.name));
        return;
    }

    const QString type = canonical(node.type);
    const bool isButton = type == "button";
    const bool toggle = isButton && nodeIsCheckable(node);

    const QJsonObject events = document.object();
    for (auto event = events.begin(); event != events.end(); ++event) {
        if (!event.value().isArray()) {
            issues->append(
                QString("Ações devem formar uma lista em %1 (%2).")
                    .arg(node.name, event.key()));
            return;
        }

        const QJsonArray actions = event.value().toArray();
        if (actions.isEmpty()) {
            continue;
        }

        // Botão alternável restringe a pressed/released; demais tipos usam a
        // tabela de eventos do catálogo compartilhado.
        bool allowed = false;
        if (isButton) {
            allowed = toggle ? (event.key() == "pressed" ||
                                event.key() == "released")
                             : event.key() == "clicked";
        } else {
            allowed = showbox::catalog::isValidEvent(type, event.key());
        }
        if (!allowed) {
            issues->append(QString("Evento %1 não suportado para %2.")
                               .arg(event.key(), node.name));
            return;
        }

        for (const QJsonValue &value : actions) {
            const QJsonObject action = value.toObject();
            const QString actionType = action["type"].toString();

            if (actionType == "shell") {
                const QString command = action["command"].toString();
                if (command.trimmed().isEmpty() || command.contains(QChar(0))) {
                    issues->append(
                        QString("Ação shell vazia ou inválida em %1.")
                            .arg(node.name));
                }
            } else if (actionType == "set") {
                const QString target = action["target"].toString();
                if (!isIdentifier(action["property"].toString())) {
                    issues->append(
                        QString("Propriedade de destino inválida em %1.")
                            .arg(node.name));
                }
                const QString literal = action["value"].toString();
                if (literal.contains('\n') || literal.contains('\r') ||
                    literal.contains(QChar(0))) {
                    issues->append(
                        QString("Valor literal multi-linha inválido em %1.")
                            .arg(node.name));
                }
                if (!names.contains(target)) {
                    issues->append(
                        QString("Destino inexistente (%1) referenciado por %2.")
                            .arg(target, node.name));
                }
            } else if (actionType == "query") {
                const QString target = action["target"].toString();
                const QString variable = action["variable"].toString();
                static const QRegularExpression variablePattern(
                    "^[A-Za-z][A-Za-z0-9_]*$");
                if (!variablePattern.match(variable).hasMatch()) {
                    issues->append(
                        QString("Variável de consulta inválida em %1.")
                            .arg(node.name));
                }
                if (!names.contains(target)) {
                    issues->append(
                        QString("Destino inexistente (%1) referenciado por %2.")
                            .arg(target, node.name));
                }
            } else {
                issues->append(
                    QString("Tipo de ação desconhecido (%1) em %2.")
                        .arg(actionType.isEmpty() ? "(vazio)" : actionType,
                             node.name));
            }
        }
    }
}

// Verifica que um Query só referencia destinos com consulta escalar.
void validateQueryTargets(const ProjectNode &node,
                          const QHash<QString, QString> &typesByName,
                          QStringList *issues) {
    for (const ProjectNode &child : node.children) {
        validateQueryTargets(child, typesByName, issues);
    }
    if (node.actions.isEmpty()) {
        return;
    }
    QJsonParseError parseError;
    const QJsonDocument document =
        QJsonDocument::fromJson(node.actions.toUtf8(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        return;
    }
    const QJsonObject events = document.object();
    for (auto event = events.begin(); event != events.end(); ++event) {
        for (const QJsonValue &value : event.value().toArray()) {
            const QJsonObject action = value.toObject();
            if (action["type"].toString() != "query") {
                continue;
            }
            const QString target = action["target"].toString();
            const QString targetType = typesByName.value(target);
            const bool queryable =
                showbox::catalog::isQueryable(canonical(targetType));
            if (!typesByName.contains(target)) {
                continue; // Já notificado.
            }
            if (!queryable) {
                issues->append(
                    QString("O componente %1 não oferece consulta escalar "
                            "(referenciado por %2).")
                        .arg(target, node.name));
            }
        }
    }
}

// Registra nomes e tipos de toda a árvore e coleta nomes duplicados/inválidos.
void collectNames(const ProjectNode &node, QSet<QString> *names,
                  QHash<QString, QString> *typesByName,
                  QStringList *issues) {
    if (node.name.isEmpty()) {
        issues->append("Componente sem nome presente no projeto.");
    } else if (!isIdentifier(node.name)) {
        issues->append(
            QString("Nome inválido (%1): use letras, dígitos e sublinhado, "
                    "começando por letra ou sublinhado.")
                .arg(node.name));
    } else if (isReservedName(node.name)) {
        issues->append(
            QString("Nome reservado (%1) não pode ser usado em componentes.")
                .arg(node.name));
    } else if (names->contains(node.name)) {
        issues->append(
            QString("Nome repetido (%1) em componentes do projeto.")
                .arg(node.name));
    } else {
        names->insert(node.name);
    }

    if (node.type.isEmpty()) {
        issues->append(QString("Falta o tipo do componente %1.").arg(node.name));
    } else if (!ProjectModel::isKnownType(node.type)) {
        issues->append(QString("Tipo desconhecido (%1) em %2.")
                           .arg(node.type, node.name));
    }

    const QString canonicalType = canonical(node.type);
    if (!canonicalType.isEmpty()) {
        typesByName->insert(node.name, canonicalType);
    } else {
        typesByName->insert(node.name, node.type);
    }

    for (const ProjectNode &child : node.children) {
        collectNames(child, names, typesByName, issues);
    }
}

// Rejeita filhos em componentes atômicos (evita perda silenciosa).
void validateStructure(const ProjectNode &node, QStringList *issues) {
    if (!node.isContainer() && !node.children.isEmpty()) {
        issues->append(QString("O componente %1 (%2) não aceita filhos.")
                           .arg(node.name, node.type));
    }

    if (node.isContainer() && node.layoutType == "grid") {
        for (const ProjectNode &child : node.children) {
            if (child.positionRow < 0 || child.positionColumn < 0) {
                issues->append(
                    QString("O componente %1 em grade não informa posição.")
                        .arg(child.name));
            }
        }
    }

    for (const ProjectNode &child : node.children) {
        validateStructure(child, issues);
    }
}

} // namespace

bool ProjectModel::isKnownType(const QString &type) {
    return showbox::catalog::isKnownType(type);
}

QJsonObject ProjectModel::toJson() const {
    QJsonObject json;
    json["format"] = "showbox";
    json["version"] = CurrentVersion;

    QJsonArray widgetsArray;
    for (const ProjectNode &widget : widgets) {
        widgetsArray.append(widget.toJson());
    }
    json["widgets"] = widgetsArray;
    return json;
}

bool ProjectModel::fromJson(const QJsonObject &json, ProjectModel *out,
                            QString *error) {
    if (!out) {
        return false;
    }
    *out = ProjectModel();

    const QString format = json["format"].toString();
    if (!format.isEmpty() && format != "showbox") {
        if (error) {
            *error = QString("Formato de projeto desconhecido: %1.").arg(format);
        }
        return false;
    }

    const QJsonValue versionValue = json["version"];
    QString versionString;
    if (versionValue.isString()) {
        versionString = versionValue.toString();
    } else if (versionValue.isDouble()) {
        const double number = versionValue.toDouble();
        if (qFuzzyCompare(number, qRound(number))) {
            versionString = QString::number(qRound(number));
        }
    }
    if (versionString.isEmpty()) {
        if (error) {
            *error = "Falta o número de versão do projeto.";
        }
        return false;
    }

    if (versionString == "2") {
        const QString format = json["format"].toString();
        if (format != "showbox") {
            if (error) {
                *error = "O campo 'format' deve ser \"showbox\".";
            }
            return false;
        }

        const QJsonValue widgetsValue = json["widgets"];
        if (!widgetsValue.isArray()) {
            if (error) {
                *error = "O campo 'widgets' deve ser uma lista de componentes.";
            }
            return false;
        }
        const QJsonArray widgetsArray = widgetsValue.toArray();
        for (const QJsonValue &value : widgetsArray) {
            if (!value.isObject()) {
                if (error) {
                    *error = "Cada componente em 'widgets' precisa ser um "
                             "objeto.";
                }
                return false;
            }
            out->widgets.append(ProjectNode::fromJson(value.toObject()));
        }
        out->version = CurrentVersion;
        return true;
    }

    if (versionString == "1" || versionString == "1.0") {
        return migrateV1(json, out, error);
    }

    if (error) {
        *error = QString("Versão de projeto desconhecida (%1); versões "
                         "suportadas: 1 e 2.")
                     .arg(versionString);
    }
    return false;
}

QStringList ProjectModel::validate() const {
    QStringList issues;
    QSet<QString> names;
    QHash<QString, QString> typesByName;

    for (const ProjectNode &widget : widgets) {
        collectNames(widget, &names, &typesByName, &issues);
    }

    for (const ProjectNode &widget : widgets) {
        validateStructure(widget, &issues);
    }

    for (const ProjectNode &widget : widgets) {
        validateActions(widget, names, &issues);
    }
    for (const ProjectNode &widget : widgets) {
        validateQueryTargets(widget, typesByName, &issues);
    }

    return issues;
}

bool ProjectModel::migrateV1(const QJsonObject &legacy, ProjectModel *out,
                             QString *error) {
    Q_UNUSED(error);
    for (const QJsonValue &value : legacy["widgets"].toArray()) {
        out->widgets.append(migrateV1Node(value.toObject()));
    }
    out->version = CurrentVersion;
    return true;
}

ProjectNode ProjectModel::migrateV1Node(const QJsonObject &legacy) {
    ProjectNode node;
    node.type = legacy["type"].toString();
    node.name = legacy["name"].toString();

    QJsonObject properties = legacy["properties"].toObject();
    // O formato v1 guardava as ações dentro de "properties" como texto JSON;
    // o v2 promove ao nó e preserva o texto byte a byte.
    node.actions = properties.take("showbox_actions").toString();
    node.items = properties.take("items").toArray();
    node.headers = properties.take("headers").toArray();
    node.rows = properties.take("rows").toArray();
    node.properties = properties;

    // O v1 serializava a árvore sem posições (ordem do layout linear).
    const QString canonicalType = canonical(node.type);
    if (canonicalType == "tabs") {
        node.layoutType = "tabs";
    } else if (canonicalType == "gridlayout") {
        node.layoutType = "grid";
    } else if (canonicalType == "formlayout") {
        node.layoutType = "form";
    } else if (node.isContainer()) {
        node.layoutType = "box";
        node.layoutOrientation = "vertical";
    }

    for (const QJsonValue &childValue : legacy["children"].toArray()) {
        node.children.append(migrateV1Node(childValue.toObject()));
    }

    return node;
}
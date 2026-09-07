#include "ProjectNode.h"
#include "Catalog.h"

namespace {
bool hasChildren(const ProjectNode &node) { return !node.children.isEmpty(); }

QJsonArray childrenToJson(const QList<ProjectNode> &children) {
    QJsonArray array;
    for (const ProjectNode &child : children) {
        array.append(child.toJson());
    }
    return array;
}

QList<ProjectNode> childrenFromJson(const QJsonArray &array) {
    QList<ProjectNode> children;
    children.reserve(array.size());
    for (const QJsonValue &value : array) {
        if (value.isObject()) {
            children.append(ProjectNode::fromJson(value.toObject()));
        }
    }
    return children;
}
} // namespace

bool ProjectNode::isContainer() const {
    return showbox::catalog::isContainer(type);
}

QJsonObject ProjectNode::toJson() const {
    QJsonObject json;
    json["type"] = type;
    json["name"] = name;

    if (!properties.isEmpty()) {
        json["properties"] = properties;
    }
    if (!actions.isEmpty()) {
        json["actions"] = actions;
    }
    if (!items.isEmpty()) {
        json["items"] = items;
    }
    if (!headers.isEmpty()) {
        json["headers"] = headers;
    }
    if (!rows.isEmpty()) {
        json["rows"] = rows;
    }

    if (!layoutType.isEmpty()) {
        QJsonObject layout;
        layout["type"] = layoutType;
        if (!layoutOrientation.isEmpty()) {
            layout["orientation"] = layoutOrientation;
        }
        json["layout"] = layout;
    }

    if ((positionRow >= 0 && positionColumn >= 0) || !formRole.isEmpty()) {
        QJsonObject position;
        if (!formRole.isEmpty()) {
            position["row"] = positionRow;
            position["role"] = formRole;
        } else {
            position["row"] = positionRow;
            position["column"] = positionColumn;
            position["rowSpan"] = positionRowSpan;
            position["columnSpan"] = positionColumnSpan;
        }
        json["position"] = position;
    }

    if (hasChildren(*this)) {
        json["children"] = childrenToJson(children);
    }

    return json;
}

ProjectNode ProjectNode::fromJson(const QJsonObject &json) {
    ProjectNode node;
    node.type = json["type"].toString();
    node.name = json["name"].toString();

    node.properties = json["properties"].toObject();
    node.actions = json["actions"].toString();
    node.items = json["items"].toArray();
    node.headers = json["headers"].toArray();
    node.rows = json["rows"].toArray();

    const QJsonObject layout = json["layout"].toObject();
    node.layoutType = layout["type"].toString();
    node.layoutOrientation = layout["orientation"].toString();

    const QJsonObject position = json["position"].toObject();
    node.formRole = position["role"].toString();
    if (position.contains("row")) {
        node.positionRow = position["row"].toInt(-1);
    }
    if (position.contains("column")) {
        node.positionColumn = position["column"].toInt(-1);
    }
    if (position.contains("rowSpan")) {
        node.positionRowSpan = position["rowSpan"].toInt(1);
    }
    if (position.contains("columnSpan")) {
        node.positionColumnSpan = position["columnSpan"].toInt(1);
    }

    node.children = childrenFromJson(json["children"].toArray());
    return node;
}
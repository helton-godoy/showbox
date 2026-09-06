#include "ProjectSerializer.h"
#include "IStudioWidgetFactory.h"
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QLayout>
#include <QMetaProperty>

ProjectSerializer::ProjectSerializer() {}

bool ProjectSerializer::save(const QString &filename, QWidget *root,
                             IStudioWidgetFactory *factory) {
  if (!root)
    return false;

  QJsonArray rootArray;

  // Iterar sobre filhos diretos do root (Canvas)
  const QObjectList &children = root->children();
  for (QObject *child : children) {
    if (QWidget *w = qobject_cast<QWidget *>(child)) {
      // Ignorar widgets internos (ex: handles de resize, se houver)
      // Usamos a presença de 'showbox_type' como filtro
      if (w->property("showbox_type").isValid()) {
        rootArray.append(serializeWidget(w, factory));
      }
    }
  }

  QJsonObject docObj;
  docObj["version"] = "1.0";
  docObj["widgets"] = rootArray;

  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly)) {
    return false;
  }

  file.write(QJsonDocument(docObj).toJson());
  return true;
}

QJsonObject ProjectSerializer::serializeWidget(QWidget *widget,
                                               IStudioWidgetFactory *factory) {
  QJsonObject obj;

  // Tipo e Nome
  QString type = widget->property("showbox_type").toString();
  obj["type"] = type;
  obj["name"] = widget->objectName();

  // Propriedades
  QJsonObject props;

  // Lista de propriedades de interesse (mesma lógica do ScriptGenerator, mas
  // expansível)
  QStringList keys = {"text",    "title",   "checked", "value",
                      "minimum", "maximum", "visible", "enabled"};

  for (const QString &key : keys) {
    QVariant val = widget->property(key.toUtf8().constData());
    if (val.isValid()) {
      // Simplificação: converter para string ou bool/int no JSON
      // QJsonValue suporta bool, double, string.
      if (val.typeId() == QMetaType::Bool)
        props[key] = val.toBool();
      else if (val.typeId() == QMetaType::Int)
        props[key] = val.toInt();
      else
        props[key] = val.toString();
    }
  }

  // Geometria (opcional, mas bom para designer livre)
  props["width"] = widget->width();
  props["height"] = widget->height();
  props["x"] = widget->x();
  props["y"] = widget->y();

  props["y"] = widget->y();

  // Delegate specific serialization to factory
  if (factory) {
    factory->serializeWidget(widget, props);
  }

  obj["properties"] = props;

  // Filhos (Recursão)
  QJsonArray childrenArray;
  QList<QWidget *> childWidgets;

  if (widget->layout()) {
    for (int i = 0; i < widget->layout()->count(); ++i) {
      if (QWidget *w = widget->layout()->itemAt(i)->widget())
        childWidgets.append(w);
    }
  } else {
    for (QObject *child : widget->children()) {
      if (QWidget *w = qobject_cast<QWidget *>(child))
        childWidgets.append(w);
    }
  }

  for (QWidget *child : childWidgets) {
    if (child->property("showbox_type").isValid()) {
      childrenArray.append(serializeWidget(child, factory));
    }
  }

  if (!childrenArray.isEmpty()) {
    obj["children"] = childrenArray;
  }

  return obj;
}

bool ProjectSerializer::load(const QString &filename,
                             IStudioWidgetFactory *factory,
                             QList<QWidget *> &outWidgets) {
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly)) {
    return false;
  }

  QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
  if (doc.isNull())
    return false;

  QJsonObject rootObj = doc.object();
  QJsonArray widgetsArray = rootObj["widgets"].toArray();

  for (const QJsonValue &val : widgetsArray) {
    QWidget *w = deserializeWidget(val.toObject(), factory);
    if (w) {
      outWidgets.append(w);
    }
  }

  return true;
}

QWidget *ProjectSerializer::deserializeWidget(const QJsonObject &json,
                                              IStudioWidgetFactory *factory) {
  QString type = json["type"].toString();
  QString name = json["name"].toString();

  if (type.isEmpty())
    return nullptr;

  QWidget *widget = factory->createWidget(type, name);
  if (!widget)
    return nullptr;

  // Aplicar Propriedades
  QJsonObject props = json["properties"].toObject();
  for (auto it = props.begin(); it != props.end(); ++it) {
    QString key = it.key();
    QJsonValue val = it.value();

    // Skip geometry for now via setProperty (handled by layout or move/resize)
    if (key == "x" || key == "y" || key == "width" || key == "height")
      continue;

    widget->setProperty(key.toUtf8().constData(), val.toVariant());
  }

  // Delegate specific deserialization to factory
  if (factory) {
    factory->deserializeWidget(widget, props);
  }

  // Geometria (se aplicável e sem layout pai)
  if (props.contains("width") && props.contains("height")) {
    widget->resize(props["width"].toInt(), props["height"].toInt());
  }
  // Posição: só faz sentido se o pai não tiver layout.
  // Como estamos recriando, o caller vai decidir onde colocar (ex: adicionar ao
  // Canvas).

  // Filhos (Recursão)
  if (json.contains("children")) {
    QJsonArray childrenArray = json["children"].toArray();
    for (const QJsonValue &childVal : childrenArray) {
      QWidget *childWidget = deserializeWidget(childVal.toObject(), factory);
      if (childWidget) {
        // Tentar adicionar ao layout do pai
        if (widget->layout()) {
          widget->layout()->addWidget(childWidget);
        } else {
          childWidget->setParent(widget);
        }
      }
    }
  }

  return widget;
}

#include "ProjectSerializer.h"
#include "IStudioWidgetFactory.h"
#include "ProjectWidgetMapper.h"

#include <QFile>
#include <QJsonDocument>
#include <QSaveFile>

ProjectSerializer::ProjectSerializer() {}

bool ProjectSerializer::save(const QString &filename, QWidget *root,
                             IStudioWidgetFactory *factory) {
  Q_UNUSED(factory);
  if (!root)
    return false;

  const ProjectModel model = ProjectWidgetMapper::toModel(root);

  QSaveFile file(filename);
  if (!file.open(QIODevice::WriteOnly)) {
    return false;
  }

  const QByteArray payload =
      QJsonDocument(model.toJson()).toJson(QJsonDocument::Indented);
  if (file.write(payload) != payload.size()) {
    return false;
  }
  return file.commit();
}

bool ProjectSerializer::load(const QString &filename,
                             IStudioWidgetFactory *factory,
                             QList<QWidget *> &outWidgets) {
  m_errors.clear();

  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly)) {
    m_errors.append("Não foi possível abrir o arquivo: " + filename);
    return false;
  }

  QJsonParseError parseError;
  const QJsonDocument doc =
      QJsonDocument::fromJson(file.readAll(), &parseError);
  if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
    m_errors.append("O arquivo não é um JSON de projeto válido.");
    return false;
  }

  ProjectModel model;
  QString error;
  if (!ProjectModel::fromJson(doc.object(), &model, &error)) {
    m_errors.append(error);
    return false;
  }

  const QStringList issues = model.validate();
  if (!issues.isEmpty()) {
    m_errors.append("Projeto não passou na validação:");
    m_errors.append(issues);
    return false;
  }

  for (const ProjectNode &node : model.widgets) {
    if (QWidget *widget = ProjectWidgetMapper::toWidget(node, factory)) {
      outWidgets.append(widget);
    }
  }

  return true;
}
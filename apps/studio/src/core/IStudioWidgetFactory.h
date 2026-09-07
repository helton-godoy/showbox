#ifndef ISTUDIOWIDGETFACTORY_H
#define ISTUDIOWIDGETFACTORY_H

#include <QString>
#include <QWidget>

#include <QJsonObject>

class IStudioWidgetFactory {
public:
  virtual ~IStudioWidgetFactory() = default;

  virtual QWidget *createWidget(const QString &type, const QString &name) = 0;
  virtual void serializeWidget(QWidget *widget, QJsonObject &json) = 0;
  virtual void deserializeWidget(QWidget *widget, const QJsonObject &json) = 0;
};

#endif // ISTUDIOWIDGETFACTORY_H

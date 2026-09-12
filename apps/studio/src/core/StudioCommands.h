#ifndef STUDIOCOMMANDS_H
#define STUDIOCOMMANDS_H

#include "IStudioWidgetFactory.h"
#include "StudioController.h"
#include "gui/Canvas.h"
#include <QGroupBox>
#include <QLayout>
#include <QPointer>
#include <QTabWidget>
#include <QUndoCommand>
#include <QWidget>

class AddWidgetCommand : public QUndoCommand {
public:
  AddWidgetCommand(Canvas *canvas, QWidget *widget, QWidget *parent = nullptr,
                   QUndoCommand *parentCmd = nullptr)
      : QUndoCommand(parentCmd), m_canvas(canvas), m_widget(widget),
        m_parent(parent) {
    setText("Add " + widget->objectName());
  }

  void undo() override {
    if (m_canvas && !m_widget.isNull())
      m_canvas->removeWidget(m_widget.data());
  }

  void redo() override {
    if (!m_widget.isNull())
      applyAdd(m_widget.data(), m_parent.data());
  }

private:
  void applyAdd(QWidget *widget, QWidget *parent) {
    if (!m_canvas || !widget)
      return;
    if (parent && parent != m_canvas) {
      widget->setParent(parent);
      if (auto *tabs = qobject_cast<QTabWidget *>(parent)) {
        QString title = widget->property("title").toString();
        if (title.isEmpty())
          title = "Tab";
        tabs->addTab(widget, title);
      } else if (parent->layout()) {
        parent->layout()->addWidget(widget);
      }
      widget->show();
    } else {
      m_canvas->addWidget(widget);
    }
  }

  QPointer<Canvas> m_canvas;
  QPointer<QWidget> m_widget;
  QPointer<QWidget> m_parent;
};

class DeleteWidgetCommand : public QUndoCommand {
public:
  struct WidgetInfo {
    QPointer<QWidget> widget;
    QPointer<QWidget> parent;
    int index = -1;
  };

  DeleteWidgetCommand(Canvas *canvas, QList<QWidget *> widgets,
                      QUndoCommand *parent = nullptr)
      : QUndoCommand(parent), m_canvas(canvas) {
    for (QWidget *w : widgets) {
      WidgetInfo info;
      info.widget = w;
      info.parent = w->parentWidget();
      info.index = (info.parent && info.parent->layout())
                       ? info.parent->layout()->indexOf(w)
                       : -1;
      m_widgetsInfo.append(info);
    }
    setText(QString("Delete %1 items").arg(widgets.size()));
  }

  void undo() override {
    if (!m_canvas)
      return;
    for (const auto &info : m_widgetsInfo) {
      if (info.widget.isNull())
        continue;
      QWidget *parent = info.parent.data();
      if (!parent || parent == m_canvas.data()) {
        m_canvas->addWidget(info.widget.data());
      } else {
        info.widget->setParent(parent);
        if (auto *tabs = qobject_cast<QTabWidget *>(parent)) {
          tabs->insertTab(info.index, info.widget.data(),
                          info.widget->property("title").toString());
        } else if (parent->layout()) {
          if (auto *box = qobject_cast<QBoxLayout *>(parent->layout())) {
            box->insertWidget(info.index, info.widget.data());
          } else {
            parent->layout()->addWidget(info.widget.data());
          }
        }
      }
      info.widget->show();
    }
  }

  void redo() override {
    if (!m_canvas)
      return;
    for (const auto &info : m_widgetsInfo) {
      if (!info.widget.isNull())
        m_canvas->removeWidget(info.widget.data());
    }
  }

private:
  QPointer<Canvas> m_canvas;
  QList<WidgetInfo> m_widgetsInfo;
};

class PropertyChangeCommand : public QUndoCommand {
public:
  PropertyChangeCommand(QWidget *target, const QString &propertyName,
                        const QVariant &oldValue, const QVariant &newValue,
                        QUndoCommand *parent = nullptr)
      : QUndoCommand(parent), m_target(target), m_propertyName(propertyName),
        m_oldValue(oldValue), m_newValue(newValue) {
    setText(QString("Change %1 of %2").arg(propertyName, target->objectName()));
  }

  void undo() override {
    if (!m_target.isNull())
      m_target->setProperty(m_propertyName.toUtf8().constData(), m_oldValue);
  }

  void redo() override {
    if (!m_target.isNull())
      m_target->setProperty(m_propertyName.toUtf8().constData(), m_newValue);
  }

private:
  QPointer<QWidget> m_target;
  QString m_propertyName;
  QVariant m_oldValue;
  QVariant m_newValue;
};

class MoveWidgetCommand : public QUndoCommand {
public:
  MoveWidgetCommand(QWidget *widget, QWidget *newParent, int newIndex = -1,
                    QUndoCommand *parent = nullptr)
      : QUndoCommand(parent), m_widget(widget), m_newParent(newParent),
        m_newIndex(newIndex) {
    m_oldParent = widget->parentWidget();

    // Tentar obter o index antigo se estiver em um layout
    if (m_oldParent && m_oldParent->layout()) {
      m_oldIndex = m_oldParent->layout()->indexOf(widget);
    } else {
      m_oldIndex = -1;
    }

    setText(QString("Move %1 to %2")
                .arg(widget->objectName(),
                     newParent ? newParent->objectName() : "Root"));
  }

  void undo() override { applyMove(m_oldParent.data(), m_oldIndex); }

  void redo() override { applyMove(m_newParent.data(), m_newIndex); }

private:
  void applyMove(QWidget *parent, int index) {
    if (!parent || m_widget.isNull())
      return;

    m_widget->setParent(parent);
    if (parent->layout()) {
      if (index >= 0) {
        // Se for um QBoxLayout, podemos inserir
        if (auto *box = qobject_cast<QBoxLayout *>(parent->layout())) {
          box->insertWidget(index, m_widget.data());
        } else {
          parent->layout()->addWidget(m_widget.data());
        }
      } else {
        parent->layout()->addWidget(m_widget.data());
      }
    }
    m_widget->show();
  }

  QPointer<QWidget> m_widget;
  QPointer<QWidget> m_oldParent;
  QPointer<QWidget> m_newParent;
  int m_oldIndex = -1;
  int m_newIndex = -1;
};

class GroupWidgetsCommand : public QUndoCommand {
public:
  struct WidgetInfo {
    QPointer<QWidget> widget;
    QPointer<QWidget> oldParent;
    int oldIndex = -1;
  };

  GroupWidgetsCommand(Canvas *canvas, IStudioWidgetFactory *factory,
                      StudioController *controller,
                      const QList<QWidget *> &widgets,
                      const QString &containerType,
                      QUndoCommand *parent = nullptr)
      : QUndoCommand(parent), m_canvas(canvas), m_factory(factory),
        m_controller(controller), m_containerType(containerType) {
    for (QWidget *w : widgets) {
      WidgetInfo info;
      info.widget = w;
      info.oldParent = w->parentWidget();
      info.oldIndex = (info.oldParent && info.oldParent->layout())
                          ? info.oldParent->layout()->indexOf(w)
                          : -1;
      m_widgetsInfo.append(info);
    }
    setText("Group in " + containerType);
  }

  void undo() override {
    if (!m_canvas)
      return;
    for (const auto &info : m_widgetsInfo) {
      if (info.widget.isNull() || info.oldParent.isNull())
        continue;
      info.widget->setParent(info.oldParent.data());
      if (info.oldParent->layout()) {
        if (auto *box = qobject_cast<QBoxLayout *>(info.oldParent->layout())) {
          box->insertWidget(info.oldIndex, info.widget.data());
        } else {
          info.oldParent->layout()->addWidget(info.widget.data());
        }
      }
      info.widget->show();
    }
    if (!m_container.isNull())
      m_canvas->removeWidget(m_container.data());
    if (m_controller)
      m_controller->selectWidget(nullptr);
  }

  void redo() override {
    if (!m_canvas || !m_factory)
      return;
    if (m_container.isNull()) {
      QWidget *created = m_factory->createWidget(
          m_containerType, m_containerType.toLower() + "_group");
      if (!created)
        return;
      m_container = created;
      // Configurar layout padrão se for Frame/GroupBox
      if (m_containerType == "Frame") {
        m_container->setProperty("showbox_type", "frame");
        auto *l = new QHBoxLayout(m_container.data());
        l->setContentsMargins(5, 5, 5, 5);
      } else if (m_containerType == "GroupBox") {
        m_container->setProperty("showbox_type", "groupbox");
        auto *l = new QVBoxLayout(m_container.data());
        l->setContentsMargins(5, 15, 5, 5);
      }
    }

    m_canvas->addWidget(m_container.data());
    for (const auto &info : m_widgetsInfo) {
      if (info.widget.isNull())
        continue;
      if (m_container->layout()) {
        m_container->layout()->addWidget(info.widget.data());
      } else {
        info.widget->setParent(m_container.data());
      }
      info.widget->show();
    }
    if (m_controller)
      m_controller->selectWidget(m_container.data());
  }

private:
  QPointer<Canvas> m_canvas;
  IStudioWidgetFactory *m_factory;
  QPointer<StudioController> m_controller;
  QList<WidgetInfo> m_widgetsInfo;
  QString m_containerType;
  QPointer<QWidget> m_container;
};

class ChangeLayoutCommand : public QUndoCommand {
public:
  ChangeLayoutCommand(QWidget *container, const QString &newType,
                      QUndoCommand *parent = nullptr)
      : QUndoCommand(parent), m_container(container), m_newType(newType) {
    if (qobject_cast<QHBoxLayout *>(container->layout())) {
      m_oldType = "hbox";
    } else {
      m_oldType = "vbox";
    }
    setText("Change Layout to " + newType);
  }

  void undo() override { applyLayout(m_oldType); }

  void redo() override { applyLayout(m_newType); }

private:
  void applyLayout(const QString &type) {
    if (m_container.isNull())
      return;

    // 1. Coletar widgets atuais
    QList<QWidget *> widgets;
    QLayout *oldLayout = m_container->layout();
    if (oldLayout) {
      QLayoutItem *item;
      while ((item = oldLayout->takeAt(0)) != nullptr) {
        if (QWidget *w = item->widget()) {
          widgets.append(w);
        }
        delete item;
      }
      delete oldLayout;
    }

    // 2. Criar novo layout
    QLayout *newLayout = nullptr;
    if (type == "hbox") {
      newLayout = new QHBoxLayout(m_container);
    } else {
      newLayout = new QVBoxLayout(m_container);
    }
    newLayout->setContentsMargins(5, 5, 5, 5);

    // 3. Readicionar widgets
    for (QWidget *w : widgets) {
      newLayout->addWidget(w);
    }
  }

  QPointer<QWidget> m_container;
  QString m_oldType;
  QString m_newType;
};

#endif // STUDIOCOMMANDS_H

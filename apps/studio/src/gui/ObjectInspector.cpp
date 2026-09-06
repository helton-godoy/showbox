#include "ObjectInspector.h"
#include "../core/StudioCommands.h"
#include "../core/StudioController.h"
#include <QDebug>
#include <QDropEvent>
#include <QElapsedTimer>
#include <QMenu>
#include <QTabWidget>

ObjectInspector::ObjectInspector(QWidget *parent) : QTreeWidget(parent) {
  setHeaderLabel("Hierarquia de Objetos");
  setColumnCount(1);

  setDragEnabled(true);
  setAcceptDrops(true);
  setDragDropMode(QAbstractItemView::InternalMove);
  setSelectionMode(QAbstractItemView::ExtendedSelection);

  connect(this, &QTreeWidget::itemSelectionChanged, this,
          &ObjectInspector::onSelectionChanged);
  connect(this, &QTreeWidget::currentItemChanged, this,
          &ObjectInspector::onCurrentItemChanged);
}

void ObjectInspector::onSelectionChanged() {
  if (!m_controller)
    return;

  // Bloquear sinais para não criar loop infinito se o controller emitir sinais
  // de volta
  m_controller->blockSignals(true);

  // Limpar seleção atual no controller (silenciosamente via parâmetro ou
  // limpando antes) Vamos usar uma abordagem: Coletar todos os selecionados e
  // atualizar o controller.

  QList<QTreeWidgetItem *> items = selectedItems();
  QList<QWidget *> widgets;

  for (QTreeWidgetItem *item : items) {
    QWidget *w = item->data(0, Qt::UserRole).value<QWidget *>();
    if (w)
      widgets.append(w);
  }

  // Como o controller não tem um "setSelection(list)", vamos desabilitar os
  // destaques manuais ou adicionar um método setSelection ao controller. Vou
  // usar selectWidget(nullptr) para limpar e depois multiSelect em cada um.
  m_controller->selectWidget(nullptr);
  for (QWidget *w : widgets) {
    m_controller->multiSelectWidget(w);
  }

  m_controller->blockSignals(false);
}

void ObjectInspector::onCurrentItemChanged(QTreeWidgetItem *current,
                                           QTreeWidgetItem *previous) {
  if (current) {
    QWidget *widget = current->data(0, Qt::UserRole).value<QWidget *>();
    emit itemSelected(widget);
  }
}

void ObjectInspector::contextMenuEvent(QContextMenuEvent *event) {
  if (!m_controller)
    return;

  auto selected = m_controller->selectedWidgets();
  if (selected.isEmpty())
    return;

  QMenu menu(this);
  QAction *groupFrame = menu.addAction("Group in Frame (HBox)");
  QAction *groupGroupBox = menu.addAction("Group in GroupBox (VBox)");
  menu.addSeparator();
  QAction *deleteAction = menu.addAction("Delete");

  QAction *res = menu.exec(event->globalPos());

  if (res == groupFrame) {
    emit requestGrouping("Frame");
  } else if (res == groupGroupBox) {
    emit requestGrouping("GroupBox");
  } else if (res == deleteAction) {
    emit requestDelete();
  }
}

void ObjectInspector::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::RightButton) {
    QTreeWidgetItem *item = itemAt(event->pos());
    if (item && item->isSelected()) {
      // Preserva seleção múltipla para o menu de contexto
      return;
    }
  }
  QTreeWidget::mousePressEvent(event);
}

void ObjectInspector::dropEvent(QDropEvent *event) {
  QTreeWidgetItem *item = currentItem();
  if (!item) {
    QTreeWidget::dropEvent(event);
    return;
  }

  QWidget *widget = item->data(0, Qt::UserRole).value<QWidget *>();

  // Executar o drop visual do Qt
  QTreeWidget::dropEvent(event);

  // Identificar o novo pai na árvore
  QTreeWidgetItem *newParentItem = item->parent();
  QWidget *newParentWidget = nullptr;

  if (newParentItem) {
    newParentWidget = newParentItem->data(0, Qt::UserRole).value<QWidget *>();
  }

  if (widget && newParentWidget) {
    // --- VALIDAÇÃO DE CONTAINER ---
    QString type = newParentWidget->property("showbox_type").toString();
    bool isContainer = (type == "window" || type == "groupbox" ||
                        type == "frame" || type == "page");

    // Se soltou sobre algo que NÃO é container, movemos para o pai desse algo
    // (sibling)
    if (!isContainer) {
      newParentWidget = newParentWidget->parentWidget();
    }

    int index = -1;
    if (newParentItem) {
      index = newParentItem->indexOfChild(item);
    } else {
      index = invisibleRootItem()->indexOfChild(item);
    }

    if (m_controller) {
      m_controller->undoStack()->push(
          new MoveWidgetCommand(widget, newParentWidget, index));
    } else {
      widget->setParent(newParentWidget);
      if (newParentWidget && newParentWidget->layout()) {
        newParentWidget->layout()->addWidget(widget);
      }
      widget->show();
    }
  }
}

void ObjectInspector::onWidgetAdded(QWidget *widget) {
  if (!widget)
    return;

  QWidget *parentWidget = widget->parentWidget();
  QTreeWidgetItem *parentItem = invisibleRootItem();

  if (parentWidget && m_widgetToItem.contains(parentWidget)) {
    parentItem = m_widgetToItem[parentWidget];
  }

  addWidgetToTree(widget, parentItem);

  // Garantir que o novo item seja visível
  if (parentItem != invisibleRootItem()) {
    parentItem->setExpanded(true);
  }
}

void ObjectInspector::onWidgetRemoved(QWidget *widget) {
  if (!widget || !m_widgetToItem.contains(widget))
    return;

  QTreeWidgetItem *item = m_widgetToItem[widget];

  // Remover recursivamente do mapa para evitar dangling pointers
  QList<QWidget *> toRemove;
  QMapIterator<QWidget *, QTreeWidgetItem *> i(m_widgetToItem);
  while (i.hasNext()) {
    i.next();
    // Se o item for o próprio ou descendente (na árvore do Qt)
    if (i.key() == widget || widget->isAncestorOf(i.key())) {
      toRemove.append(i.key());
    }
  }

  for (QWidget *w : toRemove) {
    m_widgetToItem.remove(w);
  }

  delete item;
}

void ObjectInspector::updateHierarchy(QWidget *root) {
  QElapsedTimer timer;
  timer.start();

  clear();
  m_widgetToItem.clear();

  if (!root)
    return;

  addWidgetToTree(root, invisibleRootItem());
  expandAll();

  qDebug() << "[PERF] ObjectInspector::updateHierarchy levou" << timer.elapsed()
           << "ms para" << m_widgetToItem.size() << "widgets.";
}

void ObjectInspector::addWidgetToTree(QWidget *widget,
                                      QTreeWidgetItem *parentItem) {
  if (!widget)
    return;

  QString name = widget->objectName();
  if (name.isEmpty())
    name = widget->metaObject()->className();

  QTreeWidgetItem *item = new QTreeWidgetItem(parentItem);
  item->setText(0, name);
  item->setData(0, Qt::UserRole, QVariant::fromValue(widget));

  m_widgetToItem[widget] = item;

  // Verificar se deve recursar: Apenas containers devem expor seus filhos na
  // árvore. Widgets atômicos (Button, ComboBox) têm filhos internos (Qt
  // implementation details) que devem ser ocultados.
  QVariant typeVar = widget->property("showbox_type");
  bool isContainer =
      true; // Por padrão, assumimos container (útil para o Canvas/Root)

  if (typeVar.isValid()) {
    QString type = typeVar.toString();
    // Lista explícita de containers
    isContainer = (type == "window" || type == "groupbox" || type == "frame" ||
                   type == "tabs" || type == "page");
  }

  if (!isContainer)
    return;

  // Caso Especial: QTabWidget
  if (QTabWidget *tab = qobject_cast<QTabWidget *>(widget)) {
    for (int i = 0; i < tab->count(); ++i) {
      addWidgetToTree(tab->widget(i), item);
    }
  }
  // Caso Padrão: Iterar filhos diretos (filtrando internos do Qt)
  else {
    for (QObject *child : widget->children()) {
      if (QWidget *w = qobject_cast<QWidget *>(child)) {
        // Ignorar widgets internos do Qt (como layouts ou barras)
        if (!w->objectName().startsWith("qt_")) {
          addWidgetToTree(w, item);
        }
      }
    }
  }
}

void ObjectInspector::selectItemForWidget(QWidget *widget) {
  if (m_widgetToItem.contains(widget)) {
    blockSignals(true);
    setCurrentItem(m_widgetToItem[widget]);
    blockSignals(false);
  }
}

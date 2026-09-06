#include "Canvas.h"
#include "../core/IStudioWidgetFactory.h"
#include "../core/StudioCommands.h"
#include "../core/StudioController.h"
#include <QDebug>
#include <QListWidget>
#include <QMenu>
#include <QMimeData>
#include <QStyle>
#include <QTabWidget>
#include <QTreeWidget>

Canvas::Canvas(IStudioWidgetFactory *factory, QWidget *parent)
    : QWidget(parent), m_factory(factory) {
  m_layout = new QVBoxLayout(this);
  m_layout->setAlignment(Qt::AlignTop);

  // Visual do Canvas (parecido com uma janela de diálogo)
  // O seletor *[selected="true"] garante que a borda de seleção não apague as
  // cores do usuário
  setStyleSheet("Canvas { background-color: #f0f0f0; border: 1px solid #ccc; } "
                "*[selected=\"true\"] { border: 2px solid #0078d7; }");

  setAcceptDrops(true);
}

void Canvas::mousePressEvent(QMouseEvent *event) {
  // Clicar no fundo do canvas limpa a seleção
  if (m_controller) {
    m_controller->selectWidget(nullptr);
  }
  QWidget::mousePressEvent(event);
}

void Canvas::contextMenuEvent(QContextMenuEvent *event) {
  if (!m_controller)
    return;

  QMenu menu(this);
  auto selected = m_controller->selectedWidgets();

  if (!selected.isEmpty()) {
    if (selected.size() == 1) {
      QWidget *w = selected.first();
      QString showboxType = w->property("showbox_type").toString();
      if (showboxType == "tabwidget") {
        menu.addAction("Add Tab Page", [this, w]() { emit requestAddPage(w); });
        menu.addAction("Remove Current Page",
                       [this, w]() { emit requestRemovePage(w); });
        menu.addSeparator();
      }
    }

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
  } else {
    menu.addAction("Canvas context menu (No selection)");
    menu.exec(event->globalPos());
  }
}

void Canvas::addWidget(QWidget *widget) {
  if (!widget)
    return;
  widget->setParent(this);
  m_layout->addWidget(widget);
  widget->show();
}

void Canvas::removeWidget(QWidget *widget) {
  if (!widget)
    return;

  // Remover do layout do Canvas se for filho direto
  if (widget->parentWidget() == this) {
    m_layout->removeWidget(widget);
  } else if (auto *parent = widget->parentWidget()) {
    // Remover de um TabWidget
    if (auto *tabs = qobject_cast<QTabWidget *>(parent)) {
      int idx = tabs->indexOf(widget);
      if (idx >= 0)
        tabs->removeTab(idx);
    }
    // Remover de um layout genérico
    else if (parent->layout()) {
      parent->layout()->removeWidget(widget);
    }
  }

  widget->hide();
  widget->setParent(nullptr);
}

void Canvas::clear() {
  setSelectedWidget(nullptr);
  QLayoutItem *item;
  while ((item = m_layout->takeAt(0)) != nullptr) {
    if (QWidget *w = item->widget()) {
      delete w;
    }
    delete item;
  }
}

void Canvas::setSelectedWidget(QWidget *widget) {
  m_currentSelection = widget;
  // A lógica visual de seleção agora é tratada pelo StudioController
}

void Canvas::dragEnterEvent(QDragEnterEvent *event) {
  // Aceitar drag de QListWidget (Classic) ou QTreeWidget (Tree)
  if (event->mimeData()->hasText() ||
      qobject_cast<QListWidget *>(event->source()) ||
      qobject_cast<QTreeWidget *>(event->source())) {
    event->acceptProposedAction();
  }
}

void Canvas::dragMoveEvent(QDragMoveEvent *event) {
  QString dragType;
  if (event->mimeData()->hasText()) {
    dragType = event->mimeData()->text();
  }

  // Highlight container quando arrasta sobre ele
  QWidget *container = findContainerAtPos(event->position().toPoint());

  bool isPageDrag = (dragType.toLower() == "page");

  // SE estiver arrastando uma Page e o alvo for uma Page existente,
  // tenta encontrar o TabWidget pai para redirecionar o alvo.
  // NOTA: A Page é filha de QStackedWidget (interno do Qt), não diretamente do
  // QTabWidget.
  if (isPageDrag && container) {
    QString cType = container->property("showbox_type").toString();
    if (cType == "page") {
      // Usar qobject_cast para encontrar o QTabWidget ancestral
      QWidget *foundTabs = nullptr;
      QWidget *p = container->parentWidget();
      while (p && p != this) {
        if (qobject_cast<QTabWidget *>(p)) {
          foundTabs = p;
          break;
        }
        p = p->parentWidget();
      }
      // Se encontrou TabWidget, redireciona; caso contrário, invalida o container
      container = foundTabs; // Será nullptr se não encontrou, indicando drop inválido
    }
  }

  // Remover highlight do container anterior se houver
  static QWidget *lastHighlighted = nullptr;
  if (lastHighlighted && lastHighlighted != container) {
    highlightContainer(lastHighlighted, false);
  }

  if (container && container != this) {
    QString containerType = container->property("showbox_type").toString();

    // Se for Page: SÓ aceita se o container final for TabWidget
    if (isPageDrag) {
      if (containerType == "tabs") {
        // Highlight AZUL para indicar que vai adicionar uma nova aba
        highlightContainer(container, true, true);
        lastHighlighted = container;
      } else {
        // Se for Page mas o container não for TabWidget, não destaca (operação
        // inválida)
        highlightContainer(container, false); // Garante que não fique verde
        lastHighlighted = nullptr;
      }
    } else {
      // Comportamento normal para outros widgets (Verde)
      highlightContainer(container, true, false);
      lastHighlighted = container;
    }

  } else {
    lastHighlighted = nullptr;
  }

  event->acceptProposedAction();
}

void Canvas::dropEvent(QDropEvent *event) {
  QString type;
  const QMimeData *mime = event->mimeData();

  if (mime->hasText()) {
    type = mime->text();
  } else if (QListWidget *list = qobject_cast<QListWidget *>(event->source())) {
    // Modo Classic (QToolBox com QListWidget)
    QList<QListWidgetItem *> items = list->selectedItems();
    if (!items.isEmpty()) {
      type = items.first()->text();
    }
  } else if (QTreeWidget *tree = qobject_cast<QTreeWidget *>(event->source())) {
    // Modo Tree (QTreeWidget)
    QList<QTreeWidgetItem *> items = tree->selectedItems();
    if (!items.isEmpty()) {
      QTreeWidgetItem *item = items.first();
      // Apenas itens folha (sem filhos) são widgets
      if (item->childCount() == 0) {
        type = item->text(0);
      }
    }
  }

  if (type.isEmpty())
    return;

  // Gerar um nome único simples
  QString name =
      QString("%1_%2").arg(type.toLower()).arg(m_layout->count() + 1);

  QWidget *newWidget = m_factory->createWidget(type, name);

  if (newWidget) {
    // Detectar se o drop foi sobre um container que aceita filhos
    QWidget *targetContainer = findContainerAtPos(event->position().toPoint());
    qDebug() << "[DROP] Initial targetContainer:" 
             << (targetContainer ? targetContainer->objectName() : "nullptr")
             << "type:" << (targetContainer ? targetContainer->property("showbox_type").toString() : "");

    // Regra específica para Page:
    // Se soltar em cima de uma 'page' existente (conteúdo da aba),
    // redirecionar para o TabWidget pai.
    // NOTA: A Page é filha de QStackedWidget (interno do Qt), não diretamente do
    // QTabWidget. Usar qobject_cast para confiabilidade.
    if (type.toLower() == "page" && targetContainer &&
        targetContainer->property("showbox_type").toString() == "page") {
      QWidget *p = targetContainer->parentWidget();
      while (p && p != this) {
        if (qobject_cast<QTabWidget *>(p)) {
          targetContainer = p;
          break;
        }
        p = p->parentWidget();
      }
    }

    // Restrição: Page só pode ser solta em TabWidget
    if (type.toLower() == "page") {
      QString targetType = targetContainer
                               ? targetContainer->property("showbox_type").toString()
                               : "";
      if (targetType != "tabs") {
        // Page fora de TabWidget: cancelar o drop e destruir o widget criado
        delete newWidget;
        return;
      }
    } else {
      // Outros widgets NÃO podem ser soltos diretamente no TabWidget (devem ir na
      // Page atual)
      if (targetContainer &&
          targetContainer->property("showbox_type").toString() == "tabs") {
        if (auto *tabs = qobject_cast<QTabWidget *>(targetContainer)) {
          qDebug() << "[DROP] Target is TabWidget, currentIndex:" << tabs->currentIndex()
                   << "currentWidget:" << tabs->currentWidget();
          targetContainer = tabs->currentWidget();
        }
      }
      // Se o target é uma Page, verificar se é a aba ativa do TabWidget pai
      // Isso corrige o bug onde childAt() pode retornar uma Page invisível
      else if (targetContainer &&
               targetContainer->property("showbox_type").toString() == "page") {
        qDebug() << "[DROP] Target is Page:" << targetContainer->objectName();
        QWidget *p = targetContainer->parentWidget();
        while (p && p != this) {
          if (auto *tabs = qobject_cast<QTabWidget *>(p)) {
            qDebug() << "[DROP] Found TabWidget, currentIndex:" << tabs->currentIndex()
                     << "currentWidget:" << tabs->currentWidget()
                     << "vs targetContainer:" << targetContainer;
            // Redirecionar para a aba ativa, não a Page que childAt encontrou
            targetContainer = tabs->currentWidget();
            qDebug() << "[DROP] Redirected to:" << targetContainer->objectName();
            break;
          }
          p = p->parentWidget();
        }
      }
    }

    if (!targetContainer)
      targetContainer = this;

    if (m_controller) {
      m_controller->undoStack()->push(
          new AddWidgetCommand(this, newWidget, targetContainer));
    } else {
      // Fallback sem Undo
      if (targetContainer == this) {
        addWidget(newWidget);
      } else {
        newWidget->setParent(targetContainer);
        if (auto *tabs = qobject_cast<QTabWidget *>(targetContainer)) {
          tabs->addTab(newWidget, type);
        } else if (targetContainer->layout()) {
          targetContainer->layout()->addWidget(newWidget);
        }
        newWidget->show();
      }
    }

    emit widgetAdded(newWidget);
    event->acceptProposedAction();
  }
}

QWidget *Canvas::findContainerAtPos(const QPoint &pos) {
  // Usar childAt para encontrar o widget mais profundo
  QWidget *child = childAt(pos);

  // Subir na hierarquia até encontrar um container válido ou chegar no Canvas
  while (child && child != this) {
    QString showboxType = child->property("showbox_type").toString();

    if (showboxType.contains("layout") || showboxType == "groupbox" ||
        showboxType == "frame" || showboxType == "scrollarea" ||
        showboxType == "tabs" || showboxType == "page") {
      return child;
    }
    child = child->parentWidget();
  }

  return nullptr;
}

void Canvas::highlightContainer(QWidget *container, bool highlight, bool blue) {
  if (!container)
    return;

  if (highlight) {
    container->setProperty("drop_target", true);
    QString color = blue ? "#00aaff" : "#00ff00";
    container->setStyleSheet(
        QString("QWidget[drop_target=\"true\"] { border: 2px solid %1; }").arg(color));
  } else {
    container->setProperty("drop_target", false);
    container->setStyleSheet(""); // Limpar estilo dinâmico
  }
  container->style()->polish(container);
}

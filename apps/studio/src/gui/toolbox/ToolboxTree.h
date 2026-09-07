#ifndef TOOLBOXTREE_H
#define TOOLBOXTREE_H

#include "AbstractToolbox.h"
#include <QHeaderView>
#include <QTreeWidget>
#include <QVBoxLayout>

/**
 * @brief Toolbox estilo árvore usando QTreeWidget
 *
 * Permite múltiplas categorias expandidas simultaneamente.
 * Cada categoria é um nó pai que pode ser expandido/colapsado
 * independentemente.
 */
class ToolboxTree : public AbstractToolbox {
  Q_OBJECT

public:
  explicit ToolboxTree(QWidget *parent = nullptr) : AbstractToolbox(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_tree = new QTreeWidget(this);
    m_tree->setHeaderHidden(true);
    m_tree->setDragEnabled(true);
    m_tree->setDragDropMode(QAbstractItemView::DragOnly);
    m_tree->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tree->setIndentation(16);
    m_tree->setAnimated(true);
    m_tree->setRootIsDecorated(true);
    m_tree->setExpandsOnDoubleClick(true);

    // Estilização limpa sem dependência de ícones externos
    m_tree->setStyleSheet(R"(
            QTreeWidget {
                background-color: #2d2d2d;
                border: none;
            }
            QTreeWidget::item {
                padding: 4px 2px;
                color: #e0e0e0;
            }
            QTreeWidget::item:has-children {
                font-weight: bold;
                color: #ffffff;
                background-color: #3a3a3a;
                border-bottom: 1px solid #444;
            }
            QTreeWidget::item:!has-children {
                padding-left: 12px;
            }
            QTreeWidget::item:!has-children:hover {
                background-color: #404040;
            }
            QTreeWidget::item:!has-children:selected {
                background-color: #0078d7;
                color: white;
            }
            QTreeWidget::branch {
                background-color: #2d2d2d;
            }
            QTreeWidget::branch:has-children:closed {
                border-image: none;
                image: none;
            }
            QTreeWidget::branch:has-children:open {
                border-image: none;
                image: none;
            }
        )");

    // Emitir sinal quando item folha é clicado
    connect(m_tree, &QTreeWidget::itemClicked, this,
            [this](QTreeWidgetItem *item, int) {
              // Só emite para itens folha (widgets), não categorias
              if (item->childCount() == 0) {
                emit widgetSelected(item->text(0));
              }
            });

    layout->addWidget(m_tree);
  }

  void addCategory(const QString &categoryName,
                   const QStringList &items) override {
    QTreeWidgetItem *category = new QTreeWidgetItem(m_tree);
    category->setText(0, categoryName);
    category->setFlags(category->flags() &
                       ~Qt::ItemIsDragEnabled); // Categoria não arrastável
    category->setExpanded(true);                // Expandido por padrão

    for (const QString &item : items) {
      QTreeWidgetItem *child = new QTreeWidgetItem(category);
      child->setText(0, item);
      child->setFlags(child->flags() | Qt::ItemIsDragEnabled);
    }
  }

  QString styleName() const override { return "Tree (Expandable)"; }

  /**
   * @brief Expande todas as categorias
   */
  void expandAll() { m_tree->expandAll(); }

  /**
   * @brief Colapsa todas as categorias
   */
  void collapseAll() { m_tree->collapseAll(); }

private:
  QTreeWidget *m_tree;
};

#endif // TOOLBOXTREE_H

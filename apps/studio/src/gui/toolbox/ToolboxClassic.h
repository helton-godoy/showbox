#ifndef TOOLBOXCLASSIC_H
#define TOOLBOXCLASSIC_H

#include "AbstractToolbox.h"
#include <QListWidget>
#include <QToolBox>
#include <QVBoxLayout>

/**
 * @brief Toolbox estilo clássico usando QToolBox
 *
 * Mostra apenas uma categoria por vez (comportamento acordeão).
 * Similar ao Qt Designer.
 */
class ToolboxClassic : public AbstractToolbox {
  Q_OBJECT

public:
  explicit ToolboxClassic(QWidget *parent = nullptr) : AbstractToolbox(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_toolBox = new QToolBox(this);
    layout->addWidget(m_toolBox);
  }

  void addCategory(const QString &categoryName,
                   const QStringList &items) override {
    QListWidget *list = new QListWidget();
    list->setDragEnabled(true);
    list->setDragDropMode(QAbstractItemView::DragOnly);
    list->addItems(items);

    // Emitir sinal quando item é clicado
    connect(
        list, &QListWidget::itemClicked, this,
        [this](QListWidgetItem *item) { emit widgetSelected(item->text()); });

    m_toolBox->addItem(list, categoryName);
  }

  void markItemExperimental(const QString &displayName,
                            const QString &tooltip) override {
    for (QListWidget *list : findChildren<QListWidget *>()) {
      for (QListWidgetItem *item : list->findItems(
               displayName, Qt::MatchExactly)) {
        // Sem Enabled/Selectable: não clica, não seleciona e não arrasta
        // (o modo DragOnly só arrasta selecionados).
        item->setFlags(item->flags() & ~Qt::ItemIsEnabled &
                       ~Qt::ItemIsSelectable);
        item->setToolTip(tooltip);
      }
    }
  }

  QString styleName() const override { return "Classic (QToolBox)"; }

private:
  QToolBox *m_toolBox;
};

#endif // TOOLBOXCLASSIC_H

#ifndef OBJECTINSPECTOR_H
#define OBJECTINSPECTOR_H

#include <QTreeWidget>

class StudioController;

class ObjectInspector : public QTreeWidget {
  Q_OBJECT
public:
  explicit ObjectInspector(QWidget *parent = nullptr);

  void setController(StudioController *controller) {
    m_controller = controller;
  }

  // Atualiza a árvore baseada no widget raiz do canvas
  void updateHierarchy(QWidget *root);

  // Métodos incrementais para evitar reconstrução total
  void onWidgetAdded(QWidget *widget);
  void onWidgetRemoved(QWidget *widget);

  // Sincroniza a seleção quando clicado no canvas
  void selectItemForWidget(QWidget *widget);

signals:
  void itemSelected(QWidget *widget);
  void requestGrouping(const QString &containerType);
  void requestDelete();

protected:
  void dropEvent(QDropEvent *event) override;
  void contextMenuEvent(QContextMenuEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;

private slots:
  void onSelectionChanged();
  void onCurrentItemChanged(QTreeWidgetItem *current,
                            QTreeWidgetItem *previous);

private:
  void addWidgetToTree(QWidget *widget, QTreeWidgetItem *parentItem);
  QMap<QWidget *, QTreeWidgetItem *> m_widgetToItem;
  StudioController *m_controller = nullptr;
};

#endif // OBJECTINSPECTOR_H

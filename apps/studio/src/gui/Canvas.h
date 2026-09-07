#ifndef CANVAS_H
#define CANVAS_H

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QVBoxLayout>
#include <QWidget>

class IStudioWidgetFactory;
class StudioController;

class Canvas : public QWidget {
  Q_OBJECT
public:
  explicit Canvas(IStudioWidgetFactory *factory, QWidget *parent = nullptr);

  void setController(StudioController *controller) {
    m_controller = controller;
  }

  void addWidget(QWidget *widget);
  void removeWidget(QWidget *widget);
  void setSelectedWidget(QWidget *widget);
  void clear();

signals:
  void widgetAdded(QWidget *widget);
  void widgetSelected(QWidget *widget);
  void requestGrouping(const QString &containerType);
  void requestDelete();
  void requestAddPage(QWidget *tabWidget);
  void requestRemovePage(QWidget *tabWidget);

protected:
  void dragEnterEvent(QDragEnterEvent *event) override;
  void dragMoveEvent(QDragMoveEvent *event) override;
  void dropEvent(QDropEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void contextMenuEvent(QContextMenuEvent *event) override;

private:
  QVBoxLayout *m_layout;
  IStudioWidgetFactory *m_factory;
  StudioController *m_controller = nullptr;
  QWidget *m_currentSelection = nullptr;

  // Suporte a drop em containers
  QWidget *findContainerAtPos(const QPoint &pos);
  void highlightContainer(QWidget *container, bool highlight, bool blue = false);
};

#endif // CANVAS_H

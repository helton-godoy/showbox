#ifndef STUDIOCONTROLLER_H
#define STUDIOCONTROLLER_H

#include <QMouseEvent>
#include <QObject>
#include <QUndoStack>
#include <QWidget>

class StudioController : public QObject {
  Q_OBJECT
public:
  explicit StudioController(QObject *parent = nullptr);

  // Adiciona um widget ao controle do editor
  void manageWidget(QWidget *widget);
  void selectWidget(QWidget *widget);
  void multiSelectWidget(QWidget *widget);

  QWidget *selectedWidget() const {
    return m_selectedWidgets.isEmpty() ? nullptr : m_selectedWidgets.first();
  }
  QList<QWidget *> selectedWidgets() const { return m_selectedWidgets; }
  QUndoStack *undoStack() const { return m_undoStack; }

signals:
  void widgetSelected(QWidget *widget);
  void selectionChanged();

protected:
  bool eventFilter(QObject *watched, QEvent *event) override;

private:
  QList<QWidget *> m_selectedWidgets;
  QUndoStack *m_undoStack = nullptr;
};

#endif // STUDIOCONTROLLER_H

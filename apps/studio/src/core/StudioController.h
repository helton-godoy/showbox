#ifndef STUDIOCONTROLLER_H
#define STUDIOCONTROLLER_H

#include <QMouseEvent>
#include <QObject>
#include <QPointer>
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
    return m_selectedWidgets.isEmpty() ? nullptr
                                       : m_selectedWidgets.first().data();
  }
  QList<QWidget *> selectedWidgets() const {
    QList<QWidget *> widgets;
    for (const auto &guarded : m_selectedWidgets) {
      if (!guarded.isNull())
        widgets.append(guarded.data());
    }
    return widgets;
  }
  QUndoStack *undoStack() const { return m_undoStack; }

 signals:
  void widgetSelected(QWidget *widget);
  void selectionChanged();

protected:
  bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
  void pruneSelection(QObject *destroyed);

private:
  void trackWidget(QWidget *widget);

  // QPointer: entradas anulam sozinhas quando o widget é deletado, evitando
  // seleção pendurada após delete/clear/undo.
  QList<QPointer<QWidget>> m_selectedWidgets;
  QUndoStack *m_undoStack = nullptr;
};

#endif // STUDIOCONTROLLER_H

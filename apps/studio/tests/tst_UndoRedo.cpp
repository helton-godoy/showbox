#include <QtTest>
#include <QUndoStack>
#include "core/StudioController.h"
#include "core/StudioCommands.h"
#include "core/StudioWidgetFactory.h"
#include "gui/Canvas.h"

class tst_UndoRedo : public QObject
{
    Q_OBJECT

private slots:
    void testUndoStackInitialization();
    void testAddDeleteCommands();
};

void tst_UndoRedo::testUndoStackInitialization()
{
    StudioController controller;
    QUndoStack *stack = controller.undoStack();
    QVERIFY(stack != nullptr);
    QCOMPARE(stack->count(), 0);
}

void tst_UndoRedo::testAddDeleteCommands()
{
    StudioWidgetFactory factory;
    Canvas canvas(&factory);
    StudioController controller;
    QUndoStack *stack = controller.undoStack();

    QWidget *btn = factory.createWidget("PushButton", "test_btn");
    
    // 1. Testar Add
    stack->push(new AddWidgetCommand(&canvas, btn));
    QCOMPARE(stack->count(), 1);
    QVERIFY(canvas.children().contains(btn));

    // 2. Testar Undo Add
    stack->undo();
    QVERIFY(!canvas.children().contains(btn));

    // 3. Testar Redo Add
    stack->redo();
    QVERIFY(canvas.children().contains(btn));

    // 4. Testar Delete
    stack->push(new DeleteWidgetCommand(&canvas, {btn}));
    QCOMPARE(stack->count(), 2);
    QVERIFY(!canvas.children().contains(btn));

    // 5. Testar Undo Delete
    stack->undo();
    QVERIFY(canvas.children().contains(btn));
}

QTEST_MAIN(tst_UndoRedo)
#include "tst_UndoRedo.moc"

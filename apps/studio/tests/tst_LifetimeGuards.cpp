#include <QtTest>
#include <QMenuBar>
#include "core/StudioController.h"
#include "core/StudioCommands.h"
#include "core/StudioWidgetFactory.h"
#include "gui/Canvas.h"
#include "gui/MainWindow.h"
#include "gui/ObjectInspector.h"
#include "gui/PropertyEditor.h"

class tst_LifetimeGuards : public QObject
{
    Q_OBJECT

private slots:
    void selectionPrunedAfterDelete();
    void propertyEditorClearsAfterTargetDeleted();
    void inspectorSurvivesWidgetDeath();
    void undoDeleteAfterClearIsNoOp();
    void menuBarFollowsStandardOrder();
};

void tst_LifetimeGuards::selectionPrunedAfterDelete()
{
    StudioWidgetFactory factory;
    Canvas canvas(&factory);
    StudioController controller;
    canvas.setController(&controller);

    QWidget *btn = factory.createWidget("pushbutton", "guard_btn");
    QVERIFY(btn);
    canvas.addWidget(btn);
    controller.manageWidget(btn);
    controller.selectWidget(btn);
    QCOMPARE(controller.selectedWidget(), btn);

    // Deletar o widget selecionado não pode deixar seleção pendurada.
    delete btn;
    QVERIFY(controller.selectedWidget() == nullptr);
    // Operações seguintes sobre seleção vazia não podem falhar.
    controller.selectWidget(nullptr);
    QVERIFY(controller.selectedWidgets().isEmpty());
}

void tst_LifetimeGuards::propertyEditorClearsAfterTargetDeleted()
{
    StudioWidgetFactory factory;
    PropertyEditor editor;

    QWidget *btn = factory.createWidget("pushbutton", "guard_prop");
    QVERIFY(btn);
    editor.setTargetWidget(btn);
    QVERIFY(editor.rowCount() > 0);

    // Deletar o alvo deve limpar a tabela em vez de pendurar m_target.
    delete btn;
    QCOMPARE(editor.rowCount(), 0);

    editor.setTargetWidget(nullptr);
    QCOMPARE(editor.rowCount(), 0);
}

void tst_LifetimeGuards::inspectorSurvivesWidgetDeath()
{
    StudioWidgetFactory factory;
    Canvas canvas(&factory);
    ObjectInspector inspector;

    QWidget *btn = factory.createWidget("pushbutton", "guard_insp");
    canvas.addWidget(btn);
    inspector.updateHierarchy(&canvas);

    delete btn;

    // Reconstruir a hierarquia e consultar o mapa após a morte do widget.
    inspector.updateHierarchy(&canvas);
    inspector.selectItemForWidget(nullptr);
}

void tst_LifetimeGuards::undoDeleteAfterClearIsNoOp()
{
    StudioWidgetFactory factory;
    Canvas canvas(&factory);
    StudioController controller;
    QUndoStack *stack = controller.undoStack();

    QWidget *btn = factory.createWidget("pushbutton", "guard_undo");
    canvas.addWidget(btn);
    stack->push(new DeleteWidgetCommand(&canvas, {btn}));
    QVERIFY(!canvas.children().contains(btn));

    // Simular New/Open: delete real sem limpar a pilha e depois undo.
    // Com guardas QPointer, o undo vira no-op em vez de use-after-free.
    delete btn;
    stack->undo();
    stack->redo();
}

void tst_LifetimeGuards::menuBarFollowsStandardOrder()
{
    MainWindow window;
    QMenuBar *bar = window.menuBar();
    QVERIFY(bar);
    QStringList titles;
    for (QAction *action : bar->actions())
        titles << action->text();
    QVERIFY(titles.contains("&File"));
    QVERIFY(titles.contains("&Edit"));
    QVERIFY(titles.contains("&View"));
    QVERIFY(titles.contains("&Help"));
    QVERIFY(titles.indexOf("&File") < titles.indexOf("&Edit"));
    QVERIFY(titles.indexOf("&Edit") < titles.indexOf("&View"));
    QVERIFY(titles.indexOf("&View") < titles.indexOf("&Help"));
}

QTEST_MAIN(tst_LifetimeGuards)
#include "tst_LifetimeGuards.moc"

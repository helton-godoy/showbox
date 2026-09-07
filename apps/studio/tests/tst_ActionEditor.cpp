#include <QtTest>
#include <QComboBox>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLineEdit>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QPushButton>
#include "gui/ActionEditor.h"
#include "gui/Canvas.h"
#include "gui/MainWindow.h"
#include "core/StudioCommands.h"
#include "core/StudioController.h"
#include "core/StudioWidgetFactory.h"

class ActionEditorTest : public QObject {
    Q_OBJECT
private slots:
    void selectionDoesNotRewriteActions() {
        QPushButton button;
        button.setProperty("showbox_type", "button");
        const QString json = QString::fromUtf8(QJsonDocument(QJsonObject{{"clicked", QJsonArray{
            QJsonObject{{"type", "set"}, {"target", "result"}, {"property", "text"}, {"value", "Inicial"}},
            QJsonObject{{"type", "shell"}, {"command", "showbox_get VALUE entry\nshowbox_set result text \"$VALUE\""}}
        }}}).toJson());
        button.setProperty("showbox_actions", json);
        ActionEditor editor;
        QSignalSpy changed(&editor, &ActionEditor::actionsChanged);
        editor.setTargetWidget(&button);
        auto *list = editor.findChild<QListWidget *>("actionList");
        list->setCurrentRow(0);
        QCOMPARE(changed.size(), 0);
        QCOMPARE(button.property("showbox_actions").toString(), json);
        editor.findChild<QLineEdit *>("actionValue")->setText("Novo 'valor' = $literal");
        auto object = QJsonDocument::fromJson(editor.getActionsJson().toUtf8()).object();
        QCOMPARE(object["clicked"].toArray()[0].toObject()["value"].toString(), "Novo 'valor' = $literal");
        const QString before = editor.getActionsJson();
        list->setCurrentRow(1);
        QCOMPARE(editor.getActionsJson(), before);
        auto *code = editor.findChild<QPlainTextEdit *>("actionCommand");
        QVERIFY(code->toPlainText().contains('\n'));
        code->setPlainText("printf 'a\\n'\nprintf 'b\\n'");
        object = QJsonDocument::fromJson(editor.getActionsJson().toUtf8()).object();
        QCOMPARE(object["clicked"].toArray()[1].toObject()["command"].toString(), code->toPlainText());
    }
    void demoIsAvailableInStudio() {
        MainWindow window;
        QVERIFY(QMetaObject::invokeMethod(&window, "onDemoClicked"));
        auto *editor = window.findChild<ActionEditor *>();
        QVERIFY(editor);
        QVERIFY(editor->getActionsJson().contains("showbox_get VALUE entry"));
        QVERIFY(window.findChild<QPushButton *>("run"));
    }
    void modifiedTrackingAcrossSaveAndEdit() {
        MainWindow window;
        auto *canvas = window.findChild<Canvas *>();
        auto *controller = window.findChild<StudioController *>();
        QVERIFY(canvas);
        QVERIFY(controller);

        QVERIFY(!window.hasUnsavedChanges());

        StudioWidgetFactory factory;
        QWidget *button = factory.createWidget("pushbutton", "run");
        QVERIFY(button);
        controller->undoStack()->push(new AddWidgetCommand(canvas, button, canvas));
        QVERIFY(window.hasUnsavedChanges());

        window.markDocumentSaved();
        QVERIFY(!window.hasUnsavedChanges());

        QWidget *label = factory.createWidget("label", "lbl");
        QVERIFY(label);
        controller->undoStack()->push(new AddWidgetCommand(canvas, label, canvas));
        QVERIFY(window.hasUnsavedChanges());

        controller->undoStack()->undo();
        QVERIFY(!window.hasUnsavedChanges());
    }
};
QTEST_MAIN(ActionEditorTest)
#include "tst_ActionEditor.moc"

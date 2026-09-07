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
#include "gui/MainWindow.h"

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
};
QTEST_MAIN(ActionEditorTest)
#include "tst_ActionEditor.moc"

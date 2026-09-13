#include <QtTest>
#include <QJsonArray>
#include <QMap>
#include <QSignalSpy>
#include <QTabWidget>
#include <QTemporaryDir>

#include "automation/AutomationProtocol.h"
#include "automation/AutomationDescriptors.h"
#include "gui/MainWindow.h"

class tst_StudioAutomation : public QObject {
    Q_OBJECT

private slots:
    void protocolResponsesHaveStableShape();
    void schemasAreCentralAndStrict();
    void facadeUsesVersionedModel();
    void facadeMutationsUseStudioServices();
    void textboxPropertyMutationUpdatesSnapshot();
    void compoundPropertiesAndTypesRoundTrip();
    void compoundUndoIsAtomic();
    void widgetAddRejectsInvalidNames();
    void actionSchemasRequireConditionalFields();
    void mcpDoesNotPublishEventsSubscribe();
    void atomicParentsRejectChildren();
    void rejectedMutationLeavesNoRedo();
    void incrementalFixOnInvalidProject();
    void actionUndoRestoresDirty();
    void enumsAreStrict();
    void tabMoveUndoRestoresIndexAndTitle();
    void pageTitleUpdatesVisibleTabText();
    void dependentPropertiesRestoreFully();
    void undoRedoEmitSingleEvent();
    void duplicateDiagnosticsCountAsNew();
    void projectEventsHaveSingleSource();
    void setPropertySchemaIsConditional();
    void guiStackEditsPublishProjectChanged();
    void documentTransactionsAreObservable();
    void rejectsUnknownAndInternalProperties();
    void dirtyProjectRequiresForce();
};

void tst_StudioAutomation::protocolResponsesHaveStableShape() {
    const QJsonObject response = showbox::automation::makeError(
        7, -32020, "error", "studio", "falhou",
        QJsonObject{{"method", "widget.add"}}, {}, "tente novamente");
    QCOMPARE(response.value("jsonrpc").toString(), QString("2.0"));
    QCOMPARE(response.value("id").toInt(), 7);
    const QJsonObject error = response.value("error").toObject();
    QVERIFY(error.contains("code"));
    QVERIFY(error.contains("message"));
    const QJsonObject data = error.value("data").toObject();
    for (const QString &key : {QString("severity"), QString("component"),
                               QString("context"), QString("location"),
                               QString("suggestion")})
        QVERIFY2(data.contains(key), qPrintable(key));
    // Erros sem request identificável usam id null explícito, nunca ausente.
    const QJsonObject nullId = showbox::automation::makeError(
        QJsonValue(QJsonValue::Null), -32700, "error", "automation",
        "JSON-RPC inválido.");
    QVERIFY(nullId.contains("id"));
    QVERIFY(nullId.value("id").isNull());
}

void tst_StudioAutomation::schemasAreCentralAndStrict() {
    const auto &descriptors = showbox::automation::methodDescriptors();
    const QJsonArray mcpTools = showbox::automation::mcpToolJson();
    // MCP publica todos os métodos exceto events.subscribe, que exige
    // conexão persistente e não pode ser sustentado por tools/call.
    QCOMPARE(mcpTools.size(), descriptors.size() - 1);
    int mcpIndex = 0;
    for (int i = 0; i < descriptors.size(); ++i) {
        const QJsonObject schema = descriptors.at(i).inputSchema;
        if (descriptors.at(i).name == "events.subscribe")
            continue;
        QVERIFY(!schema.value("additionalProperties").toBool(true));
        QCOMPARE(mcpTools.at(mcpIndex).toObject().value("name").toString(),
                 descriptors.at(i).name);
        QCOMPARE(mcpTools.at(mcpIndex).toObject().value("inputSchema").toObject(),
                 schema);
        ++mcpIndex;
    }

    const auto *events = showbox::automation::methodDescriptor("events.subscribe");
    QVERIFY(events);
    QString error;
    QVERIFY(!showbox::automation::validateParams(
        *events, QJsonObject{{"events", QJsonArray{42}}}, &error));
    QVERIFY(error.contains("params.events"));

    const auto *property = showbox::automation::methodDescriptor("widget.setProperty");
    QVERIFY(property);
    QVERIFY(!showbox::automation::validateParams(
        *property, QJsonObject{{"name", "x"}, {"property", "text"},
                               {"value", QJsonObject{{"internal", true}}}},
        &error));
}

void tst_StudioAutomation::facadeUsesVersionedModel() {
    MainWindow window;
    const QJsonObject snapshot = window.automationProjectSnapshot();
    QCOMPARE(snapshot.value("format").toString(), QString("showbox"));
    QCOMPARE(snapshot.value("version").toInt(), 2);
    QVERIFY(snapshot.value("widgets").isArray());
    QVERIFY(window.automationUiTree().value("nodes").isArray());
}

void tst_StudioAutomation::facadeMutationsUseStudioServices() {
    MainWindow window;
    QString error;
    QVERIFY2(window.automationAddWidget("label", "automation_label", {},
                                       &error),
             qPrintable(error));
    QVERIFY2(window.automationSetProperty("automation_label", "text",
                                          "Automated", &error),
             qPrintable(error));
    QCOMPARE(window.automationProjectSnapshot().value("widgets").toArray().size(),
             2);
    QCOMPARE(window.automationDiagnostics().size(), 0);
    QVERIFY2(window.automationSelectWidget("automation_label", &error),
             qPrintable(error));
    QCOMPARE(window.automationProjectSnapshot().value("selected").toString(),
             QString("automation_label"));
    QVERIFY2(window.automationUndo(&error), qPrintable(error));
    QVERIFY2(window.automationRedo(&error), qPrintable(error));
}

void tst_StudioAutomation::textboxPropertyMutationUpdatesSnapshot() {
    MainWindow window;
    QString error;
    QVERIFY2(window.automationNew(&error), qPrintable(error));
    QVERIFY2(window.automationAddWidget("textbox", "entry", {}, &error),
             qPrintable(error));
    QVERIFY2(window.automationSetProperty("entry", "text", "valor", &error),
             qPrintable(error));
    const QJsonArray widgets =
        window.automationProjectSnapshot().value("widgets").toArray();
    QCOMPARE(widgets.size(), 1);
    QCOMPARE(widgets.first().toObject().value("properties").toObject()
                 .value("text")
                 .toString(),
             QString("valor"));
}

namespace {
QJsonObject automationNode(const QJsonArray &nodes, const QString &name) {
    for (const QJsonValue &value : nodes) {
        const QJsonObject node = value.toObject();
        if (node.value("name").toString() == name)
            return node;
    }
    return {};
}
}

void tst_StudioAutomation::compoundPropertiesAndTypesRoundTrip() {
    MainWindow window;
    QString error;
    QVERIFY(window.automationNew(&error));
    for (const auto &spec : {QPair<QString, QString>("textbox", "entry"),
                             {"combobox", "choice"}, {"listbox", "items"},
                             {"table", "data"}, {"checkbox", "flag"},
                             {"spinbox", "count"}}) {
        QVERIFY2(window.automationAddWidget(spec.first, spec.second, {}, &error),
                 qPrintable(error));
    }
    QVERIFY(window.automationSetProperty("entry", "text", "abc", &error));
    QVERIFY(window.automationSetProperty("entry", "placeholder", "hint", &error));
    QVERIFY(window.automationSetProperty("entry", "readOnly", true, &error));
    QVERIFY(window.automationSetProperty("choice", "items",
                                          QJsonArray{"one", "two", "three"}, &error));
    QVERIFY(window.automationSetProperty("choice", "currentIndex", 2, &error));
    QVERIFY(window.automationSetProperty("items", "items",
                                          QJsonArray{"a", "b"}, &error));
    QVERIFY(window.automationSetProperty("data", "headers",
                                          QJsonArray{"A", "B"}, &error));
    QVERIFY(window.automationSetProperty("data", "rows",
                                          QJsonArray{QJsonArray{"1", "2"},
                                                     QJsonArray{"3", "4"}}, &error));
    QVERIFY(window.automationSetProperty("flag", "checked", true, &error));
    QVERIFY(window.automationSetProperty("count", "value", 7, &error));

    const QJsonArray widgets =
        window.automationProjectSnapshot().value("widgets").toArray();
    QCOMPARE(automationNode(widgets, "entry").value("properties").toObject()
                 .value("text").toString(), QString("abc"));
    QCOMPARE(automationNode(widgets, "entry").value("properties").toObject()
                 .value("readOnly").toBool(), true);
    QCOMPARE(automationNode(widgets, "choice").value("items").toArray().size(), 3);
    QCOMPARE(automationNode(widgets, "choice").value("properties").toObject()
                 .value("currentIndex").toInt(), 2);
    QCOMPARE(automationNode(widgets, "items").value("items").toArray().size(), 2);
    QCOMPARE(automationNode(widgets, "data").value("headers").toArray().size(), 2);
    QCOMPARE(automationNode(widgets, "data").value("rows").toArray().size(), 2);
    QCOMPARE(automationNode(widgets, "flag").value("properties").toObject()
                 .value("checked").toBool(), true);
    QCOMPARE(automationNode(widgets, "count").value("properties").toObject()
                 .value("value").toInt(), 7);

    QVERIFY(window.automationUndo(&error));
    QCOMPARE(automationNode(window.automationProjectSnapshot().value("widgets").toArray(),
                            "count").value("properties").toObject().value("value").toInt(), 0);
    QVERIFY(window.automationRedo(&error));
    QCOMPARE(automationNode(window.automationProjectSnapshot().value("widgets").toArray(),
                            "count").value("properties").toObject().value("value").toInt(), 7);
}

void tst_StudioAutomation::compoundUndoIsAtomic() {
    MainWindow window;
    QString error;
    QVERIFY2(window.automationNew(&error), qPrintable(error));
    QVERIFY2(window.automationAddWidget("table", "data", {}, &error),
             qPrintable(error));
    QVERIFY2(window.automationSetProperty("data", "headers",
                                          QJsonArray{"A", "B", "C"}, &error),
             qPrintable(error));
    QVERIFY2(window.automationSetProperty(
                 "data", "rows",
                 QJsonArray{QJsonArray{"1", "2", "3"},
                            QJsonArray{"4", "5", "6"}},
                 &error),
             qPrintable(error));
    // Reduzir headers trunca a largura, mas o undo deve restaurar os dados.
    QVERIFY2(window.automationSetProperty("data", "headers",
                                          QJsonArray{"A"}, &error),
             qPrintable(error));
    QCOMPARE(automationNode(window.automationProjectSnapshot().value("widgets").toArray(),
                            "data").value("headers").toArray().size(), 1);
    QVERIFY2(window.automationUndo(&error), qPrintable(error));
    QJsonObject restored = automationNode(
        window.automationProjectSnapshot().value("widgets").toArray(), "data");
    QCOMPARE(restored.value("headers").toArray().size(), 3);
    QCOMPARE(restored.value("rows").toArray().size(), 2);
    QCOMPARE(restored.value("rows").toArray().at(0).toArray().size(), 3);
    QCOMPARE(restored.value("rows").toArray().at(0).toArray().at(2).toString(),
             QString("3"));
    QVERIFY2(window.automationRedo(&error), qPrintable(error));
    QCOMPARE(automationNode(window.automationProjectSnapshot().value("widgets").toArray(),
                            "data").value("headers").toArray().size(), 1);

    QVERIFY2(window.automationAddWidget("combobox", "choice", {}, &error),
             qPrintable(error));
    QVERIFY2(window.automationSetProperty("choice", "items",
                                          QJsonArray{"one", "two", "three"},
                                          &error),
             qPrintable(error));
    QVERIFY2(window.automationSetProperty("choice", "currentIndex", 2, &error),
             qPrintable(error));
    QVERIFY2(window.automationSetProperty("choice", "items",
                                          QJsonArray{"solo"}, &error),
             qPrintable(error));
    QVERIFY2(window.automationUndo(&error), qPrintable(error));
    QJsonObject combo = automationNode(
        window.automationProjectSnapshot().value("widgets").toArray(), "choice");
    QCOMPARE(combo.value("items").toArray().size(), 3);
    QCOMPARE(combo.value("properties").toObject().value("currentIndex").toInt(), 2);
}

void tst_StudioAutomation::widgetAddRejectsInvalidNames() {
    MainWindow window;
    QString error;
    QVERIFY2(window.automationNew(&error), qPrintable(error));
    QVERIFY(!window.automationAddWidget("label", "nome inválido", {}, &error));
    QVERIFY(!window.automationAddWidget("label", "main", {}, &error));
    QVERIFY(!window.automationAddWidget("label", "showbox", {}, &error));
    QVERIFY(!window.automationAddWidget("label", "1abc", {}, &error));
    QVERIFY(!window.automationAddWidget("label", "", {}, &error));
    QCOMPARE(window.automationProjectSnapshot().value("widgets").toArray().size(), 0);
    QVERIFY2(window.automationAddWidget("label", "ok_nome", {}, &error),
             qPrintable(error));
    QVERIFY(!window.automationAddWidget("label", "ok_nome", {}, &error));
}

void tst_StudioAutomation::actionSchemasRequireConditionalFields() {
    QString error;
    const auto *add = showbox::automation::methodDescriptor("action.add");
    QVERIFY(add);
    // shell exige command; set exige target/property/value; query exige target/variable.
    QVERIFY(!showbox::automation::validateParams(
        *add, QJsonObject{{"name", "b"}, {"event", "clicked"},
                          {"action", QJsonObject{{"type", "shell"}}}},
        &error));
    QVERIFY(!showbox::automation::validateParams(
        *add, QJsonObject{{"name", "b"}, {"event", "clicked"},
                          {"action", QJsonObject{{"type", "set"},
                                                 {"target", "x"}}}},
        &error));
    QVERIFY(!showbox::automation::validateParams(
        *add, QJsonObject{{"name", "b"}, {"event", "clicked"},
                          {"action", QJsonObject{{"type", "query"},
                                                 {"target", "x"}}}},
        &error));
    QVERIFY(showbox::automation::validateParams(
        *add, QJsonObject{{"name", "b"}, {"event", "clicked"},
                          {"action", QJsonObject{{"type", "shell"},
                                                 {"command", "echo oi"}}}},
        &error));

    MainWindow window;
    QVERIFY2(window.automationNew(&error), qPrintable(error));
    QVERIFY2(window.automationAddWidget("pushbutton", "run", {}, &error),
             qPrintable(error));
    QVERIFY2(window.automationAddWidget("label", "result", {}, &error),
             qPrintable(error));
    // Modelo proposto inválido (destino inexistente) não pode ser gravado.
    QVERIFY(!window.automationSetActions(
        "run", QJsonObject{{"clicked", QJsonArray{QJsonObject{
                      {"type", "set"}, {"target", "nope"}, {"property", "text"},
                      {"value", "x"}}}}}, &error));
    QVERIFY(window.automationDiagnostics().isEmpty());
    QVERIFY(window.automationSetActions(
        "run", QJsonObject{{"clicked", QJsonArray{QJsonObject{
                      {"type", "shell"}, {"command", "echo oi"}}}}}, &error));
}

void tst_StudioAutomation::mcpDoesNotPublishEventsSubscribe() {
    const QJsonArray tools = showbox::automation::mcpToolJson();
    for (const QJsonValue &value : tools)
        QVERIFY(value.toObject().value("name").toString() != "events.subscribe");
}

void tst_StudioAutomation::atomicParentsRejectChildren() {
    MainWindow window;
    QString error;
    QVERIFY2(window.automationNew(&error), qPrintable(error));
    QVERIFY2(window.automationAddWidget("textbox", "entry", {}, &error),
             qPrintable(error));
    QVERIFY2(window.automationAddWidget("combobox", "choice", {}, &error),
             qPrintable(error));
    QVERIFY2(window.automationAddWidget("groupbox", "box", {}, &error),
             qPrintable(error));
    // Tipos atômicos compostos possuem layout interno, mas não são
    // containers no modelo: filhos ali sumiriam do snapshot/save.
    QVERIFY(!window.automationAddWidget("label", "orphan1", "entry", &error));
    QVERIFY(!window.automationAddWidget("label", "orphan2", "choice", &error));
    QVERIFY2(window.automationAddWidget("label", "child", "box", &error),
             qPrintable(error));
    QVERIFY(!window.automationMoveWidget("child", "entry", -1, &error));
    QVERIFY2(window.automationMoveWidget("child", "box", -1, &error),
             qPrintable(error));
    // Snapshot só contém filhos de containers.
    const QJsonArray widgets =
        window.automationProjectSnapshot().value("widgets").toArray();
    bool foundChild = false;
    for (const QJsonValue &value : widgets) {
        const QJsonObject node = value.toObject();
        if (node.value("name").toString() == "box") {
            for (const QJsonValue &child : node.value("children").toArray()) {
                if (child.toObject().value("name").toString() == "child")
                    foundChild = true;
            }
        }
        QVERIFY(node.value("name").toString() != "orphan1");
        QVERIFY(node.value("name").toString() != "orphan2");
    }
    QVERIFY(foundChild);
}

void tst_StudioAutomation::rejectedMutationLeavesNoRedo() {
    MainWindow window;
    QString error;
    QVERIFY2(window.automationNew(&error), qPrintable(error));
    QVERIFY2(window.automationAddWidget("slider", "level", {}, &error),
             qPrintable(error));
    // Enum fora do domínio deve falhar sem empilhar comando recusado.
    QVERIFY(!window.automationSetProperty("level", "orientation", 999, &error));
    QVERIFY(!window.automationRedo(&error));
    const QJsonArray widgets =
        window.automationProjectSnapshot().value("widgets").toArray();
    const int orientation = automationNode(widgets, "level")
                                .value("properties").toObject()
                                .value("orientation").toInt(-99);
    QVERIFY(orientation == 1 || orientation == 2);
    QVERIFY(orientation != 999);
}

void tst_StudioAutomation::incrementalFixOnInvalidProject() {
    MainWindow window;
    QString error;
    QVERIFY2(window.automationNew(&error), qPrintable(error));
    QVERIFY2(window.automationAddWidget("label", "keep", {}, &error),
             qPrintable(error));
    QVERIFY2(window.automationAddWidget("label", "other", {}, &error),
             qPrintable(error));
    // Criar diagnóstico não relacionado fora da API (renomear no QWidget).
    QWidget *other = window.findChild<QWidget *>("other");
    QVERIFY(other);
    other->setObjectName("nome inválido");
    QVERIFY(!window.automationDiagnostics().isEmpty());
    // Correção incremental em outro componente não pode ser bloqueada pelo
    // diagnóstico pré-existente.
    QVERIFY2(window.automationSetProperty("keep", "text", "fixed", &error),
             qPrintable(error));
    const QJsonArray widgets =
        window.automationProjectSnapshot().value("widgets").toArray();
    QCOMPARE(automationNode(widgets, "keep").value("properties").toObject()
                 .value("text").toString(), QString("fixed"));
}

void tst_StudioAutomation::actionUndoRestoresDirty() {
    MainWindow window;
    QString error;
    QVERIFY2(window.automationNew(&error), qPrintable(error));
    QVERIFY2(window.automationAddWidget("pushbutton", "run", {}, &error),
             qPrintable(error));
    QVERIFY2(window.automationAddWidget("label", "result", {}, &error),
             qPrintable(error));
    window.markDocumentSaved();
    QVERIFY(!window.hasUnsavedChanges());
    QVERIFY(window.automationSetActions(
        "run", QJsonObject{{"clicked", QJsonArray{QJsonObject{
                      {"type", "shell"}, {"command", "echo oi"}}}}}, &error));
    QVERIFY(window.hasUnsavedChanges());
    QVERIFY(window.automationProjectSnapshot().value("dirty").toBool());
    QVERIFY2(window.automationUndo(&error), qPrintable(error));
    QVERIFY(!window.hasUnsavedChanges());
    QVERIFY(!window.automationProjectSnapshot().value("dirty").toBool());
    QVERIFY2(window.automationRedo(&error), qPrintable(error));
    QVERIFY(window.hasUnsavedChanges());
}

void tst_StudioAutomation::enumsAreStrict() {
    QString error;
    const auto *descriptor =
        showbox::automation::methodDescriptor("widget.setProperty");
    QVERIFY(descriptor);
    QVERIFY(!showbox::automation::validateParams(
        *descriptor, QJsonObject{{"name", "s"}, {"property", "orientation"},
                                {"value", 999}},
        &error));
    QVERIFY(!showbox::automation::validateParams(
        *descriptor, QJsonObject{{"name", "t"}, {"property", "echoMode"},
                                {"value", 99}},
        &error));
    QVERIFY(showbox::automation::validateParams(
        *descriptor, QJsonObject{{"name", "s"}, {"property", "orientation"},
                                {"value", 2}},
        &error));

    MainWindow window;
    QVERIFY2(window.automationNew(&error), qPrintable(error));
    QVERIFY2(window.automationAddWidget("slider", "level", {}, &error),
             qPrintable(error));
    QVERIFY2(window.automationAddWidget("textbox", "entry", {}, &error),
             qPrintable(error));
    QVERIFY(!window.automationSetProperty("level", "orientation", 999, &error));
    QVERIFY2(window.automationSetProperty("level", "orientation", 2, &error),
             qPrintable(error));
    QVERIFY(!window.automationSetProperty("entry", "echoMode", 99, &error));
    QVERIFY2(window.automationSetProperty("entry", "echoMode", 2, &error),
             qPrintable(error));
}

void tst_StudioAutomation::tabMoveUndoRestoresIndexAndTitle() {
    MainWindow window;
    QString error;
    QVERIFY2(window.automationNew(&error), qPrintable(error));
    QVERIFY2(window.automationAddWidget("tabs", "tabsA", {}, &error),
             qPrintable(error));
    QVERIFY2(window.automationAddWidget("tabs", "tabsB", {}, &error),
             qPrintable(error));
    QVERIFY2(window.automationAddWidget("page", "p1", "tabsA", &error),
             qPrintable(error));
    QVERIFY2(window.automationAddWidget("page", "p2", "tabsA", &error),
             qPrintable(error));
    auto childrenOf = [&](const char *tabs) {
        const QJsonArray widgets =
            window.automationProjectSnapshot().value("widgets").toArray();
        return automationNode(widgets, tabs).value("children").toArray();
    };
    QCOMPARE(childrenOf("tabsA").size(), 3); // página padrão + p1 + p2
    QCOMPARE(childrenOf("tabsA").last().toObject().value("name").toString(),
             QString("p2"));
    QVERIFY2(window.automationMoveWidget("p1", "tabsB", 0, &error),
             qPrintable(error));
    QCOMPARE(childrenOf("tabsB").first().toObject().value("name").toString(),
             QString("p1"));
    QVERIFY2(window.automationUndo(&error), qPrintable(error));
    const QJsonArray back = childrenOf("tabsA");
    QCOMPARE(back.size(), 3);
    QCOMPARE(back.at(1).toObject().value("name").toString(), QString("p1"));
    QVERIFY2(window.automationRedo(&error), qPrintable(error));
    QCOMPARE(childrenOf("tabsB").first().toObject().value("name").toString(),
             QString("p1"));
}

void tst_StudioAutomation::pageTitleUpdatesVisibleTabText() {
    MainWindow window;
    QString error;
    QVERIFY2(window.automationNew(&error), qPrintable(error));
    QVERIFY2(window.automationAddWidget("tabs", "tabs", {}, &error),
             qPrintable(error));
    QVERIFY2(window.automationAddWidget("page", "page", "tabs", &error),
             qPrintable(error));
    QTabWidget *tabs = window.findChild<QTabWidget *>("tabs");
    QVERIFY(tabs);
    QWidget *page = window.findChild<QWidget *>("page");
    QVERIFY(page);
    const int index = tabs->indexOf(page);
    QVERIFY(index >= 0);
    QVERIFY2(window.automationSetProperty("page", "title", "Renamed", &error),
             qPrintable(error));
    QCOMPARE(tabs->tabText(index), QString("Renamed"));
    QVERIFY2(window.automationUndo(&error), qPrintable(error));
    QVERIFY(tabs->tabText(tabs->indexOf(page)) != QString("Renamed"));
    QVERIFY2(window.automationRedo(&error), qPrintable(error));
    QCOMPARE(tabs->tabText(tabs->indexOf(page)), QString("Renamed"));
}

void tst_StudioAutomation::dependentPropertiesRestoreFully() {
    MainWindow window;
    QString error;
    QVERIFY2(window.automationNew(&error), qPrintable(error));
    QVERIFY2(window.automationAddWidget("spinbox", "count", {}, &error),
             qPrintable(error));
    QVERIFY2(window.automationSetProperty("count", "value", 50, &error),
             qPrintable(error));
    // Elevar o mínimo prende value em 90 no Qt; o undo deve restaurar
    // mínimo E valor originais, sem resíduo.
    QVERIFY2(window.automationSetProperty("count", "minimum", 90, &error),
             qPrintable(error));
    QCOMPARE(automationNode(window.automationProjectSnapshot().value("widgets").toArray(),
                            "count").value("properties").toObject()
                 .value("value").toInt(), 90);
    QVERIFY2(window.automationUndo(&error), qPrintable(error));
    QJsonObject restored = automationNode(
        window.automationProjectSnapshot().value("widgets").toArray(), "count");
    QCOMPARE(restored.value("properties").toObject().value("minimum").toInt(), 0);
    QCOMPARE(restored.value("properties").toObject().value("value").toInt(), 50);

    QVERIFY2(window.automationAddWidget("checkbox", "flag", {}, &error),
             qPrintable(error));
    QVERIFY2(window.automationSetProperty("flag", "checked", true, &error),
             qPrintable(error));
    QVERIFY2(window.automationSetProperty("flag", "checkable", false, &error),
             qPrintable(error));
    QVERIFY2(window.automationUndo(&error), qPrintable(error));
    QJsonObject flag = automationNode(
        window.automationProjectSnapshot().value("widgets").toArray(), "flag");
    QCOMPARE(flag.value("properties").toObject().value("checkable").toBool(), true);
    QCOMPARE(flag.value("properties").toObject().value("checked").toBool(), true);
}

void tst_StudioAutomation::undoRedoEmitSingleEvent() {
    MainWindow window;
    QString error;
    QVERIFY2(window.automationNew(&error), qPrintable(error));
    QVERIFY2(window.automationAddWidget("label", "item", {}, &error),
             qPrintable(error));
    QVERIFY2(window.automationSetProperty("item", "text", "v1", &error),
             qPrintable(error));
    QSignalSpy spy(&window, &MainWindow::automationEvent);
    QVERIFY2(window.automationUndo(&error), qPrintable(error));
    int dirty = 0, diagnostics = 0;
    for (const QList<QVariant> &args : spy) {
        if (args.value(0).toString() == "dirty.changed")
            ++dirty;
        if (args.value(0).toString() == "diagnostics.changed")
            ++diagnostics;
    }
    QCOMPARE(dirty, 1);
    QCOMPARE(diagnostics, 1);
    spy.clear();
    QVERIFY2(window.automationRedo(&error), qPrintable(error));
    dirty = diagnostics = 0;
    for (const QList<QVariant> &args : spy) {
        if (args.value(0).toString() == "dirty.changed")
            ++dirty;
        if (args.value(0).toString() == "diagnostics.changed")
            ++diagnostics;
    }
    QCOMPARE(dirty, 1);
    QCOMPARE(diagnostics, 1);
}

void tst_StudioAutomation::duplicateDiagnosticsCountAsNew() {
    MainWindow window;
    QString error;
    QVERIFY2(window.automationNew(&error), qPrintable(error));
    QVERIFY2(window.automationAddWidget("pushbutton", "run", {}, &error),
             qPrintable(error));
    QVERIFY2(window.automationAddWidget("label", "result", {}, &error),
             qPrintable(error));
    // Injetar fora da API duas ocorrências IDÊNTICAS do mesmo diagnóstico.
    QWidget *run = window.findChild<QWidget *>("run");
    QVERIFY(run);
    run->setProperty("showbox_actions",
                     QString(R"({"clicked":[{"type":"shell","command":""},)"
                             R"({"type":"shell","command":""}]})"));
    QCOMPARE(window.automationDiagnostics().size(), 2);
    // Terceira ocorrência idêntica agrava (multiset) e deve ser recusada.
    QVERIFY(!window.automationSetActions(
        "run", QJsonObject{{"clicked", QJsonArray{
                      QJsonObject{{"type", "shell"}, {"command", ""}},
                      QJsonObject{{"type", "shell"}, {"command", ""}},
                      QJsonObject{{"type", "shell"}, {"command", ""}}}}},
        &error));
    QCOMPARE(window.automationDiagnostics().size(), 2);
    QVERIFY(!window.automationRedo(&error));
    // Correção que reduz a contagem é permitida.
    QVERIFY2(window.automationSetActions(
                 "run", QJsonObject{{"clicked", QJsonArray{QJsonObject{
                               {"type", "shell"}, {"command", "echo oi"}}}}},
                 &error),
             qPrintable(error));
    QVERIFY(window.automationDiagnostics().isEmpty());
}

namespace {
int automationEventCount(QSignalSpy *spy, const char *name) {
    int count = 0;
    for (const QList<QVariant> &args : *spy) {
        if (args.value(0).toString() == QString(name))
            ++count;
    }
    return count;
}
}

void tst_StudioAutomation::projectEventsHaveSingleSource() {
    MainWindow window;
    QString error;
    QVERIFY2(window.automationNew(&error), qPrintable(error));
    QVERIFY2(window.automationAddWidget("label", "item", {}, &error),
             qPrintable(error));
    QSignalSpy spy(&window, &MainWindow::automationEvent);
    QVERIFY2(window.automationSetProperty("item", "text", "v1", &error),
             qPrintable(error));
    QCOMPARE(automationEventCount(&spy, "project.changed"), 1);
    spy.clear();
    QVERIFY2(window.automationSelectWidget("item", &error), qPrintable(error));
    QCOMPARE(automationEventCount(&spy, "selection.changed"), 1);
    QCOMPARE(automationEventCount(&spy, "project.changed"), 0);
    spy.clear();
    QVERIFY2(window.automationUndo(&error), qPrintable(error));
    QCOMPARE(automationEventCount(&spy, "project.changed"), 1);
    spy.clear();
    QVERIFY2(window.automationNew(true, &error), qPrintable(error));
    QCOMPARE(automationEventCount(&spy, "project.changed"), 1);
    spy.clear();
    QVERIFY2(window.automationStopPreview(&error), qPrintable(error));
    QCOMPARE(automationEventCount(&spy, "project.changed"), 0);
}

void tst_StudioAutomation::setPropertySchemaIsConditional() {
    const auto *descriptor =
        showbox::automation::methodDescriptor("widget.setProperty");
    QVERIFY(descriptor);
    const QJsonObject schema = descriptor->inputSchema;
    const QJsonArray branches = schema.value("oneOf").toArray();
    QVERIFY(!branches.isEmpty());
    // Cada propriedade mutável aparece em exatamente um branch, com o tipo
    // de valor correspondente.
    QMap<QString, QString> valueTypeFor;
    for (const QJsonValue &branch : branches) {
        const QJsonObject properties =
            branch.toObject().value("properties").toObject();
        const QJsonArray names =
            properties.value("property").toObject().value("enum").toArray();
        const QJsonObject value = properties.value("value").toObject();
        QVERIFY(!names.isEmpty());
        for (const QJsonValue &name : names) {
            QVERIFY2(!valueTypeFor.contains(name.toString()),
                     qPrintable("propriedade em dois branches: " + name.toString()));
            valueTypeFor[name.toString()] = value.value("type").toString();
        }
        if (names.contains("orientation")) {
            QCOMPARE(names.size(), 1);
            QVERIFY(value.value("enum").toArray().contains(1));
            QVERIFY(value.value("enum").toArray().contains(2));
        }
        if (names.contains("echoMode")) {
            QCOMPARE(names.size(), 1);
            QCOMPARE(value.value("minimum").toInt(-1), 0);
            QCOMPARE(value.value("maximum").toInt(-1), 3);
        }
    }
    for (const QString &property :
         showbox::automation::mutableProperties("all"))
        QVERIFY2(valueTypeFor.contains(property), qPrintable(property));
    QCOMPARE(valueTypeFor.value("enabled"), QString("boolean"));
    QCOMPARE(valueTypeFor.value("text"), QString("string"));
    QCOMPARE(valueTypeFor.value("value"), QString("integer"));
    QCOMPARE(valueTypeFor.value("items"), QString("array"));
    QCOMPARE(valueTypeFor.value("rows"), QString("array"));
    // Tipos incompatíveis são recusados no schema (-32602), antes da execução.
    const auto *setProp =
        showbox::automation::methodDescriptor("widget.setProperty");
    QString error;
    const auto rejects = [&](const char *property, const QJsonValue &value) {
        return !showbox::automation::validateParams(
            *setProp, QJsonObject{{"name", "w"},
                                 {"property", property},
                                 {"value", value}},
            &error);
    };
    QVERIFY(rejects("enabled", "yes"));
    QVERIFY(rejects("width", QJsonArray{"1"}));
    QVERIFY(rejects("text", 42));
    QVERIFY(rejects("items", "one"));
    QVERIFY(rejects("rows", QJsonArray{"not-array"}));
    QVERIFY(rejects("width", -1));
    QVERIFY(showbox::automation::validateParams(
        *setProp, QJsonObject{{"name", "w"}, {"property", "text"},
                             {"value", "ok"}},
        &error));
}

void tst_StudioAutomation::guiStackEditsPublishProjectChanged() {
    MainWindow window;
    QString error;
    QVERIFY2(window.automationNew(&error), qPrintable(error));
    QVERIFY2(window.automationAddWidget("label", "item", {}, &error),
             qPrintable(error));
    QVERIFY2(window.automationSelectWidget("item", &error), qPrintable(error));
    QSignalSpy spy(&window, &MainWindow::automationEvent);
    // Edição iniciada pela GUI (slot privado) empilha direto no QUndoStack:
    // o caminho comum publica project.changed com source "gui".
    QVERIFY(QMetaObject::invokeMethod(&window, "onDeleteClicked"));
    QCOMPARE(automationEventCount(&spy, "project.changed"), 1);
    QString source;
    for (const QList<QVariant> &args : spy) {
        if (args.value(0).toString() == "project.changed")
            source =
                args.value(1).toJsonObject().value("source").toString();
    }
    QCOMPARE(source, QString("gui"));
    spy.clear();
    // Undo iniciado pela GUI (QAction de Edit/toolbar, sem facade).
    QAction *undoAction = nullptr;
    for (QAction *action : window.findChildren<QAction *>()) {
        if (action->shortcut() == QKeySequence::Undo) {
            undoAction = action;
            break;
        }
    }
    QVERIFY(undoAction);
    undoAction->trigger();
    QCOMPARE(automationEventCount(&spy, "project.changed"), 1);
    QVERIFY(!window.automationProjectSnapshot().value("widgets").toArray().isEmpty());
}

void tst_StudioAutomation::documentTransactionsAreObservable() {
    MainWindow window;
    QString error;
    QVERIFY2(window.automationNew(&error), qPrintable(error));
    QVERIFY2(window.automationAddWidget("label", "keep", {}, &error),
             qPrintable(error));
    window.markDocumentSaved();
    // Snapshot observado no momento de cada project.changed.
    QList<int> widgetCounts;
    QObject::connect(&window, &MainWindow::automationEvent, &window,
                     [&](const QString &name, const QJsonObject &) {
                         if (name == "project.changed") {
                             widgetCounts.append(
                                 window.automationProjectSnapshot()
                                     .value("widgets").toArray().size());
                         }
                     });
    // Transição pela GUI: um único project.changed, já com o estado final.
    QVERIFY(QMetaObject::invokeMethod(&window, "onNewClicked"));
    QCOMPARE(widgetCounts.size(), 1);
    QCOMPARE(widgetCounts.first(), 0);
    // Pilha vazia: ações nativas de Undo/Redo acompanham (sem bloqueio).
    QAction *undoAction = nullptr;
    QAction *redoAction = nullptr;
    for (QAction *action : window.findChildren<QAction *>()) {
        if (action->shortcut() == QKeySequence::Undo)
            undoAction = action;
        if (action->shortcut() == QKeySequence::Redo)
            redoAction = action;
    }
    QVERIFY(undoAction && redoAction);
    QVERIFY(!undoAction->isEnabled());
    QVERIFY(!redoAction->isEnabled());
    // Save anuncia a transição para limpo.
    QSignalSpy spy(&window, &MainWindow::automationEvent);
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    const QString path = tempDir.filePath("doc.sbxproj");
    QVERIFY2(window.automationAddWidget("label", "saved", {}, &error),
             qPrintable(error));
    QVERIFY(window.hasUnsavedChanges());
    QVERIFY2(window.automationSave(path, &error), qPrintable(error));
    QVERIFY(!window.hasUnsavedChanges());
    int dirtyFalse = 0;
    for (const QList<QVariant> &args : spy) {
        if (args.value(0).toString() == "dirty.changed" &&
            !args.value(1).toJsonObject().value("dirty").toBool())
            ++dirtyFalse;
    }
    QCOMPARE(dirtyFalse, 1);
}

void tst_StudioAutomation::rejectsUnknownAndInternalProperties() {
    MainWindow window;
    QString error;
    QVERIFY(window.automationNew(&error));
    QVERIFY(window.automationAddWidget("label", "label", {}, &error));
    QVERIFY(!window.automationSetProperty("label", "doesNotExist", "x", &error));
    QVERIFY(!window.automationSetProperty("label", "showbox_type", "button", &error));
    QVERIFY(!window.automationSetProperty("label", "objectName", "other", &error));
}

void tst_StudioAutomation::dirtyProjectRequiresForce() {
    MainWindow window;
    QString error;
    QVERIFY(window.automationNew(&error));
    QVERIFY(window.automationAddWidget("label", "keep", {}, &error));
    const QJsonObject before = window.automationProjectSnapshot();
    QVERIFY(!window.automationNew(&error));
    QVERIFY(error.contains("force"));
    QCOMPARE(window.automationProjectSnapshot().value("widgets"),
             before.value("widgets"));
    QVERIFY(window.automationNew(true, &error));
    QCOMPARE(window.automationProjectSnapshot().value("widgets").toArray().size(), 0);
}

QTEST_MAIN(tst_StudioAutomation)
#include "tst_StudioAutomation.moc"

#include <QtTest>
#include <QJsonArray>

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
}

void tst_StudioAutomation::schemasAreCentralAndStrict() {
    const auto &descriptors = showbox::automation::methodDescriptors();
    const QJsonArray mcpTools = showbox::automation::mcpToolJson();
    QCOMPARE(mcpTools.size(), descriptors.size());
    for (int i = 0; i < descriptors.size(); ++i) {
        const QJsonObject schema = descriptors.at(i).inputSchema;
        QVERIFY(!schema.value("additionalProperties").toBool(true));
        QCOMPARE(mcpTools.at(i).toObject().value("name").toString(),
                 descriptors.at(i).name);
        QCOMPARE(mcpTools.at(i).toObject().value("inputSchema").toObject(),
                 schema);
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

#include <QtTest>

#include "automation/AutomationProtocol.h"
#include "gui/MainWindow.h"

class tst_StudioAutomation : public QObject {
    Q_OBJECT

private slots:
    void protocolResponsesHaveStableShape();
    void facadeUsesVersionedModel();
    void facadeMutationsUseStudioServices();
};

void tst_StudioAutomation::protocolResponsesHaveStableShape() {
    const QJsonObject response = showbox::automation::makeError(
        7, -32020, "error", "studio", "falhou",
        QJsonObject{{"method", "widget.add"}}, {}, "tente novamente");
    QCOMPARE(response.value("jsonrpc").toString(), QString("2.0"));
    QCOMPARE(response.value("id").toInt(), 7);
    const QJsonObject error = response.value("error").toObject();
    for (const QString &key : {QString("code"), QString("severity"),
                               QString("component"), QString("message"),
                               QString("context"), QString("location"),
                               QString("suggestion")})
        QVERIFY2(error.contains(key), qPrintable(key));
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

QTEST_MAIN(tst_StudioAutomation)
#include "tst_StudioAutomation.moc"

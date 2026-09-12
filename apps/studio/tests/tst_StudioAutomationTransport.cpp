#include <QtTest>

#include "automation/AutomationClient.h"
#include "automation/AutomationProtocol.h"
#include "automation/StudioAutomationServer.h"
#include "gui/MainWindow.h"

#include <QCoreApplication>
#include <QEventLoop>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLocalSocket>
#include <QProcess>
#include <QRandomGenerator>
#include <QElapsedTimer>

#ifndef STUDIOCTL_PATH
#define STUDIOCTL_PATH "showbox-studioctl"
#endif
#ifndef MCP_PATH
#define MCP_PATH "showbox-studio-mcp"
#endif

class tst_StudioAutomationTransport : public QObject {
    Q_OBJECT

private slots:
    void init();
    void cleanup();
    void framingValidationAndNotifications();
    void eventsAreFilteredAndReconnectWorks();
    void readOnlyServerRejectsMutations();
    void cliAndMcpUseThePublicSocket();

private:
    QJsonObject sendLine(const QByteArray &line);
    QJsonObject readLine();
    void writeJson(const QJsonObject &object);

    MainWindow *m_window = nullptr;
    StudioAutomationServer *m_server = nullptr;
    QLocalSocket *m_socket = nullptr;
    QString m_socketName;
};

void tst_StudioAutomationTransport::init() {
    m_window = new MainWindow;
    m_socketName = QString("showbox-sb017-test-%1-%2")
                       .arg(QCoreApplication::applicationPid())
                       .arg(QRandomGenerator::global()->generate());
    m_server = new StudioAutomationServer(m_window, false, false, m_window);
    QString error;
    if (!m_server->listen(m_socketName, &error)) {
        delete m_server;
        m_server = nullptr;
        delete m_window;
        m_window = nullptr;
        // Skip somente mediante condição ambiental explícita. Em ambientes
        // normais o listen é obrigatório: mascarar regressões como skip
        // deixaria o CI verde diante de colisões ou falhas reais.
        // Para reconhecer um sandbox sem socket local, execute com
        // SHOWBOX_ALLOW_TRANSPORT_SKIP=1.
        if (qEnvironmentVariable("SHOWBOX_ALLOW_TRANSPORT_SKIP") == "1") {
            QSKIP(qPrintable("sockets locais indisponíveis neste ambiente: " + error));
        }
        QFAIL(qPrintable("QLocalServer.listen falhou; regressão de transporte: " + error));
    }
    m_socket = new QLocalSocket(this);
    m_socket->connectToServer(m_socketName);
    QVERIFY2(m_socket->waitForConnected(1000),
             qPrintable(m_socket->errorString()));
}

void tst_StudioAutomationTransport::cleanup() {
    if (m_socket) {
        m_socket->disconnectFromServer();
        m_socket->deleteLater();
        m_socket = nullptr;
    }
    delete m_server;
    m_server = nullptr;
    delete m_window;
    m_window = nullptr;
    QLocalServer::removeServer(m_socketName);
}

void tst_StudioAutomationTransport::writeJson(const QJsonObject &object) {
    const QByteArray line = QJsonDocument(object).toJson(QJsonDocument::Compact) + '\n';
    QVERIFY(m_socket->write(line) == line.size());
    QVERIFY(m_socket->waitForBytesWritten(1000));
}

QJsonObject tst_StudioAutomationTransport::readLine() {
    QElapsedTimer timer;
    timer.start();
    while (!m_socket->canReadLine() && timer.elapsed() < 1000) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 10);
        QTest::qWait(1);
    }
    if (!m_socket->canReadLine()) {
        QTest::qFail("tempo esgotado aguardando resposta JSONL", __FILE__, __LINE__);
        return {};
    }
    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(m_socket->readLine(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        QTest::qFail("resposta JSONL inválida", __FILE__, __LINE__);
        return {};
    }
    return doc.object();
}

QJsonObject tst_StudioAutomationTransport::sendLine(const QByteArray &line) {
    if (m_socket->write(line + '\n') != line.size() + 1 ||
        !m_socket->waitForBytesWritten(1000)) {
        QTest::qFail("não foi possível enviar linha JSONL", __FILE__, __LINE__);
        return {};
    }
    return readLine();
}

void tst_StudioAutomationTransport::framingValidationAndNotifications() {
    const QByteArray request =
        R"({"jsonrpc":"2.0","id":"partial","method":"system.describe"})";
    QVERIFY(m_socket->write(request.left(17)) == 17);
    QVERIFY(m_socket->waitForBytesWritten(1000));
    QCoreApplication::processEvents(QEventLoop::AllEvents, 10);
    QVERIFY(!m_socket->waitForReadyRead(30));
    QVERIFY(m_socket->write(request.mid(17) + '\n') == request.size() - 17 + 1);
    QVERIFY(m_socket->waitForBytesWritten(1000));
    const QJsonObject described = readLine();
    QCOMPARE(described.value("id").toString(), QString("partial"));
    QVERIFY(described.value("result").toObject().value("methods").isArray());

    writeJson(QJsonObject{{"jsonrpc", "2.0"}, {"id", 2},
                          {"method", "system.capabilities"}});
    const QJsonObject capabilities = readLine();
    QCOMPARE(capabilities.value("id").toInt(), 2);
    QCOMPARE(capabilities.value("result").toObject().value("maxMessageBytes").toInt(),
             showbox::automation::MaxMessageBytes);

    const QJsonObject invalid = sendLine("not-json");
    QCOMPARE(invalid.value("error").toObject().value("code").toInt(), -32700);
    QVERIFY(invalid.contains("id"));
    QVERIFY(invalid.value("id").isNull());
    const QJsonObject unknown = sendLine(
        R"({"jsonrpc":"2.0","id":3,"method":"no.such.method"})");
    QCOMPARE(unknown.value("error").toObject().value("code").toInt(), -32601);
    const QJsonObject badParams = sendLine(
        R"({"jsonrpc":"2.0","id":4,"method":"project.open","params":{}})");
    QCOMPARE(badParams.value("error").toObject().value("code").toInt(), -32602);
    QVERIFY(badParams.value("error").toObject().value("data").isObject());

    const QJsonObject previewAuth = sendLine(
        R"({"jsonrpc":"2.0","id":5,"method":"preview.start","params":{}})");
    QCOMPARE(previewAuth.value("error").toObject().value("code").toInt(), -32011);

    writeJson(QJsonObject{{"jsonrpc", "2.0"}, {"method", "system.capabilities"}});
    QVERIFY(!m_socket->waitForReadyRead(150));

    QByteArray tooLarge(showbox::automation::MaxMessageBytes + 1, 'x');
    QVERIFY(m_socket->write(tooLarge + '\n') == tooLarge.size() + 1);
    QVERIFY(m_socket->waitForBytesWritten(1000));
    const QJsonObject oversized = readLine();
    QCOMPARE(oversized.value("error").toObject().value("code").toInt(), -32031);
    QVERIFY(oversized.contains("id"));
    QVERIFY(oversized.value("id").isNull());
    QTRY_VERIFY_WITH_TIMEOUT(m_socket->state() == QLocalSocket::UnconnectedState, 1000);
}

void tst_StudioAutomationTransport::eventsAreFilteredAndReconnectWorks() {
    writeJson(QJsonObject{{"jsonrpc", "2.0"}, {"id", 10},
                          {"method", "events.subscribe"},
                          {"params", QJsonObject{{"events", QJsonArray{"project.changed"}}}}});
    const QJsonObject subscription = readLine();
    QVERIFY(subscription.value("result").toObject().value("subscribed").toBool());

    writeJson(QJsonObject{{"jsonrpc", "2.0"}, {"id", 11},
                          {"method", "widget.add"},
                          {"params", QJsonObject{{"type", "label"},
                                                  {"name", "transport_label"}}}});
    QJsonObject first = readLine();
    QJsonObject event = first;
    if (first.value("method").toString() == "events.event") {
        event = first;
        first = readLine();
    } else {
        event = readLine();
    }
    QCOMPARE(first.value("id").toInt(), 11);
    QCOMPARE(event.value("method").toString(), QString("events.event"));
    QCOMPARE(event.value("params").toObject().value("name").toString(),
             QString("project.changed"));

    m_socket->disconnectFromServer();
    QElapsedTimer timer;
    timer.start();
    while (m_socket->state() != QLocalSocket::UnconnectedState &&
           timer.elapsed() < 1000) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 10);
        QTest::qWait(1);
    }
    QVERIFY(m_socket->state() == QLocalSocket::UnconnectedState);
    m_socket->connectToServer(m_socketName);
    QVERIFY(m_socket->waitForConnected(1000));
    const QJsonObject response = sendLine(
        R"({"jsonrpc":"2.0","id":12,"method":"preview.status"})");
    QCOMPARE(response.value("id").toInt(), 12);
}

void tst_StudioAutomationTransport::readOnlyServerRejectsMutations() {
    MainWindow readOnlyWindow;
    const QString readOnlyName = m_socketName + "-ro";
    StudioAutomationServer readOnlyServer(&readOnlyWindow, true, false);
    QString error;
    QVERIFY2(readOnlyServer.listen(readOnlyName, &error),
             qPrintable("servidor readOnly deveria escutar: " + error));
    QLocalSocket readOnlySocket;
    readOnlySocket.connectToServer(readOnlyName);
    QVERIFY2(readOnlySocket.waitForConnected(1000),
             qPrintable(readOnlySocket.errorString()));
    std::function<QJsonObject(const QJsonObject &)> roCall =
        [&](const QJsonObject &request) -> QJsonObject {
        const QByteArray line =
            QJsonDocument(request).toJson(QJsonDocument::Compact) + '\n';
        if (readOnlySocket.write(line) != line.size())
            return {};
        if (!readOnlySocket.waitForBytesWritten(1000))
            return {};
        QElapsedTimer timer;
        timer.start();
        while (!readOnlySocket.canReadLine() && timer.elapsed() < 1000) {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 10);
            QTest::qWait(1);
        }
        if (!readOnlySocket.canReadLine())
            return {};
        QJsonParseError parseError;
        const QJsonDocument doc =
            QJsonDocument::fromJson(readOnlySocket.readLine(), &parseError);
        if (parseError.error != QJsonParseError::NoError || !doc.isObject())
            return {};
        return doc.object();
    };
    QJsonObject described = roCall(
        QJsonObject{{"jsonrpc", "2.0"}, {"id", 30}, {"method", "system.describe"}});
    QVERIFY(!described.isEmpty());
    QVERIFY(described.value("result").toObject().value("readOnly").toBool());
    QJsonObject rejected = roCall(
        QJsonObject{{"jsonrpc", "2.0"}, {"id", 31}, {"method", "widget.add"},
                    {"params", QJsonObject{{"type", "label"}, {"name", "ro_label"}}}});
    QVERIFY(!rejected.isEmpty());
    QCOMPARE(rejected.value("error").toObject().value("code").toInt(), -32010);
    QJsonObject snapshot = roCall(
        QJsonObject{{"jsonrpc", "2.0"}, {"id", 32}, {"method", "project.snapshot"}});
    QVERIFY(!snapshot.isEmpty());
    QVERIFY(snapshot.value("result").isObject());
    readOnlySocket.disconnectFromServer();
}

void tst_StudioAutomationTransport::cliAndMcpUseThePublicSocket() {
    QProcess cli;
    cli.start(STUDIOCTL_PATH, {"--socket", m_socketName, "system.describe"});
    QElapsedTimer timer;
    timer.start();
    while (cli.state() != QProcess::NotRunning && timer.elapsed() < 3000) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 10);
        QTest::qWait(1);
    }
    QVERIFY(cli.state() == QProcess::NotRunning);
    QCOMPARE(cli.exitCode(), 0);
    const QJsonDocument cliResponse = QJsonDocument::fromJson(cli.readAllStandardOutput());
    QVERIFY(cliResponse.object().value("result").toObject().contains("methods"));

    QProcess missing;
    missing.start(STUDIOCTL_PATH,
                  {"--socket", "showbox-sb017-no-such-endpoint", "system.capabilities"});
    QVERIFY(missing.waitForFinished(3000));
    QCOMPARE(missing.exitCode(), 2);
    QVERIFY(!missing.readAllStandardError().isEmpty());

    QProcess mcp;
    mcp.start(MCP_PATH, {"--socket", m_socketName});
    QVERIFY(mcp.waitForStarted(1000));
    const QByteArray requests =
        R"({"jsonrpc":"2.0","id":20,"method":"initialize","params":{}}
{"jsonrpc":"2.0","id":21,"method":"tools/list","params":{}}
{"jsonrpc":"2.0","id":22,"method":"tools/call","params":{"name":"preview.status","arguments":{}}}
)";
    QVERIFY(mcp.write(requests) == requests.size());
    mcp.closeWriteChannel();
    timer.restart();
    while (mcp.state() != QProcess::NotRunning && timer.elapsed() < 3000) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 10);
        QTest::qWait(1);
    }
    QVERIFY(mcp.state() == QProcess::NotRunning);
    QCOMPARE(mcp.exitCode(), 0);
    const QList<QByteArray> lines = mcp.readAllStandardOutput().split('\n');
    QVERIFY(lines.size() >= 3);
    QVERIFY(QJsonDocument::fromJson(lines.at(1)).object().value("result")
                .toObject().value("tools").toArray().size() > 10);
}

QTEST_MAIN(tst_StudioAutomationTransport)
#include "tst_StudioAutomationTransport.moc"

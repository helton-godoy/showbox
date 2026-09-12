#include "StudioAutomationServer.h"

#include "AutomationProtocol.h"
#include "AutomationDescriptors.h"
#include "Catalog.h"
#include "../gui/MainWindow.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QLocalSocket>

namespace {

QJsonObject findNode(const QJsonArray &nodes, const QString &name) {
    for (const QJsonValue &value : nodes) {
        const QJsonObject node = value.toObject();
        if (node.value("name").toString() == name)
            return node;
        const QJsonObject child = findNode(node.value("children").toArray(), name);
        if (!child.isEmpty())
            return child;
    }
    return {};
}

} // namespace

StudioAutomationServer::StudioAutomationServer(MainWindow *window, bool readOnly,
                                               bool allowExecution,
                                               QObject *parent)
    : QObject(parent), m_window(window), m_readOnly(readOnly),
      m_allowExecution(allowExecution) {
    connect(&m_server, &QLocalServer::newConnection, this,
            &StudioAutomationServer::onNewConnection);
    connect(m_window, &MainWindow::automationEvent, this,
            &StudioAutomationServer::onStudioEvent);
}

StudioAutomationServer::~StudioAutomationServer() {
    m_shuttingDown = true;
    m_server.close();

    // QLocalServer é pai dos sockets aceitos. Desconectamos os sinais antes
    // de limpar os mapas, pois a destruição dos filhos pode emitir
    // disconnected enquanto a subclasse ainda está sendo destruída.
    const auto sockets = m_buffers.keys();
    for (QLocalSocket *socket : sockets) {
        if (!socket)
            continue;
        disconnect(socket, nullptr, this, nullptr);
        socket->abort();
    }
    m_subscriptions.clear();
    m_buffers.clear();
}

bool StudioAutomationServer::listen(const QString &name, QString *error) {
    m_server.setSocketOptions(QLocalServer::UserAccessOption);
    if (!m_server.listen(name)) {
        const QString firstError = m_server.errorString();
        // Só removemos um endpoint obsoleto depois de provar que não há um
        // processo ativo aceitando conexões. Uma instância viva nunca é
        // removida silenciosamente.
        QLocalSocket probe;
        probe.connectToServer(name);
        if (!probe.waitForConnected(200) &&
            (probe.error() == QLocalSocket::ServerNotFoundError ||
             probe.error() == QLocalSocket::ConnectionRefusedError)) {
            QLocalServer::removeServer(name);
            if (m_server.listen(name))
                return true;
        }
        if (error)
            *error = firstError;
        return false;
    }
    return true;
}

void StudioAutomationServer::onNewConnection() {
    while (QLocalSocket *socket = m_server.nextPendingConnection()) {
        m_buffers.insert(socket, {});
        connect(socket, &QLocalSocket::readyRead, this,
                &StudioAutomationServer::onReadyRead);
        connect(socket, &QLocalSocket::disconnected, this,
                &StudioAutomationServer::onDisconnected);
    }
}

void StudioAutomationServer::onReadyRead() {
    auto *socket = qobject_cast<QLocalSocket *>(sender());
    if (!socket)
        return;
    QByteArray &buffer = m_buffers[socket];
    buffer += socket->readAll();
    if (buffer.size() > showbox::automation::MaxBufferBytes) {
        writeResponse(socket, showbox::automation::makeError(
                                  QJsonValue(), -32030, "error", "transport",
                                  "Buffer de entrada excedeu o limite.",
                                  QJsonObject{{"maxBufferBytes", showbox::automation::MaxBufferBytes}},
                                  {}, "Reduza a mensagem e envie uma linha por vez."));
        buffer.clear();
        socket->disconnectFromServer();
        return;
    }
    while (true) {
        const qsizetype newline = buffer.indexOf('\n');
        if (newline < 0)
            break;
        if (newline > showbox::automation::MaxMessageBytes) {
            writeResponse(socket, showbox::automation::makeError(
                                      QJsonValue(), -32031, "error", "transport",
                                      "Mensagem excedeu o limite de tamanho.",
                                      QJsonObject{{"maxMessageBytes", showbox::automation::MaxMessageBytes}},
                                      {}, "Reduza a solicitação JSON."));
            buffer.clear();
            socket->disconnectFromServer();
            return;
        }
        const QByteArray line = buffer.left(newline).trimmed();
        buffer.remove(0, newline + 1);
        if (line.isEmpty())
            continue;
        QJsonParseError parseError;
        const QJsonDocument document =
            QJsonDocument::fromJson(line, &parseError);
        QJsonObject response;
        if (parseError.error != QJsonParseError::NoError ||
            !document.isObject()) {
            response = showbox::automation::makeError(
                QJsonValue(), -32700, "error", "automation",
                "JSON-RPC inválido.", {}, {},
                "Envie um objeto JSON completo em uma única linha.");
        } else {
            response = dispatch(document.object(), socket);
            if (!document.object().contains("id"))
                response = {};
        }
        if (!response.isEmpty())
            writeResponse(socket, response);
    }
}

void StudioAutomationServer::onDisconnected() {
    if (m_shuttingDown)
        return;
    auto *socket = qobject_cast<QLocalSocket *>(sender());
    if (!socket)
        return;
    m_subscriptions.remove(socket);
    m_buffers.remove(socket);
    socket->deleteLater();
}

QJsonObject StudioAutomationServer::dispatch(const QJsonObject &request,
                                              QLocalSocket *socket) {
    const QJsonValue id = request.value("id");
    if (request.value("jsonrpc").toString() != "2.0") {
        return showbox::automation::makeError(
            id, -32600, "error", "automation", "Solicitação JSON-RPC inválida.",
            {}, {}, "Use jsonrpc=\"2.0\".");
    }
    if (request.contains("id") &&
        !(id.isString() || id.isDouble() || id.isNull())) {
        return showbox::automation::makeError(
            id, -32600, "error", "automation", "O id deve ser string ou número.");
    }
    const QString method = request.value("method").toString();
    if (method.isEmpty()) {
        return showbox::automation::makeError(
            id, -32600, "error", "automation", "O método é obrigatório.");
    }
    const auto *descriptor = showbox::automation::methodDescriptor(method);
    if (!descriptor) {
        return showbox::automation::makeError(
            id, -32601, "error", "automation", "Método não encontrado: " + method,
            {}, {}, "Consulte system.describe para a lista de métodos.");
    }
    const QJsonValue paramsValue = request.value("params");
    if (!paramsValue.isUndefined() && !paramsValue.isObject()) {
        return showbox::automation::makeError(
            id, -32602, "error", "automation",
            "Os parâmetros devem ser um objeto JSON.");
    }
    QString paramsError;
    if (!showbox::automation::validateParams(*descriptor, paramsValue.toObject(),
                                             &paramsError)) {
        return showbox::automation::makeError(
            id, -32602, "error", "automation", paramsError, {}, {},
            "Consulte o inputSchema publicado por system.describe.");
    }
    const QJsonObject response =
        dispatchMethod(id, method, paramsValue.toObject(), socket);
    if (id.isUndefined())
        return {};
    if (!id.isUndefined() && !id.isNull() && response.isEmpty())
        return showbox::automation::makeError(
            id, -32603, "error", "automation", "Falha interna ao responder.");
    return response;
}

QJsonObject StudioAutomationServer::dispatchMethod(const QJsonValue &id,
                                                    const QString &method,
                                                    const QJsonObject &params,
                                                    QLocalSocket *socket) {
    using namespace showbox::automation;
    if (isMutation(method) && m_readOnly) {
        return makeError(id, -32010, "error", "automation",
                         "A operação está desabilitada em modo somente leitura.",
                         QJsonObject{{"method", method}}, {},
                         "Inicie o Studio sem --automation-read-only para mutar o projeto.");
    }
    if (method == "system.describe") {
        return makeResult(id, QJsonObject{
            {"name", "Showbox Studio Automation Interface"},
            {"protocol", "json-rpc"},
            {"version", ProtocolVersion},
            {"readOnly", m_readOnly},
            {"allowExecution", m_allowExecution},
            {"methods", methodDescriptorJson()}});
    }
    if (method == "system.capabilities") {
        return makeResult(id, QJsonObject{
            {"protocolVersion", ProtocolVersion},
            {"readOnly", m_readOnly},
            {"allowExecution", m_allowExecution},
            {"localTransport", "QLocalSocket"},
            {"lineDelimited", true},
            {"maxMessageBytes", MaxMessageBytes},
            {"maxBufferBytes", MaxBufferBytes}});
    }
    if (method == "project.snapshot")
        return makeResult(id, m_window->automationProjectSnapshot());
    if (method == "ui.tree")
        return makeResult(id, m_window->automationUiTree());
    if (method == "diagnostics.list")
        return makeResult(id, QJsonObject{{"diagnostics", m_window->automationDiagnostics()}});
    if (method == "export.validate")
        return makeResult(id, m_window->automationExportValidate());
    if (method == "preview.status")
        return makeResult(id, QJsonObject{{"running", m_window->automationPreviewRunning()}});
    if (method == "preview.logs")
        return makeResult(id, QJsonObject{{"running", m_window->automationPreviewRunning()},
                                          {"text", m_window->automationPreviewLogs()}});
    if (method == "events.subscribe") {
        QSet<QString> subscription;
        for (const QJsonValue &event : params.value("events").toArray())
            subscription.insert(event.toString());
        if (subscription.isEmpty())
            subscription.insert("*");
        m_subscriptions[socket] = subscription;
        return makeResult(id, QJsonObject{{"subscribed", true},
                                          {"events", QJsonArray::fromStringList(
                                                         subscription.values())}});
    }

    QString error;
    bool success = false;
    QJsonObject result;
    const bool dirtyBefore = m_window->automationProjectSnapshot().value("dirty").toBool();
    if (method == "project.new") {
        const bool force = params.value("force").toBool(false);
        success = m_window->automationNew(force, &error);
        result = QJsonObject{{"ok", true}, {"discarded", dirtyBefore && force}};
    } else if (method == "project.open") {
        const bool force = params.value("force").toBool(false);
        success = m_window->automationOpen(params.value("path").toString(), force, &error);
        result = QJsonObject{{"ok", true}, {"discarded", dirtyBefore && force}};
    } else if (method == "project.save") {
        success = m_window->automationSave(params.value("path").toString(), &error);
    } else if (method == "widget.add") {
        success = m_window->automationAddWidget(
            params.value("type").toString(), params.value("name").toString(),
            params.value("parent").toString(), &error);
    } else if (method == "widget.remove") {
        success = m_window->automationRemoveWidget(params.value("name").toString(), &error);
    } else if (method == "widget.select") {
        success = m_window->automationSelectWidget(params.value("name").toString(), &error);
    } else if (method == "widget.move") {
        success = m_window->automationMoveWidget(
            params.value("name").toString(), params.value("parent").toString(),
            params.value("index").toInt(-1), &error);
    } else if (method == "widget.setProperty") {
        success = m_window->automationSetProperty(
            params.value("name").toString(), params.value("property").toString(),
            params.value("value"), &error);
    } else if (method == "history.undo") {
        success = m_window->automationUndo(&error);
    } else if (method == "history.redo") {
        success = m_window->automationRedo(&error);
    } else if (method == "preview.start") {
        if (!m_allowExecution) {
            return makeError(id, -32011, "error", "preview",
                             "A execução está desabilitada pela automação.",
                             {}, {}, "Inicie o Studio com --automation-allow-execution.");
        }
        success = m_window->automationStartPreview(&error);
    } else if (method == "preview.stop") {
        success = m_window->automationStopPreview(&error);
    } else if (method == "export.bash") {
        success = m_window->automationExport(params.value("path").toString(),
                                              &result, &error);
    } else if (method == "action.add" || method == "action.update" ||
               method == "action.remove") {
        const QString name = params.value("name").toString();
        const QString event = params.value("event").toString();
        const QJsonObject snapshot = m_window->automationProjectSnapshot();
        QJsonObject node = findNode(snapshot.value("widgets").toArray(), name);
        if (node.isEmpty()) {
            error = "Componente não encontrado: " + name;
        } else if (!showbox::catalog::isValidEvent(node.value("type").toString(),
                                                   event)) {
            error = "Evento não suportado pelo componente: " + event;
        } else {
            QJsonParseError parseError;
            QJsonDocument actions = QJsonDocument::fromJson(
                node.value("actions").toString().toUtf8(), &parseError);
            QJsonObject actionObject = actions.isObject() ? actions.object() : QJsonObject{};
            QJsonArray list = actionObject.value(event).toArray();
            const int index = params.value("index").toInt(-1);
            if (method == "action.add") {
                list.append(params.value("action").toObject());
            } else if (index < 0 || index >= list.size()) {
                error = "Índice de ação inválido.";
            } else if (method == "action.update") {
                list[index] = params.value("action").toObject();
            } else {
                list.removeAt(index);
            }
            if (error.isEmpty()) {
                actionObject[event] = list;
                success = m_window->automationSetActions(name, actionObject, &error);
            }
        }
    }

    if (!success) {
        return makeError(id, -32020, "error", "studio",
                         error.isEmpty() ? "A operação falhou." : error,
                         QJsonObject{{"method", method}}, {},
                         "Consulte diagnostics.list e tente novamente.");
    }
    if (result.isEmpty())
        result = QJsonObject{{"ok", true}};
    notifyChanged(method);
    return makeResult(id, result);
}

void StudioAutomationServer::writeResponse(QLocalSocket *socket,
                                            const QJsonObject &response) {
    if (!socket)
        return;
    socket->write(QJsonDocument(response).toJson(QJsonDocument::Compact) + '\n');
    socket->flush();
}

void StudioAutomationServer::notifyChanged(const QString &method) {
    const QString event = method == "widget.select" ? "selection.changed"
                                                     : "project.changed";
    notifyEvent(event, QJsonObject{{"source", "automation"},
                                   {"method", method}});
}

void StudioAutomationServer::onStudioEvent(const QString &name,
                                            const QJsonObject &data) {
    notifyEvent(name, data);
}

void StudioAutomationServer::notifyEvent(const QString &name,
                                         const QJsonObject &data) {
    const QJsonObject notification{
        {"jsonrpc", "2.0"},
        {"method", "events.event"},
        {"params", QJsonObject{{"name", name}, {"data", data}}}};
    for (auto it = m_subscriptions.cbegin(); it != m_subscriptions.cend(); ++it) {
        if (it.value().contains("*") || it.value().contains(name))
            writeResponse(it.key(), notification);
    }
}

bool StudioAutomationServer::isMutation(const QString &method) const {
    return method == "project.new" || method == "project.open" ||
           method == "project.save" || method.startsWith("widget.") ||
           method.startsWith("action.") || method.startsWith("history.") ||
           method == "preview.start" || method == "preview.stop" ||
           method == "export.bash";
}

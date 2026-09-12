#include "AutomationClient.h"
#include "AutomationDescriptors.h"

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTextStream>
#include <unistd.h>

namespace {

QJsonObject errorResponse(const QJsonValue &id, const QString &message) {
    return QJsonObject{{"jsonrpc", "2.0"}, {"id", id},
                       {"error", QJsonObject{{"code", -32602},
                                              {"message", message}}}};
}

} // namespace

int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addOption({"socket", "Nome/caminho do socket local.", "socket",
                      QString("showbox-studio-%1").arg(static_cast<uint>(::getuid()))});
    parser.process(app);
    AutomationClient client(parser.value("socket"));
    QTextStream in(stdin);
    QTextStream out(stdout);
    while (!in.atEnd()) {
        const QByteArray line = in.readLine().toUtf8();
        QJsonParseError parseError;
        const QJsonDocument request = QJsonDocument::fromJson(line, &parseError);
        if (parseError.error != QJsonParseError::NoError || !request.isObject())
            continue;
        const QJsonObject object = request.object();
        const QJsonValue id = object.value("id");
        const QString method = object.value("method").toString();
        QJsonObject response;
        if (method == "initialize") {
            QJsonObject result;
            result["protocolVersion"] = "2024-11-05";
            result["serverInfo"] = QJsonObject{
                {"name", "showbox-studio-mcp"}, {"version", "1"}};
            result["capabilities"] = QJsonObject{{"tools", QJsonObject{}}};
            response = QJsonObject{{"jsonrpc", "2.0"}, {"id", id},
                                   {"result", result}};
        } else if (method == "notifications/initialized") {
            continue;
        } else if (method == "tools/list") {
            response = QJsonObject{{"jsonrpc", "2.0"}, {"id", id},
                                   {"result", QJsonObject{{"tools", showbox::automation::mcpToolJson()}}}};
        } else if (method == "tools/call") {
            const QJsonObject params = object.value("params").toObject();
            const QString name = params.value("name").toString();
            if (name.isEmpty()) {
                response = errorResponse(id, "params.name é obrigatório.");
            } else if (name == "events.subscribe") {
                response = errorResponse(
                    id, "events.subscribe exige conexão persistente e não está "
                        "disponível via MCP; use o socket JSON-RPC diretamente.");
            } else {
                QString transportError;
                const QJsonObject rpc = client.call(
                    name, params.value("arguments").toObject(), &transportError);
                if (rpc.isEmpty()) {
                    response = errorResponse(id, transportError);
                } else {
                    const QString text = QString::fromUtf8(
                        QJsonDocument(rpc).toJson(QJsonDocument::Compact));
                    response = QJsonObject{
                        {"jsonrpc", "2.0"}, {"id", id},
                        {"result", QJsonObject{
                            {"content", QJsonArray{QJsonObject{{"type", "text"}, {"text", text}}}},
                            {"isError", rpc.contains("error")}}}};
                }
            }
        } else {
            response = errorResponse(id, "Método MCP não suportado: " + method);
        }
        out << QJsonDocument(response).toJson(QJsonDocument::Compact) << '\n';
        out.flush();
    }
    return 0;
}

#include "AutomationClient.h"

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTextStream>
#include <unistd.h>

namespace {

QJsonObject tool(const QString &name, const QString &description) {
    return QJsonObject{{"name", name}, {"description", description},
                       {"inputSchema", QJsonObject{{"type", "object"}}}};
}

QJsonArray tools() {
    return QJsonArray{
        tool("system.describe", "Descreve o protocolo público do Studio."),
        tool("project.snapshot", "Obtém o snapshot versionado do projeto."),
        tool("ui.tree", "Obtém a árvore pública de componentes."),
        tool("diagnostics.list", "Lista diagnósticos estruturados."),
        tool("export.validate", "Valida a possibilidade de exportação Bash."),
        tool("preview.status", "Obtém o estado do preview."),
        tool("preview.logs", "Obtém os logs acumulados do preview."),
        tool("project.new", "Cria um projeto vazio."),
        tool("project.open", "Abre um projeto pelo caminho."),
        tool("project.save", "Salva o projeto pelo caminho."),
        tool("widget.add", "Adiciona um componente."),
        tool("widget.remove", "Remove um componente."),
        tool("widget.select", "Seleciona um componente."),
        tool("widget.setProperty", "Altera uma propriedade."),
        tool("history.undo", "Desfaz uma alteração."),
        tool("history.redo", "Refaz uma alteração.")};
}

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
                                   {"result", QJsonObject{{"tools", tools()}}}};
        } else if (method == "tools/call") {
            const QJsonObject params = object.value("params").toObject();
            const QString name = params.value("name").toString();
            if (name.isEmpty()) {
                response = errorResponse(id, "params.name é obrigatório.");
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

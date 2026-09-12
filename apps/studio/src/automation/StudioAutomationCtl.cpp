#include "AutomationClient.h"

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QTextStream>
#include <unistd.h>

int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("showbox-studioctl");
    QCommandLineParser parser;
    parser.setApplicationDescription(
        "Cliente da Showbox Studio Automation Interface.");
    parser.addHelpOption();
    parser.addOption({"socket", "Nome/caminho do socket local.", "socket",
                      QString("showbox-studio-%1").arg(static_cast<uint>(::getuid()))});
    parser.addOption({"params", "Objeto JSON de parâmetros.", "json", "{}"});
    parser.addOption({"pretty", "Formata a resposta JSON para leitura humana."});
    parser.addPositionalArgument("method", "Método JSON-RPC (ou comando curto).");
    parser.process(app);

    const QStringList positional = parser.positionalArguments();
    if (positional.isEmpty()) {
        QTextStream(stderr) << "Uso: showbox-studioctl [opções] método\n";
        return 2;
    }
    QString method = positional.first();
    if (!method.contains('.')) {
        const QHash<QString, QString> aliases{
            {"describe", "system.describe"}, {"capabilities", "system.capabilities"},
            {"snapshot", "project.snapshot"}, {"tree", "ui.tree"},
            {"diagnostics", "diagnostics.list"}, {"validate", "export.validate"},
            {"status", "preview.status"}, {"logs", "preview.logs"}};
        method = aliases.value(method, method);
    }
    QJsonParseError parseError;
    const QJsonDocument paramsDocument = QJsonDocument::fromJson(
        parser.value("params").toUtf8(), &parseError);
    if (parseError.error != QJsonParseError::NoError ||
        !paramsDocument.isObject()) {
        QTextStream(stderr) << "--params deve ser um objeto JSON válido.\n";
        return 2;
    }

    AutomationClient client(parser.value("socket"));
    QString transportError;
    const QJsonObject response = client.call(method, paramsDocument.object(),
                                              &transportError);
    if (response.isEmpty()) {
        QTextStream(stderr) << transportError << '\n';
        return 2;
    }
    QTextStream out(stdout);
    out << QJsonDocument(response).toJson(
        parser.isSet("pretty") ? QJsonDocument::Indented
                                : QJsonDocument::Compact);
    if (response.contains("error"))
        return 1;
    return 0;
}

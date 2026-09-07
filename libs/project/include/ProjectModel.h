#ifndef PROJECT_MODEL_H
#define PROJECT_MODEL_H

#include "ProjectNode.h"
#include <QJsonObject>
#include <QStringList>

// Modelo de projeto versionado do Showbox. Contém a versão do formato, a lista
// de widgets raiz e as regras de validação (estrutural, nomes, referências e
// ações). A serialização JSON e a migração do formato v1 vivem aqui; o
// mapeamento QWidget está no Studio.
class ProjectModel {
public:
    static constexpr int CurrentVersion = 2;

    int version = CurrentVersion;
    QList<ProjectNode> widgets;

    // Converte o modelo para o JSON v2 (formato "showbox").
    QJsonObject toJson() const;

    // Lê o JSON e normaliza para a versão corrente. Migra automaticamente
    // projetos v1; versões desconhecidas são recusadas com mensagem clara.
    static bool fromJson(const QJsonObject &json, ProjectModel *out,
                         QString *error = nullptr);

    // Valida o modelo completo. Retorna uma lista vazia quando válido.
    QStringList validate() const;

    static bool isKnownType(const QString &type);

private:
    static bool migrateV1(const QJsonObject &legacy, ProjectModel *out,
                          QString *error);
    static ProjectNode migrateV1Node(const QJsonObject &legacy);
};

#endif // PROJECT_MODEL_H
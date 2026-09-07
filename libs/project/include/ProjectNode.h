#ifndef PROJECT_NODE_H
#define PROJECT_NODE_H

#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include <QString>

// Nó do modelo de projeto do Showbox. Independente de QWidget: o mapeamento
// para a árvore da interface é responsabilidade do Studio (ProjectWidgetMapper).
struct ProjectNode {
    // Identificação
    QString type;
    QString name;

    // Propriedades genéricas (bag compatível com o formato v1). Valores
    // escalares: string, número ou booleano.
    QJsonObject properties;

    // Ações: texto JSON exato escrito pelo editor (preservado byte a byte).
    // vazio indica ausência de ações.
    QString actions;

    // Combobox/listbox e tabela
    QJsonArray items;
    QJsonArray headers;
    QJsonArray rows;

    // Tela do layout de um container: "" | "box" | "grid" | "form" | "tabs"
    // | "scroll-area". Vazio para componentes atômicos.
    QString layoutType;
    QString layoutOrientation; // "vertical" | "horizontal" (apenas box)

    // Posição no layout do pai. Para grid, R/C deste item; para form, a linha
    // e o papel (label/field). -1 significa "não informado".
    int positionRow = -1;
    int positionColumn = -1;
    int positionRowSpan = 1;
    int positionColumnSpan = 1;
    QString formRole; // "" | "label" | "field"

    // Filhos em ordem de layout
    QList<ProjectNode> children;

    bool isContainer() const;

    QJsonObject toJson() const;
    static ProjectNode fromJson(const QJsonObject &json);
};

#endif // PROJECT_NODE_H
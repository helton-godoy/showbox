#ifndef PROJECT_WIDGET_MAPPER_H
#define PROJECT_WIDGET_MAPPER_H

#include <QList>
#include <QWidget>

#include "ProjectModel.h"

class IStudioWidgetFactory;

// Ponte entre a árvore QWidget do Studio e o modelo de projeto versionado
// (fonte de verdade para salvar/reabrir). A serialização do QWidget lê o estado
// real dos widgets (incluindo textos internos de textbox, itens de combobox,
// headers/rows de tabela e posições grid/form), e a criação reconstrói a
// hierarquia preservando layout, abas e posições.
class ProjectWidgetMapper {
public:
    static ProjectModel toModel(QWidget *root);
    static ProjectNode toNode(QWidget *widget);

    static QWidget *toWidget(const ProjectNode &node,
                             IStudioWidgetFactory *factory);

private:
    static void readData(QWidget *widget, ProjectNode *node);
    static void readTable(QWidget *widget, ProjectNode *node);
    static void readTitleLabel(QWidget *widget, QJsonObject *props);
    static void fillChildren(QWidget *widget, ProjectNode *node);

    static void applyData(QWidget *widget, const ProjectNode &node);
    static void applyProperties(QWidget *widget, const ProjectNode &node);
    static void setTitleLabel(QWidget *widget, const QString &text);
};

#endif // PROJECT_WIDGET_MAPPER_H
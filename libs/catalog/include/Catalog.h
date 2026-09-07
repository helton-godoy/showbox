#ifndef SHOWBOX_CATALOG_H
#define SHOWBOX_CATALOG_H

#include <QList>
#include <QString>
#include <QStringList>

namespace showbox {
namespace catalog {

// Registro declarativo de um tipo de componente showbox. `type` é o nome
// canônico usado no showbox_type e no modelo de projeto; `cliType` é o nome
// aceito pelo `add` do motor quando divergir (ex.: pushbutton para button);
// `aliases` reúne as demais grafias aceitas como sinônimos.
struct WidgetInfo {
    QString type;
    QString cliType;
    QStringList aliases;
    QString displayName;
    QString toolboxGroup;
    bool container = false;
    bool parserCapable = false;
    bool scriptable = false;
    bool queryable = false;
    QStringList events;
    QStringList properties;
};

// Fonte única do catálogo: tipos, aliases, eventos e vocabulário de
// propriedades. Consumida pelo motor, por libs/project e pelo Studio.
const QList<WidgetInfo> &widgetCatalog();

// Resolve um nome (canônico ou alias, sem diferenciar maiúsculas) para o
// registro correspondente; nullptr se desconhecido.
const WidgetInfo *widgetInfo(const QString &type);

QString canonicalType(const QString &type);
QString cliType(const QString &type);

bool isKnownType(const QString &type);
bool isContainer(const QString &type);
bool isScriptable(const QString &type);
bool isParserCapable(const QString &type);
bool isQueryable(const QString &type);
bool isValidEvent(const QString &type, const QString &event);

// Categorias na ordem de exibição do toolbox do Studio.
const QStringList &toolboxGroups();

} // namespace catalog
} // namespace showbox

#endif // SHOWBOX_CATALOG_H
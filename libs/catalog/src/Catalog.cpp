#include "Catalog.h"

namespace showbox {
namespace catalog {

namespace {

// A tabela usa o nome canônico (showbox_type), o nome do `add` do motor quando
// divergir, e mantém as demais grafias como aliases. As flags espelham o
// comportamento atual: scriptable cobre apenas o que o ScriptGenerator exporta
// nesta etapa; parserCapable cobre o que o `add` do motor cria como widget.
const QList<WidgetInfo> &table() {
    static const QList<WidgetInfo> data = {
        {"window", "window", {}, "Window", {},
         true, true, false, false, {}, {"width", "height"}},
        {"label", "label", {}, "Label", "Básico",
         false, true, true, false, {},
         {"text", "icon", "wordwrap", "picture", "animation"}},
        {"button", "pushbutton", {"pushbutton"}, "Button", "Básico",
         false, true, true, false,
         {"clicked", "pressed", "released"},
         {"checkable", "checked", "icon", "apply", "exit", "default"}},
        {"textbox", "textbox", {"lineedit"}, "TextBox", "Entrada",
         false, true, true, true, {},
         {"text", "password", "placeholder"}},
        {"textview", "textview", {"textedit"}, "TextView", "Entrada",
         false, true, false, false, {},
         {"file", "readonly", "html"}},
        {"checkbox", "checkbox", {}, "CheckBox", "Básico",
         false, true, true, true, {},
         {"text", "checked"}},
        {"radiobutton", "radiobutton", {}, "RadioButton", "Básico",
         false, true, true, true, {},
         {"text", "checked"}},
        {"spinbox", "spinbox", {}, "SpinBox", "Entrada",
         false, false, false, false, {},
         {"value", "minimum", "maximum", "singleStep"}},
        {"slider", "slider", {}, "Slider", "Entrada",
         false, true, true, true, {"changed"},
         {"value", "minimum", "maximum", "horizontal", "vertical"}},
        {"combobox", "combobox", {"dropdownlist"}, "ComboBox", "Entrada",
         false, true, false, false, {},
         {"items", "editable", "selection"}},
        {"listbox", "listbox", {}, "ListBox", "Dados",
         false, true, false, false, {},
         {"items", "selection", "activation"}},
        {"table", "table", {}, "Table", "Dados",
         false, true, false, false, {},
         {"headers", "readonly", "selection", "search", "file", "rows"}},
        {"progressbar", "progressbar", {}, "ProgressBar", "Dados",
         false, true, true, false, {},
         {"value", "minimum", "maximum", "format", "busy", "vertical"}},
        {"calendar", "calendar", {}, "Calendar", "Entrada",
         false, true, false, false, {},
         {"date", "minimum", "maximum", "nonavigation", "selection",
          "format"}},
        {"chart", "chart", {}, "Chart", "Dados",
         false, true, false, false, {},
         {"type", "data"}},
        {"separator", "separator", {"line"}, "Separator", "Básico",
         false, true, false, false, {},
         {"horizontal", "vertical", "plain", "raised", "sunken"}},
        {"groupbox", "groupbox", {}, "GroupBox", "Containers",
         true, true, true, false, {},
         {"title", "checkable", "checked", "vertical"}},
        {"frame", "frame", {}, "Frame", "Containers",
         true, true, true, false, {},
         {"vertical", "box", "panel", "styled", "raised", "sunken"}},
        {"tabs", "tabs", {"tabwidget"}, "Tabs", "Containers",
         true, true, true, false, {},
         {"bottom", "left", "right"}},
        {"page", "page", {}, "Page", "Containers",
         true, true, true, false, {},
         {"horizontal", "vertical", "current", "icon"}},
        {"scrollarea", "scrollarea", {}, "ScrollArea", "Containers",
         true, false, false, false, {}, {}},
        {"hboxlayout", "hboxlayout", {}, "HBoxLayout", "Layouts",
         true, false, false, false, {}, {}},
        {"vboxlayout", "vboxlayout", {}, "VBoxLayout", "Layouts",
         true, false, false, false, {}, {}},
        {"gridlayout", "gridlayout", {}, "GridLayout", "Layouts",
         true, false, false, false, {}, {}},
        {"formlayout", "formlayout", {}, "FormLayout", "Layouts",
         true, false, false, false, {}, {}},
        {"horizontalspacer", "horizontalspacer", {}, "HorizontalSpacer",
         "Spacers", false, false, false, false, {}, {}},
        {"verticalspacer", "verticalspacer", {}, "VerticalSpacer",
         "Spacers", false, false, false, false, {}, {}},
    };
    return data;
}

QString lowered(const QString &value) {
    return value.trimmed().toLower();
}

} // namespace

const QList<WidgetInfo> &widgetCatalog() {
    return table();
}

const WidgetInfo *widgetInfo(const QString &type) {
    const QString key = lowered(type);
    for (const WidgetInfo &info : table()) {
        if (info.type == key) {
            return &info;
        }
        for (const QString &alias : info.aliases) {
            if (alias == key) {
                return &info;
            }
        }
    }
    return nullptr;
}

QString canonicalType(const QString &type) {
    const WidgetInfo *info = widgetInfo(type);
    return info ? info->type : QString();
}

QString cliType(const QString &type) {
    const WidgetInfo *info = widgetInfo(type);
    return info ? (info->cliType.isEmpty() ? info->type : info->cliType)
                : QString();
}

bool isKnownType(const QString &type) {
    return widgetInfo(type) != nullptr;
}

bool isContainer(const QString &type) {
    const WidgetInfo *info = widgetInfo(type);
    return info && info->container;
}

bool isScriptable(const QString &type) {
    const WidgetInfo *info = widgetInfo(type);
    return info && info->scriptable;
}

bool isParserCapable(const QString &type) {
    const WidgetInfo *info = widgetInfo(type);
    return info && info->parserCapable;
}

bool isQueryable(const QString &type) {
    const WidgetInfo *info = widgetInfo(type);
    return info && info->queryable;
}

bool isValidEvent(const QString &type, const QString &event) {
    const WidgetInfo *info = widgetInfo(type);
    return info && info->events.contains(lowered(event));
}

const QStringList &toolboxGroups() {
    static const QStringList groups = {
        "Layouts", "Spacers", "Básico", "Entrada", "Dados", "Containers"};
    return groups;
}

} // namespace catalog
} // namespace showbox
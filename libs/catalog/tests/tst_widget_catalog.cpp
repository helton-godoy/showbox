#include "Catalog.h"

#include <QSet>
#include <QtTest>

using namespace showbox::catalog;

class tst_widget_catalog : public QObject {
    Q_OBJECT

private slots:
    void canonicalTypesAreUnique();
    void aliasesResolveAndDontCycle();
    void scriptableSetMatchesCurrentExport();
    void parserCapableMatchesEngineAdd();
    void queryableSetMatchesEngineQuery();
    void containerFlagsCoverStudioAndModel();
    void eventsAreCoherent();
    void toolboxListsFollowCurrentStudio();
    void cliTypeMapping();
    void lookupsAreCaseInsensitive();
};

void tst_widget_catalog::canonicalTypesAreUnique() {
    QSet<QString> seen;
    for (const WidgetInfo &info : widgetCatalog()) {
        QVERIFY(!info.type.isEmpty());
        QVERIFY2(!seen.contains(info.type), qPrintable(info.type));
        seen.insert(info.type);
    }
    QVERIFY(seen.size() >= 27);
}

void tst_widget_catalog::aliasesResolveAndDontCycle() {
    QSet<QString> usedAliases;
    for (const WidgetInfo &info : widgetCatalog()) {
        for (const QString &alias : info.aliases) {
            QVERIFY2(!alias.isEmpty(), qPrintable(info.type));
            QVERIFY2(alias != info.type, qPrintable(info.type));
            QVERIFY2(!usedAliases.contains(alias), qPrintable(alias));
            usedAliases.insert(alias);
            // Alias nunca é canônico de outro registro.
            for (const WidgetInfo &other : widgetCatalog()) {
                QVERIFY2(other.type != alias, qPrintable(alias));
            }
            // Resolve de volta para o canônico.
            QCOMPARE(canonicalType(alias), info.type);
        }
    }
    // Aliases conhecidos um a um.
    QCOMPARE(canonicalType("pushbutton"), QString("button"));
    QCOMPARE(canonicalType("lineedit"), QString("textbox"));
    QCOMPARE(canonicalType("textedit"), QString("textview"));
    QCOMPARE(canonicalType("tabwidget"), QString("tabs"));
    QCOMPARE(canonicalType("dropdownlist"), QString("combobox"));
    QCOMPARE(canonicalType("line"), QString("separator"));
    QVERIFY(isKnownType("pushbutton"));
    QVERIFY(isKnownType("button"));
}

void tst_widget_catalog::scriptableSetMatchesCurrentExport() {
    QSet<QString> scriptable;
    for (const WidgetInfo &info : widgetCatalog()) {
        if (info.scriptable) {
            scriptable.insert(info.type);
        }
    }
    const QSet<QString> expected = {
        "label", "textbox", "checkbox", "radiobutton", "slider",
        "progressbar", "groupbox", "frame", "tabs", "page"};
    // button é exportado como "pushbutton".
    QCOMPARE(scriptable, expected + QSet<QString>{"button"});
    // Tudo que é exportável é criável pelo motor.
    for (const QString &type : scriptable) {
        QVERIFY2(isParserCapable(type), qPrintable(type));
    }
}

void tst_widget_catalog::parserCapableMatchesEngineAdd() {
    QSet<QString> parserCapable;
    for (const WidgetInfo &info : widgetCatalog()) {
        if (info.parserCapable) {
            parserCapable.insert(info.type);
        }
    }
    // Feixe exato do handleAdd do motor (widgets criados via add):
    // window, pushbutton/button, label, checkbox, radiobutton,
    // combobox+dropdownlist, listbox, slider, progressbar,
    // textbox+lineedit, textview+textedit, groupbox, frame, tabs+tabwidget,
    // page, table, calendar, separator+line, chart.
    const QSet<QString> expected = {
        "window",    "button",      "label",      "textbox",   "textview",
        "checkbox",  "radiobutton", "slider",     "progressbar",
        "combobox",  "listbox",     "table",      "calendar",  "chart",
        "separator", "groupbox",    "frame",      "tabs",      "page"};
    QCOMPARE(parserCapable, expected);
}

void tst_widget_catalog::queryableSetMatchesEngineQuery() {
    QSet<QString> queryable;
    for (const WidgetInfo &info : widgetCatalog()) {
        if (info.queryable) {
            queryable.insert(info.type);
        }
    }
    QCOMPARE(queryable,
             (QSet<QString>{"textbox", "slider", "checkbox", "radiobutton"}));
}

void tst_widget_catalog::containerFlagsCoverStudioAndModel() {
    const QSet<QString> containers = {
        "window",    "groupbox",   "frame",     "page",   "tabs",
        "scrollarea", "hboxlayout", "vboxlayout", "gridlayout", "formlayout"};
    for (const QString &type : containers) {
        QVERIFY2(isContainer(type), qPrintable(type));
    }
    QVERIFY(!isContainer("button"));
    QVERIFY(!isContainer("textbox"));
    QVERIFY(!isContainer("separator"));
    // Containers não-atômicos precisam ser conhecidos.
    for (const QString &type : containers) {
        QVERIFY2(isKnownType(type), qPrintable(type));
    }
}

void tst_widget_catalog::eventsAreCoherent() {
    // Somente tipos com eventos os declaram.
    for (const WidgetInfo &info : widgetCatalog()) {
        for (const QString &event : info.events) {
            QVERIFY(!event.isEmpty());
        }
    }
    // Progressbar não emite evento (alinhado ao gerador e ao editor).
    QVERIFY(!isValidEvent("progressbar", "changed"));
    // Slider emite somente changed.
    QVERIFY(isValidEvent("slider", "changed"));
    QVERIFY(!isValidEvent("slider", "clicked"));
    // Botão declara os três; a restrição checkable é do consumidor.
    QVERIFY(isValidEvent("button", "clicked"));
    QVERIFY(isValidEvent("button", "pressed"));
    QVERIFY(isValidEvent("button", "released"));
    QVERIFY(isValidEvent("pushbutton", "clicked"));
    // Tipos sem callbacks recusam qualquer evento.
    QVERIFY(!isValidEvent("checkbox", "clicked"));
    QVERIFY(!isValidEvent("textbox", "changed"));
    QVERIFY(!isValidEvent("label", "changed"));
    QVERIFY(!isValidEvent("tabs", "selectionChanged"));
}

void tst_widget_catalog::toolboxListsFollowCurrentStudio() {
    const QSet<QString> expectedTypes = {
        "hboxlayout", "vboxlayout", "gridlayout", "formlayout",
        "horizontalspacer", "verticalspacer", "label", "button",
        "checkbox", "radiobutton", "separator", "textbox", "textview",
        "spinbox", "slider", "combobox", "calendar", "listbox", "table",
        "progressbar", "chart", "groupbox", "frame", "tabs", "page",
        "scrollarea"};
    QSet<QString> visible;
    for (const WidgetInfo &info : widgetCatalog()) {
        if (!info.toolboxGroup.isEmpty()) {
            visible.insert(info.type);
        }
        QVERIFY(!info.displayName.isEmpty());
    }
    QCOMPARE(visible, expectedTypes);
    // Categorias não-vazias na ordem de exibição.
    for (const QString &group : toolboxGroups()) {
        QVERIFY(!group.isEmpty());
        bool any = false;
        for (const WidgetInfo &info : widgetCatalog()) {
            any = any || info.toolboxGroup == group;
        }
        Q_UNUSED(any);
        QVERIFY2(any, qPrintable(group));
    }
    QCOMPARE(toolboxGroups().size(), 6);
}

void tst_widget_catalog::cliTypeMapping() {
    QCOMPARE(cliType("button"), QString("pushbutton"));
    QCOMPARE(cliType("pushbutton"), QString("pushbutton"));
    QCOMPARE(cliType("textbox"), QString("textbox"));
    QCOMPARE(cliType("tabs"), QString("tabs"));
    QCOMPARE(cliType("combobox"), QString("combobox"));
    QCOMPARE(cliType("desconhecido"), QString());
}

void tst_widget_catalog::lookupsAreCaseInsensitive() {
    QCOMPARE(canonicalType("BUTTON"), QString("button"));
    QCOMPARE(canonicalType("PushButton"), QString("button"));
    QVERIFY(isValidEvent("Slider", "CHANGED"));
    QVERIFY(isContainer("GroupBox"));
    QVERIFY(isScriptable("Page"));
    QVERIFY(!isKnownType(""));
    QVERIFY(!isKnownType("framezinho"));
    QVERIFY(!canonicalType("").isEmpty() == false);
}

QTEST_APPLESS_MAIN(tst_widget_catalog)
#include "tst_widget_catalog.moc"
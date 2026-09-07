#include <QtTest>

#include <algorithm>

#include "ProjectModel.h"

class tst_ProjectModel : public QObject {
    Q_OBJECT

private slots:
    void roundTrip();
    void rejectUnknownVersion();
    void rejectUnknownFormat();
    void rejectMissingFormatInV2();
    void rejectMissingOrInvalidWidgetsInV2();
    void migrateV1();
    void migrateV1KeepsActionsVerbatim();
    void validateRejectsBadNamesAndTypes();
    void validateRejectsBadReferences();
    void validateRejectsBadEvents();
    void validateRejectsStructureBreaks();
    void validateDescendsIntoChildren();
    void migratesGridAndFormNodes();

private:
    QJsonObject basicProject() const;
};

QJsonObject tst_ProjectModel::basicProject() const {
    ProjectModel model;
    ProjectNode entry;
    entry.type = "textbox";
    entry.name = "entry";
    entry.properties = {{"text", "Olá"}};

    ProjectNode run;
    run.type = "button";
    run.name = "run";
    run.properties = {{"text", "Saudar"}};
    run.actions =
        "{\"clicked\": [{\"type\": \"shell\", \"command\": "
        "\"showbox_get VALUE entry\"}]}";

    ProjectNode result;
    result.type = "label";
    result.name = "result";
    result.properties = {{"text", "Resultado"}};

    model.widgets = {entry, run, result};
    return model.toJson();
}

void tst_ProjectModel::roundTrip() {
    const QJsonObject json = basicProject();

    ProjectModel loaded;
    QString error;
    QVERIFY2(ProjectModel::fromJson(json, &loaded, &error), qPrintable(error));
    QCOMPARE(loaded.version, 2);
    QCOMPARE(loaded.widgets.size(), 3);
    QCOMPARE(loaded.widgets[1].name, "run");
    QCOMPARE(loaded.widgets[1].properties["text"].toString(), "Saudar");
    QVERIFY(loaded.validate().isEmpty());
}

void tst_ProjectModel::rejectUnknownVersion() {
    QJsonObject json = basicProject();
    json["version"] = "9";

    ProjectModel loaded;
    QString error;
    QVERIFY(!ProjectModel::fromJson(json, &loaded, &error));
    QVERIFY(error.contains("desconhecida"));
}

void tst_ProjectModel::rejectUnknownFormat() {
    QJsonObject json = basicProject();
    json["format"] = "outro-app";

    ProjectModel loaded;
    QString error;
    QVERIFY(!ProjectModel::fromJson(json, &loaded, &error));
    QVERIFY(error.contains("Formato"));
}

void tst_ProjectModel::migrateV1() {
    const QJsonObject legacy = {
        {"version", "1.0"},
        {"widgets", QJsonArray{
                        QJsonObject{{"type", "textbox"},
                                    {"name", "entry"},
                                    {"properties", QJsonObject()}},
                        QJsonObject{{"type", "button"},
                                    {"name", "run"},
                                    {"properties",
                                     QJsonObject{{"text", "Saudar"},
                                                 {"checked", true},
                                                 {"width", 100}}}}}}};

    ProjectModel model;
    QString error;
    QVERIFY2(ProjectModel::fromJson(legacy, &model, &error), qPrintable(error));
    QCOMPARE(model.version, 2);
    QCOMPARE(model.widgets.size(), 2);

    const ProjectNode &run = model.widgets[1];
    QCOMPARE(run.properties["text"].toString(), "Saudar");
    QCOMPARE(run.properties["checked"].toBool(), true);
    QCOMPARE(run.properties["width"].toInt(), 100);
    QVERIFY(!run.properties.contains("showbox_actions"));
    QVERIFY(model.validate().isEmpty());
}

void tst_ProjectModel::migrateV1KeepsActionsVerbatim() {
    const QString actions =
        "{\"clicked\": [{\"type\": \"shell\", \"command\": \"showbox_get "
        "VALUE entry\\nshowbox_set result text \\\"Olá: $VALUE\\\"\\n\"}]}";
    const QJsonObject legacy = {
        {"version", "1.0"},
        {"widgets",
         QJsonArray{
             QJsonObject{{"type", "button"},
                         {"name", "run"},
                         {"properties",
                          QJsonObject{{"showbox_actions", actions}}}}}}};

    ProjectModel model;
    QString error;
    QVERIFY2(ProjectModel::fromJson(legacy, &model, &error), qPrintable(error));
    QCOMPARE(model.widgets[0].actions, actions);
    QVERIFY(model.validate().isEmpty());
}

void tst_ProjectModel::validateRejectsBadNamesAndTypes() {
    ProjectNode entry;
    entry.type = "textbox";
    entry.name = "entry";

    ProjectNode invalid;
    invalid.type = "textbox";
    invalid.name = "1invalido";

    ProjectNode unknown;
    unknown.type = "futuristico";
    unknown.name = "fut";

    ProjectNode reserved;
    reserved.type = "label";
    reserved.name = "main";

    ProjectModel model;
    model.widgets = {entry, invalid, unknown, reserved};

    const QStringList issues = model.validate();
    QVERIFY(!issues.isEmpty());
    QVERIFY(std::any_of(issues.begin(), issues.end(), [](const QString &issue) {
        return issue.contains("Nome inválido");
    }));
    QVERIFY(std::any_of(issues.begin(), issues.end(), [](const QString &issue) {
        return issue.contains("Tipo desconhecido");
    }));
    QVERIFY(std::any_of(issues.begin(), issues.end(), [](const QString &issue) {
        return issue.contains("reservado");
    }));
}

void tst_ProjectModel::validateRejectsBadReferences() {
    const QString lookup = "clicked\": [{\"type\": \"query\", \"target\": "
                           "\"ghost\", \"variable\": \"VAL\"}]}";

    ProjectNode button;
    button.type = "button";
    button.name = "run";
    button.actions = "{\"" + lookup;

    ProjectModel model;
    model.widgets = {button};
    const QStringList issues = model.validate();
    QVERIFY(!issues.isEmpty());
    QVERIFY(std::any_of(issues.begin(), issues.end(), [](const QString &issue) {
        return issue.contains("Destino inexistente");
    }));
}

void tst_ProjectModel::validateRejectsBadEvents() {
    ProjectNode label;
    label.type = "label";
    label.name = "lbl";
    label.actions =
        "{\"clicked\": [{\"type\": \"shell\", \"command\": \"echo ok\"}]}";

    ProjectModel model;
    model.widgets = {label};
    QStringList issues = model.validate();
    QVERIFY(!issues.isEmpty());
    QVERIFY(std::any_of(issues.begin(), issues.end(), [](const QString &issue) {
        return issue.contains("Evento");
    }));
}

void tst_ProjectModel::validateRejectsStructureBreaks() {
    ProjectNode label;
    label.type = "label";
    label.name = "lbl";
    label.children.append(ProjectNode());

    ProjectModel model;
    model.widgets = {label};
    const QStringList issues = model.validate();
    QVERIFY(!issues.isEmpty());
    QVERIFY(std::any_of(issues.begin(), issues.end(), [](const QString &issue) {
        return issue.contains("não aceita filhos");
    }));
}

void tst_ProjectModel::rejectMissingFormatInV2() {
    QJsonObject json = basicProject();
    json.remove("format");

    ProjectModel loaded;
    QString error;
    QVERIFY(!ProjectModel::fromJson(json, &loaded, &error));
    QVERIFY(error.contains("format"));
}

void tst_ProjectModel::rejectMissingOrInvalidWidgetsInV2() {
    QJsonObject noWidgets = basicProject();
    noWidgets.remove("widgets");

    QJsonObject wrongTypeWidgets = basicProject();
    wrongTypeWidgets["widgets"] = "inválido";

    QJsonObject nonObjectItems = basicProject();
    nonObjectItems["widgets"] = QJsonArray{"um", 2};

    ProjectModel loaded;
    QString error;
    QVERIFY(!ProjectModel::fromJson(noWidgets, &loaded, &error));
    QVERIFY(error.contains("widgets"));
    QVERIFY(!ProjectModel::fromJson(wrongTypeWidgets, &loaded, &error));
    QVERIFY(error.contains("widgets"));
    QVERIFY(!ProjectModel::fromJson(nonObjectItems, &loaded, &error));
    QVERIFY(error.contains("objeto"));
}

void tst_ProjectModel::validateDescendsIntoChildren() {
    ProjectNode abas;
    abas.type = "tabs";
    abas.name = "abas";

    ProjectNode page1;
    page1.type = "page";
    page1.name = "p1";
    ProjectNode run;
    run.type = "button";
    run.name = "run";
    run.actions =
        "{\"clicked\": [{\"type\": \"shell\", \"command\": \"   \"}]}";
    page1.children.append(run);

    ProjectNode page2;
    page2.type = "page";
    page2.name = "p2";
    ProjectNode lost;
    lost.type = "button";
    lost.name = "b2";
    lost.actions = "{\"clicked\": [{\"type\": \"query\", \"target\": "
                   "\"fantasma\", \"variable\": \"VAL\"}]}";
    page2.children.append(lost);

    abas.children = {page1, page2};

    ProjectModel model;
    model.widgets = {abas};

    const QStringList issues = model.validate();
    QVERIFY(std::any_of(issues.begin(), issues.end(), [](const QString &issue) {
        return issue.contains("Ação shell vazia");
    }));
    QVERIFY(std::any_of(issues.begin(), issues.end(), [](const QString &issue) {
        return issue.contains("Destino inexistente");
    }));
}

void tst_ProjectModel::migratesGridAndFormNodes() {
    const QJsonObject content = {
        {"type", "label"},
        {"name", "celula"},
        {"properties", QJsonObject{{"text", "1"}}}};
    // Componente gridlayout apenas com tipo no nó raiz, formato v1.
    const QJsonObject legacy = {
        {"version", "1.0"},
        {"widgets",
         QJsonArray{QJsonObject{{"type", "gridlayout"},
                                {"name", "grade"},
                                {"children", QJsonArray{content}}}}}};

    ProjectModel model;
    QString error;
    QVERIFY2(ProjectModel::fromJson(legacy, &model, &error), qPrintable(error));
    QCOMPARE(model.widgets[0].layoutType, "grid");
    QCOMPARE(model.widgets[0].children.size(), 1);
}

QTEST_MAIN(tst_ProjectModel)
#include "tst_ProjectModel.moc"
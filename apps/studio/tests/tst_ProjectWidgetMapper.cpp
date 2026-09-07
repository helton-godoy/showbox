#include <QtTest>

#include <QComboBox>
#include <QFormLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QTabWidget>
#include <QTableWidget>
#include <QTemporaryFile>
#include <QVBoxLayout>

#include "core/ProjectSerializer.h"
#include "core/ProjectWidgetMapper.h"
#include "core/StudioWidgetFactory.h"
#include "custom_table_widget.h"

class tst_ProjectWidgetMapper : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void roundTripNestedContainers();
    void roundTripGridAndForm();
    void roundTripComboboxListTable();
    void roundTripActionsVerbatim();
    void tabsKeepPageTitles();

private:
    StudioWidgetFactory *m_factory;

    QWidget *makeRoot() const;
    void clearTree(QWidget *root) const;
};

void tst_ProjectWidgetMapper::initTestCase() { m_factory = new StudioWidgetFactory(); }

void tst_ProjectWidgetMapper::cleanupTestCase() { delete m_factory; }

QWidget *tst_ProjectWidgetMapper::makeRoot() const {
    auto *root = new QWidget();
    root->setLayout(new QVBoxLayout(root));
    return root;
}

void tst_ProjectWidgetMapper::clearTree(QWidget *root) const {
    QLayoutItem *item;
    while ((item = root->layout()->takeAt(0)) != nullptr) {
        if (QWidget *w = item->widget()) {
            delete w;
        }
        delete item;
    }
    delete root;
}

void tst_ProjectWidgetMapper::roundTripNestedContainers() {
    QWidget *root = makeRoot();

    QWidget *group = m_factory->createWidget("groupbox", "grupo");
    auto *btn = m_factory->createWidget("button", "ok");
    btn->setProperty("text", "Confirmar");
    auto *chk = m_factory->createWidget("checkbox", "opt");
    chk->setProperty("text", "Aceitar");
    chk->setProperty("checked", true);
    group->layout()->addWidget(btn);
    group->layout()->addWidget(chk);

    QWidget *frame = m_factory->createWidget("frame", "moldura");
    auto *lbl = m_factory->createWidget("label", "titulo");
    lbl->setProperty("text", "Olá mundo");
    frame->layout()->addWidget(lbl);

    root->layout()->addWidget(group);
    root->layout()->addWidget(frame);

    const ProjectModel model = ProjectWidgetMapper::toModel(root);
    QCOMPARE(model.widgets.size(), 2);
    QCOMPARE(model.validate().size(), 0);

    const ProjectNode &groupNode = model.widgets[0];
    QCOMPARE(groupNode.layoutType, "box");
    QCOMPARE(groupNode.layoutOrientation, "vertical");
    QCOMPARE(groupNode.children.size(), 2);
    QCOMPARE(groupNode.children[1].properties["checked"].toBool(), true);

    // Round-trip de volta para widgets
    QList<QWidget *> rebuilt;
    for (const ProjectNode &node : model.widgets) {
        rebuilt.append(ProjectWidgetMapper::toWidget(node, m_factory));
    }
    QCOMPARE(rebuilt.size(), 2);

    QWidget *reGroup = rebuilt[0];
    QCOMPARE(reGroup->property("showbox_type").toString(), "groupbox");
    QCOMPARE(reGroup->layout()->count(), 2);
    QWidget *reOk = reGroup->layout()->itemAt(0)->widget();
    QCOMPARE(reOk->objectName(), "ok");
    QCOMPARE(reOk->property("text").toString(), "Confirmar");
    QCOMPARE(reGroup->layout()->itemAt(1)->widget()->property("checked").toBool(), true);

    QWidget *reFrame = rebuilt[1];
    QCOMPARE(reFrame->layout()->count(), 1);
    QWidget *reLbl = reFrame->layout()->itemAt(0)->widget();
    QCOMPARE(reLbl->objectName(), "titulo");
    QCOMPARE(reLbl->property("text").toString(), "Olá mundo");

    qDeleteAll(rebuilt);
    clearTree(root);
}

void tst_ProjectWidgetMapper::roundTripGridAndForm() {
    QWidget *root = makeRoot();

    QWidget *gridWrapper = m_factory->createWidget("gridlayout", "grade");
    auto *grid = qobject_cast<QGridLayout *>(gridWrapper->layout());
    QVERIFY(grid);
    auto *cellA = m_factory->createWidget("button", "a");
    auto *cellB = m_factory->createWidget("label", "b");
    grid->addWidget(cellA, 1, 2, 2, 3);
    grid->addWidget(cellB, 0, 1, 1, 1);

    QWidget *formWrapper = m_factory->createWidget("formlayout", "formulario");
    auto *form = qobject_cast<QFormLayout *>(formWrapper->layout());
    QVERIFY(form);
    auto *lab = m_factory->createWidget("label", "lab");
    auto *field = m_factory->createWidget("textbox", "campo");
    auto *field2 = m_factory->createWidget("textbox", "campo2");
    form->addRow(lab, field);
    form->addRow(field2);

    root->layout()->addWidget(gridWrapper);
    root->layout()->addWidget(formWrapper);

    const ProjectModel model = ProjectWidgetMapper::toModel(root);
    QCOMPARE(model.validate().size(), 0);

    const ProjectNode &gridNode = model.widgets[0];
    QCOMPARE(gridNode.layoutType, "grid");
    QCOMPARE(gridNode.children.size(), 2);
    const ProjectNode &cellANode = gridNode.children[0];
    QCOMPARE(cellANode.positionRow, 1);
    QCOMPARE(cellANode.positionColumn, 2);
    QCOMPARE(cellANode.positionRowSpan, 2);
    QCOMPARE(cellANode.positionColumnSpan, 3);

    const ProjectNode &formNode = model.widgets[1];
    QCOMPARE(formNode.layoutType, "form");
    QCOMPARE(formNode.children.size(), 3);
    QCOMPARE(formNode.children[0].formRole, "label");
    QCOMPARE(formNode.children[0].positionRow, 0);
    QCOMPARE(formNode.children[1].formRole, "field");
    QCOMPARE(formNode.children[1].positionRow, 0);
    QCOMPARE(formNode.children[2].formRole, "field");
    QCOMPARE(formNode.children[2].positionRow, 1);

    // Reconstrução
    QList<QWidget *> rebuilt;
    for (const ProjectNode &node : model.widgets) {
        rebuilt.append(ProjectWidgetMapper::toWidget(node, m_factory));
    }

    auto *reGrid = qobject_cast<QGridLayout *>(rebuilt[0]->layout());
    QVERIFY(reGrid);
    int row = -1, column = -1, rowSpan = 0, columnSpan = 0;
    QWidget *found = nullptr;
    for (int i = 0; i < reGrid->count(); ++i) {
        if (reGrid->itemAt(i)->widget() &&
            reGrid->itemAt(i)->widget()->objectName() == "a") {
            found = reGrid->itemAt(i)->widget();
            reGrid->getItemPosition(i, &row, &column, &rowSpan, &columnSpan);
        }
    }
    QVERIFY(found);
    QCOMPARE(row, 1);
    QCOMPARE(column, 2);
    QCOMPARE(rowSpan, 2);
    QCOMPARE(columnSpan, 3);

    auto *reForm = qobject_cast<QFormLayout *>(rebuilt[1]->layout());
    QVERIFY(reForm);
    QCOMPARE(reForm->rowCount(), 2);
    QWidget *reLab = reForm->itemAt(0, QFormLayout::LabelRole)->widget();
    QWidget *reField = reForm->itemAt(0, QFormLayout::FieldRole)->widget();
    QWidget *reField2 = reForm->itemAt(1, QFormLayout::FieldRole)->widget();
    QVERIFY(reLab);
    QVERIFY(reField);
    QVERIFY(reField2);
    QCOMPARE(reLab->objectName(), "lab");
    QCOMPARE(reField->objectName(), "campo");
    QCOMPARE(reField2->objectName(), "campo2");
    QVERIFY(!reForm->itemAt(1, QFormLayout::LabelRole));

    qDeleteAll(rebuilt);
    clearTree(root);
}

void tst_ProjectWidgetMapper::roundTripComboboxListTable() {
    QWidget *root = makeRoot();

    auto *combo = m_factory->createWidget("combobox", "cores");
    auto *cb = combo->findChild<QComboBox *>();
    QVERIFY(cb);
    cb->addItems({"Vermelho", "Verde", "Azul"});
    cb->setCurrentIndex(1);

    auto *list = m_factory->createWidget("listbox", "arquivos");
    list->findChild<QListWidget *>()->addItems({"a.sh", "b.sh"});

    auto *table = m_factory->createWidget("table", "dados");
    auto *tw = qobject_cast<CustomTableWidget *>(table)->table();
    tw->setColumnCount(2);
    tw->setHorizontalHeaderLabels({"Nome", "Valor"});
    tw->setRowCount(2);
    tw->setItem(0, 0, new QTableWidgetItem("x"));
    tw->setItem(0, 1, new QTableWidgetItem("1"));
    tw->setItem(1, 0, new QTableWidgetItem("y"));
    tw->setItem(1, 1, new QTableWidgetItem("2"));

    root->layout()->addWidget(combo);
    root->layout()->addWidget(list);
    root->layout()->addWidget(table);

    const ProjectModel model = ProjectWidgetMapper::toModel(root);
    QCOMPARE(model.validate().size(), 0);
    QCOMPARE(model.widgets.size(), 3);

    const ProjectNode &comboNode = model.widgets[0];
    QCOMPARE(comboNode.items.size(), 3);
    QCOMPARE(comboNode.items[1].toString(), "Verde");
    QCOMPARE(comboNode.properties["currentIndex"].toInt(), 1);

    const ProjectNode &listNode = model.widgets[1];
    QCOMPARE(listNode.items.size(), 2);
    QCOMPARE(listNode.items[0].toString(), "a.sh");

    const ProjectNode &tableNode = model.widgets[2];
    QCOMPARE(tableNode.headers.size(), 2);
    QCOMPARE(tableNode.headers[0].toString(), "Nome");
    QCOMPARE(tableNode.rows.size(), 2);
    QCOMPARE(tableNode.rows[0].toArray()[1].toString(), "1");

    QList<QWidget *> rebuilt;
    for (const ProjectNode &node : model.widgets) {
        rebuilt.append(ProjectWidgetMapper::toWidget(node, m_factory));
    }

    auto *reCombo = rebuilt[0]->findChild<QComboBox *>();
    QVERIFY(reCombo);
    QCOMPARE(reCombo->count(), 3);
    QCOMPARE(reCombo->itemText(1), "Verde");
    QCOMPARE(reCombo->currentIndex(), 1);

    auto *reList = rebuilt[1]->findChild<QListWidget *>();
    QVERIFY(reList);
    QCOMPARE(reList->count(), 2);
    QCOMPARE(reList->item(0)->text(), "a.sh");

    auto *reTw = qobject_cast<CustomTableWidget *>(rebuilt[2])->table();
    QCOMPARE(reTw->columnCount(), 2);
    QCOMPARE(reTw->horizontalHeaderItem(0)->text(), "Nome");
    QCOMPARE(reTw->rowCount(), 2);
    QCOMPARE(reTw->item(1, 1)->text(), "2");

    qDeleteAll(rebuilt);
    clearTree(root);
}

void tst_ProjectWidgetMapper::roundTripActionsVerbatim() {
    const QString actions =
        "{\"clicked\": [{\"type\": \"shell\", \"command\": \"showbox_get "
        "VALUE entry\\nshowbox_set result text \\\"Olá: $VALUE\\\"\\nprintf "
        "\\\"Ação concluída.\\\\n\\\"\"}]}";

    QWidget *root = makeRoot();
    auto *btn = m_factory->createWidget("button", "run");
    btn->setProperty("text", "Saudar");
    btn->setProperty("showbox_actions", actions);
    root->layout()->addWidget(btn);

    const ProjectModel model = ProjectWidgetMapper::toModel(root);
    QCOMPARE(model.widgets[0].actions, actions);
    QVERIFY(model.validate().isEmpty());

    QWidget *rebuilt = ProjectWidgetMapper::toWidget(model.widgets[0], m_factory);
    QCOMPARE(rebuilt->property("showbox_actions").toString(), actions);

    delete rebuilt;

    // Projeto completo salvo/reaberto preserva as ações.
    QTemporaryFile tempFile;
    QVERIFY(tempFile.open());
    const QString filename = tempFile.fileName();
    tempFile.close();

    ProjectSerializer serializer;
    QVERIFY(serializer.save(filename, root, m_factory));
    QList<QWidget *> loaded;
    QVERIFY2(serializer.load(filename, m_factory, loaded), "load falhou");
    QCOMPARE(loaded.size(), 1);
    QCOMPARE(loaded[0]->property("showbox_actions").toString(), actions);
    QVERIFY(serializer.errors().isEmpty());

    qDeleteAll(loaded);
    clearTree(root);
}

void tst_ProjectWidgetMapper::tabsKeepPageTitles() {
    QWidget *root = makeRoot();

    QWidget *tabs = m_factory->createWidget("tabs", "abas");
    auto *tw = qobject_cast<QTabWidget *>(tabs);
    while (tw->count()) {
        QWidget *page = tw->widget(0);
        tw->removeTab(0);
        delete page;
    }

    auto *page1 = m_factory->createWidget("page", "pg1");
    page1->setProperty("title", "Primeira");
    auto *lbl = m_factory->createWidget("label", "conteudo");
    lbl->setProperty("text", "texto página 1");
    page1->layout()->addWidget(lbl);
    tw->addTab(page1, page1->property("title").toString());

    auto *page2 = m_factory->createWidget("page", "pg2");
    page2->setProperty("title", "Segunda");
    tw->addTab(page2, page2->property("title").toString());

    root->layout()->addWidget(tabs);

    const ProjectModel model = ProjectWidgetMapper::toModel(root);
    QCOMPARE(model.validate().size(), 0);
    const ProjectNode &tabsNode = model.widgets[0];
    QCOMPARE(tabsNode.layoutType, "tabs");
    QCOMPARE(tabsNode.children.size(), 2);
    QCOMPARE(tabsNode.children[0].properties["title"].toString(), "Primeira");
    QCOMPARE(tabsNode.children[0].children.size(), 1);

    QWidget *reTabs = ProjectWidgetMapper::toWidget(tabsNode, m_factory);
    auto *reTw = qobject_cast<QTabWidget *>(reTabs);
    QCOMPARE(reTw->count(), 2);
    QCOMPARE(reTw->tabText(0), "Primeira");
    QCOMPARE(reTw->tabText(1), "Segunda");
    QWidget *rePage = reTw->widget(0);
    QCOMPARE(rePage->property("title").toString(), "Primeira");
    QCOMPARE(rePage->layout()->count(), 1);
    QWidget *reLbl = rePage->layout()->itemAt(0)->widget();
    QCOMPARE(reLbl->objectName(), "conteudo");
    QCOMPARE(reLbl->property("text").toString(), "texto página 1");

    delete reTabs;
    clearTree(root);
}

QTEST_MAIN(tst_ProjectWidgetMapper)
#include "tst_ProjectWidgetMapper.moc"
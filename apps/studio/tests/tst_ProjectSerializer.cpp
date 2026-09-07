#include <QtTest>
#include <QPushButton>

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTemporaryFile>

#include "core/ProjectSerializer.h"
#include "core/StudioWidgetFactory.h"

class tst_ProjectSerializer : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testSaveLoad();
    void openV1ExampleMigratesToV2();
    void rejectsUnknownVersion();
    void rejectsMalformedJson();

private:
    StudioWidgetFactory *m_factory;
};

void tst_ProjectSerializer::initTestCase()
{
    m_factory = new StudioWidgetFactory();
}

void tst_ProjectSerializer::cleanupTestCase()
{
    delete m_factory;
}

void tst_ProjectSerializer::testSaveLoad()
{
    // 1. Criar Árvore Original
    QWidget *root = new QWidget();
    
    QWidget *btn = m_factory->createWidget("PushButton", "btn_save");
    btn->setProperty("text", "Save Me");
    const QString actions = R"({"clicked":[{"type":"shell","command":"showbox_get VALUE entry"}]})";
    btn->setProperty("showbox_actions", actions);
    btn->setParent(root); // Simula Canvas

    QWidget *chk = m_factory->createWidget("CheckBox", "chk_opt");
    chk->setProperty("checked", true);
    chk->setParent(root);

    // 2. Salvar
    QTemporaryFile tempFile;
    QVERIFY(tempFile.open());
    QString filename = tempFile.fileName();
    tempFile.close(); // Fechar para permitir escrita pelo Serializer

    ProjectSerializer serializer;
    bool saved = serializer.save(filename, root, m_factory);
    QVERIFY(saved);

    // O arquivo deve estar no formato v2.
    QFile savedFile(filename);
    QVERIFY(savedFile.open(QIODevice::ReadOnly));
    const QJsonObject savedJson =
        QJsonDocument::fromJson(savedFile.readAll()).object();
    QCOMPARE(savedJson["format"].toString(), "showbox");
    QCOMPARE(savedJson["version"].toInt(), 2);

    // 3. Carregar
    QList<QWidget*> loadedWidgets;
    bool loaded = serializer.load(filename, m_factory, loadedWidgets);
    QVERIFY(loaded);
    QVERIFY(serializer.errors().isEmpty());
    QCOMPARE(loadedWidgets.size(), 2);

    // 4. Verificar
    QWidget *w1 = loadedWidgets[0];
    QWidget *w2 = loadedWidgets[1];
    
    QWidget *loadedBtn = (w1->objectName() == "btn_save") ? w1 : w2;
    QWidget *loadedChk = (w1->objectName() == "chk_opt") ? w1 : w2;

    QVERIFY(loadedBtn);
    QVERIFY(loadedChk);
    
    QCOMPARE(loadedBtn->property("text").toString(), "Save Me");
    QCOMPARE(loadedChk->property("checked").toBool(), true);
    QCOMPARE(loadedBtn->property("showbox_type"), btn->property("showbox_type"));
    QVERIFY(qobject_cast<QPushButton *>(loadedBtn));
    QCOMPARE(loadedBtn->property("showbox_actions").toString(), actions);

    delete root;
    qDeleteAll(loadedWidgets);
}

void tst_ProjectSerializer::openV1ExampleMigratesToV2()
{
    ProjectSerializer serializer;
    QList<QWidget *> widgets;
    QVERIFY2(serializer.load(DEMO_PROJECT_PATH, m_factory, widgets),
             "projeto v1 de exemplo deve abrir");
    QVERIFY(serializer.errors().isEmpty());
    QCOMPARE(widgets.size(), 3);

    // Ações shell do v1 preservadas.
    QString actions;
    for (QWidget *w : widgets) {
        if (w->objectName() == "run") {
            actions = w->property("showbox_actions").toString();
        }
    }
    QVERIFY(!actions.isEmpty());
    QVERIFY(actions.contains("showbox_get VALUE entry"));

    // Reescreve como v2 e reabre sem perda.
    QWidget *root = new QWidget();
    root->setLayout(new QVBoxLayout(root));
    for (QWidget *w : widgets) {
        root->layout()->addWidget(w);
    }

    QTemporaryFile tempFile;
    QVERIFY(tempFile.open());
    const QString filename = tempFile.fileName();
    tempFile.close();

    ProjectSerializer writer;
    QVERIFY(writer.save(filename, root, m_factory));

    QList<QWidget *> reloaded;
    QVERIFY2(writer.load(filename, m_factory, reloaded),
             "projeto v2 reescrito deve reabrir");
    QString actionsAfter;
    for (QWidget *w : reloaded) {
        if (w->objectName() == "run") {
            actionsAfter = w->property("showbox_actions").toString();
        }
    }
    QCOMPARE(actionsAfter, actions);

    delete root;
    qDeleteAll(reloaded);
}

void tst_ProjectSerializer::rejectsUnknownVersion()
{
    QTemporaryFile tempFile;
    QVERIFY(tempFile.open());
    const QString filename = tempFile.fileName();
    tempFile.close();

    QFile file(filename);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.write(R"({"format":"showbox","version":9,"widgets":[]})");
    file.close();

    ProjectSerializer serializer;
    QList<QWidget *> widgets;
    QVERIFY(!serializer.load(filename, m_factory, widgets));
    QCOMPARE(widgets.size(), 0);
    QCOMPARE(serializer.errors().size(), 1);
    QVERIFY(serializer.errors().first().contains("desconhecida"));
}

void tst_ProjectSerializer::rejectsMalformedJson()
{
    QTemporaryFile tempFile;
    QVERIFY(tempFile.open());
    const QString filename = tempFile.fileName();
    tempFile.close();

    QFile file(filename);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.write("isto não é json");
    file.close();

    ProjectSerializer serializer;
    QList<QWidget *> widgets;
    QVERIFY(!serializer.load(filename, m_factory, widgets));
    QVERIFY(!serializer.errors().isEmpty());
}

QTEST_MAIN(tst_ProjectSerializer)
#include "tst_ProjectSerializer.moc"
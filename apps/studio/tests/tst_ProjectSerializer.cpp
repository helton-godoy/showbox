#include <QtTest>
#include "core/ProjectSerializer.h"
#include "core/StudioWidgetFactory.h"
#include <QTemporaryFile>

class tst_ProjectSerializer : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testSaveLoad();

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

    // 3. Carregar
    QList<QWidget*> loadedWidgets;
    bool loaded = serializer.load(filename, m_factory, loadedWidgets);
    QVERIFY(loaded);
    QCOMPARE(loadedWidgets.size(), 2);

    // 4. Verificar
    // A ordem pode variar dependendo da implementação (children list order), 
    // mas assumindo ordem de inserção:
    
    QWidget *w1 = loadedWidgets[0];
    QWidget *w2 = loadedWidgets[1];
    
    // Identificar qual é qual
    QWidget *loadedBtn = (w1->objectName() == "btn_save") ? w1 : w2;
    QWidget *loadedChk = (w1->objectName() == "chk_opt") ? w1 : w2;

    QVERIFY(loadedBtn);
    QVERIFY(loadedChk);
    
    QCOMPARE(loadedBtn->property("text").toString(), "Save Me");
    QCOMPARE(loadedChk->property("checked").toBool(), true);
    QCOMPARE(loadedBtn->property("showbox_type").toString(), "pushbutton"); // lower case

    delete root;
    qDeleteAll(loadedWidgets);
}

QTEST_MAIN(tst_ProjectSerializer)
#include "tst_ProjectSerializer.moc"

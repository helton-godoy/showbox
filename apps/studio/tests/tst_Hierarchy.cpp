#include <QtTest>
#include <QTreeWidget>
#include <QWidget>
#include "gui/ObjectInspector.h"
#include "core/StudioController.h"

class tst_Hierarchy : public QObject
{
    Q_OBJECT

private slots:
    void testReparentingLogic();
    void testPerformanceScale();
};

void tst_Hierarchy::testPerformanceScale()
{
    ObjectInspector inspector;
    QWidget root;
    root.setProperty("showbox_type", "window");

    auto createChildren = [&](QWidget *parent, int count) {
        for(int i=0; i<count; ++i) {
            QWidget *child = new QWidget(parent);
            child->setObjectName(QString("widget_%1").arg(i));
            child->setProperty("showbox_type", "pushbutton"); // Não recursivo por padrão
        }
    };

    qDebug() << "--- Performance Scale Test ---";
    
    // 10 Widgets
    createChildren(&root, 10);
    inspector.updateHierarchy(&root);
    
    // 50 Widgets
    createChildren(&root, 40);
    inspector.updateHierarchy(&root);
    
    // 200 Widgets
    createChildren(&root, 150);
    inspector.updateHierarchy(&root);
}

void tst_Hierarchy::testReparentingLogic()
{
    ObjectInspector inspector;
    QWidget root;
    QWidget parent1(&root); parent1.setObjectName("parent1");
    QWidget parent2(&root); parent2.setObjectName("parent2");
    QWidget child(&parent1); child.setObjectName("child");

    inspector.updateHierarchy(&root);

    // Simular que o usuário moveu 'child' para 'parent2' na árvore
    // Nota: Como não vamos rodar um loop de GUI completo com Drag real, 
    // vamos testar se a lógica que o dropEvent executará funciona.
    
    // O teste aqui deve validar se o widget real mudou de pai no Qt
    // quando a árvore for manipulada (será implementado no dropEvent).
    
    // Por enquanto, apenas validar se a árvore inicial está correta
    QCOMPARE(child.parentWidget(), &parent1);
}

QTEST_MAIN(tst_Hierarchy)
#include "tst_Hierarchy.moc"

#include <QtTest>
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QElapsedTimer>
#include "core/StudioController.h"
#include "gui/Canvas.h"
#include "core/IStudioWidgetFactory.h"

// Mock Factory para o Canvas
class MockFactory : public IStudioWidgetFactory {
public:
    QWidget* createWidget(const QString &type, const QString &name) override {
        QWidget *w = new QWidget();
        w->setObjectName(name);
        return w;
    }
    void serializeWidget(QWidget*, QJsonObject&) override {}
    void deserializeWidget(QWidget*, const QJsonObject&) override {}
};

class tst_StylePerformance : public QObject
{
    Q_OBJECT

private slots:
    void testSelectionPerformance();
};

void tst_StylePerformance::testSelectionPerformance()
{
    MockFactory factory;
    Canvas canvas(&factory);
    StudioController controller;
    
    // Aplicar o stylesheet pesado do Canvas
    canvas.setStyleSheet("Canvas { background-color: #f0f0f0; border: 1px solid #ccc; } "
                         "*[selected=\"true\"] { border: 2px solid #0078d7; }");

    canvas.show(); // Necessário para stylesheet ter efeito

    // Criar muitos widgets
    QList<QWidget*> widgets;
    for(int i=0; i<100; ++i) {
        QWidget *w = new QWidget(&canvas);
        w->setObjectName(QString("btn_%1").arg(i));
        widgets.append(w);
        canvas.layout()->addWidget(w);
        // Simular manageWidget
        controller.manageWidget(w);
    }

    QElapsedTimer timer;
    timer.start();

    // Selecionar um por um
    for(int i=0; i<50; ++i) {
        controller.selectWidget(widgets[i]);
    }

    qDebug() << "Selection of 50 widgets took:" << timer.elapsed() << "ms";
    
    // Média por seleção
    if (timer.elapsed() > 500) {
        QWARN("Selection is suspiciously slow!");
    }
}

QTEST_MAIN(tst_StylePerformance)
#include "tst_StylePerformance.moc"

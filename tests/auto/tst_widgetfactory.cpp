#include <QtTest>
#include "widget_factory.h"
#include <QPushButton>
#include <QLabel>

class TestWidgetFactory : public QObject
{
    Q_OBJECT

private slots:
    void testCreateButton();
    void testCreateLabel();
};

void TestWidgetFactory::testCreateButton()
{
    // This should fail because currently it returns nullptr
    QWidget* btn = WidgetFactory::create("button");
    QVERIFY(btn != nullptr);
    
    // If it were not null, we would check type
    if (btn) {
        QCOMPARE(QString(btn->metaObject()->className()), QString("QPushButton"));
        delete btn;
    }
}

void TestWidgetFactory::testCreateLabel()
{
    QWidget* lbl = WidgetFactory::create("label");
    QVERIFY(lbl != nullptr);
    if (lbl) {
        QCOMPARE(QString(lbl->metaObject()->className()), QString("QLabel"));
        delete lbl;
    }
}

QTEST_MAIN(TestWidgetFactory)
#include "tst_widgetfactory.moc"

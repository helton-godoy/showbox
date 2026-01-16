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
    QWidget* btn = WidgetFactory::create("button");
    QVERIFY(btn != nullptr);
    
    if (btn) {
        // Updated to expect the refactored PushButtonWidget
        QCOMPARE(QString(btn->metaObject()->className()), QString("PushButtonWidget"));
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
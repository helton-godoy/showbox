#include <QtTest>
#include <ParserMain.h>
#include <IShowboxBuilder.h>

// Mock Builder
class MockBuilder : public IShowboxBuilder {
public:
    PushButtonWidget* buildPushButton(const QString &title, const QString &name) override {
        lastTitle = title;
        lastName = name;
        called = true;
        return nullptr;
    }
    QString lastTitle;
    QString lastName;
    bool called = false;
};

class TestParser : public QObject
{
    Q_OBJECT

private slots:
    void testParseAddPushButton();
};

void TestParser::testParseAddPushButton()
{
    MockBuilder builder;
    ParserMain parser(&builder);
    
    parser.processLine("add pushbutton \"Click Me\" btn_test");
    
    QVERIFY(builder.called);
    QCOMPARE(builder.lastTitle, QString("Click Me"));
    QCOMPARE(builder.lastName, QString("btn_test"));
}

QTEST_MAIN(TestParser)
#include "tst_parser.moc"


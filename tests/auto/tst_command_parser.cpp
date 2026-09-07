#include <QtTest>
#include <CommandParser.h>
#include <Tokenizer.h>

class TestCommandParser : public QObject
{
    Q_OBJECT

private slots:
    void testCommandDispatch();
    void testUnknownCommand();
};

void TestCommandParser::testCommandDispatch()
{
    CommandParser parser;
    bool called = false;
    QStringList receivedArgs;

    parser.registerCommand("add", [&](const QStringList & args) {
        called = true;
        receivedArgs = args;
    });

    parser.parseLine("add pushbutton \"Test Button\"");
    
    QVERIFY(called);
    QCOMPARE(receivedArgs.size(), 2);
    QCOMPARE(receivedArgs[0], QString("pushbutton"));
    QCOMPARE(receivedArgs[1], QString("Test Button"));
}

void TestCommandParser::testUnknownCommand()
{
    CommandParser parser;
    bool called = false;

    parser.registerCommand("add", [&](const QStringList &) {
        called = true;
    });

    parser.parseLine("invalid command here");
    
    QVERIFY(!called);
}

QTEST_MAIN(TestCommandParser)
#include "tst_command_parser.moc"

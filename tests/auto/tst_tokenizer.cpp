#include <QtTest>
#include <Tokenizer.h>

class TestTokenizer : public QObject
{
    Q_OBJECT

private slots:
    void testBasicSplitting();
    void testQuotedStrings();
    void testEscapedCharacters();
    void testEmptyAndWhitespace();
    void testComplexCommands();
    void testSingleQuotesAndNewline();
};

void TestTokenizer::testBasicSplitting()
{
    Tokenizer tokenizer("add label lbl1");
    QStringList tokens = tokenizer.tokenize();
    
    QCOMPARE(tokens.size(), 3);
    QCOMPARE(tokens[0], QString("add"));
    QCOMPARE(tokens[1], QString("label"));
    QCOMPARE(tokens[2], QString("lbl1"));
}

void TestTokenizer::testQuotedStrings()
{
    Tokenizer tokenizer("add button \"Click Me Now\" btn1");
    QStringList tokens = tokenizer.tokenize();
    
    QCOMPARE(tokens.size(), 4);
    QCOMPARE(tokens[0], QString("add"));
    QCOMPARE(tokens[1], QString("button"));
    QCOMPARE(tokens[2], QString("Click Me Now"));
    QCOMPARE(tokens[3], QString("btn1"));
}

void TestTokenizer::testEscapedCharacters()
{
    // String: add label "Name: \"Showbox\"" lbl
    Tokenizer tokenizer("add label \"Name: \\\"Showbox\\\"\" lbl");
    QStringList tokens = tokenizer.tokenize();
    
    QCOMPARE(tokens.size(), 4);
    QCOMPARE(tokens[2], QString("Name: \"Showbox\""));
}

void TestTokenizer::testEmptyAndWhitespace()
{
    Tokenizer tokenizer("   add    button    \"\"   ");
    QStringList tokens = tokenizer.tokenize();
    
    QCOMPARE(tokens.size(), 3);
    QCOMPARE(tokens[0], QString("add"));
    QCOMPARE(tokens[1], QString("button"));
    QCOMPARE(tokens[2], QString(""));
}

void TestTokenizer::testComplexCommands()
{
    Tokenizer tokenizer("add checkbox \"Turbo \\\"Extreme\\\"\" chk1 checked enabled");
    QStringList tokens = tokenizer.tokenize();
    
    QCOMPARE(tokens.size(), 6);
    QCOMPARE(tokens[2], QString("Turbo \"Extreme\""));
    QCOMPARE(tokens[5], QString("enabled"));
}

void TestTokenizer::testSingleQuotesAndNewline()
{
    Tokenizer tokenizer("add label 'first\\nsecond' note");
    const QStringList tokens = tokenizer.tokenize();
    QCOMPARE(tokens.size(), 4);
    QCOMPARE(tokens[2], QString("first\nsecond"));
}

QTEST_MAIN(TestTokenizer)
#include "tst_tokenizer.moc"

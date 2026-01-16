#include <QtTest>
#include <ParserMain.h>
#include <IShowboxBuilder.h>
#include <QWidget>

// Mock Builder
class MockBuilder : public IShowboxBuilder {
public:
    PushButtonWidget* buildPushButton(const QString &title, const QString &name) override {
        lastTitle = title;
        lastName = name;
        called = true;
        return nullptr;
    }
    
    // Stubs for new interface to allow instantiation
    QWidget* buildWindow(const Showbox::Models::WindowConfig& config) override { Q_UNUSED(config); return nullptr; }
    QWidget* buildButton(const Showbox::Models::ButtonConfig& config) override { Q_UNUSED(config); return nullptr; }
    QWidget* buildLabel(const Showbox::Models::LabelConfig& config) override { Q_UNUSED(config); return nullptr; }
    QLayout* buildLayout(const Showbox::Models::LayoutConfig& config) override { Q_UNUSED(config); return nullptr; }

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
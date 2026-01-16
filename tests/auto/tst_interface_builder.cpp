#include <QtTest>
#include <IShowboxBuilder.h>
#include <WidgetConfigs.h>
#include <QWidget>

// Mock implementation to verify interface signature
class MockBuilder : public IShowboxBuilder {
public:
    QWidget* buildWindow(const Showbox::Models::WindowConfig& config) override {
        Q_UNUSED(config);
        return nullptr;
    }
    
    QWidget* buildButton(const Showbox::Models::ButtonConfig& config) override {
        Q_UNUSED(config);
        return nullptr;
    }
    
    QWidget* buildLabel(const Showbox::Models::LabelConfig& config) override {
        Q_UNUSED(config);
        return nullptr;
    }

    QWidget* buildLineEdit(const Showbox::Models::LineEditConfig& config) override {
        Q_UNUSED(config);
        return nullptr;
    }

    QLayout* buildLayout(const Showbox::Models::LayoutConfig& config) override {
        Q_UNUSED(config);
        return nullptr;
    }

    // Legacy support
    PushButtonWidget* buildPushButton(const QString &title, const QString &name) override {
        Q_UNUSED(title);
        Q_UNUSED(name);
        return nullptr;
    }
};

class TestInterfaceBuilder : public QObject
{
    Q_OBJECT
private slots:
    void testInterfaceExistence();
};

void TestInterfaceBuilder::testInterfaceExistence()
{
    MockBuilder builder;
    QVERIFY(true);
}

QTEST_MAIN(TestInterfaceBuilder)
#include "tst_interface_builder.moc"
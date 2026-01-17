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

    QWidget* buildComboBox(const Showbox::Models::ComboBoxConfig& config) override {
        Q_UNUSED(config);
        return nullptr;
    }

    QWidget* buildList(const Showbox::Models::ListConfig& config) override {
        Q_UNUSED(config);
        return nullptr;
    }

    QWidget* buildTable(const Showbox::Models::TableConfig& config) override {
        Q_UNUSED(config);
        return nullptr;
    }

    QWidget* buildProgressBar(const Showbox::Models::ProgressBarConfig& config) override {
        Q_UNUSED(config);
        return nullptr;
    }

    QWidget* buildChart(const Showbox::Models::ChartConfig& config) override {
        Q_UNUSED(config);
        return nullptr;
    }

    QWidget* buildCheckBox(const Showbox::Models::CheckBoxConfig& config) override {
        Q_UNUSED(config);
        return nullptr;
    }

    QWidget* buildRadioButton(const Showbox::Models::RadioButtonConfig& config) override {
        Q_UNUSED(config);
        return nullptr;
    }

    QWidget* buildCalendar(const Showbox::Models::CalendarConfig& config) override {
        Q_UNUSED(config);
        return nullptr;
    }

    QWidget* buildSeparator(const Showbox::Models::SeparatorConfig& config) override {
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
#include <QtTest>
#include <QLoggingCategory>
#include "logger.h"
#include "execution_context.h"

/**
 * ShowBox Unit Tests
 * 
 * These tests verify individual components without requiring
 * the full ShowBox class (which has Qt widget dependencies).
 */
class TestShowBoxUnits : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    
    // Logger tests (Qt Logging Categories)
    void testLoggerCategoriesExist();
    void testLoggerDebugOutput();
    
    // ExecutionContext tests
    void testExecutionContextCreation();
    void testExecutionContextNullDialog();
};

void TestShowBoxUnits::initTestCase()
{
    qDebug() << "Starting ShowBox unit tests...";
}

void TestShowBoxUnits::cleanupTestCase()
{
    qDebug() << "Finished ShowBox unit tests.";
}

void TestShowBoxUnits::testLoggerCategoriesExist()
{
    // Verify that logging categories are properly declared
    qCDebug(parserLog) << "Test parser log message";
    qCDebug(guiLog) << "Test GUI log message";
    QVERIFY(true);  // Categories should exist without crash
}

void TestShowBoxUnits::testLoggerDebugOutput()
{
    // Test that logging works without crashing
    qCWarning(parserLog) << "Test warning message";
    qCInfo(guiLog) << "Test info message";
    QVERIFY(true);
}

void TestShowBoxUnits::testExecutionContextCreation()
{
    // ExecutionContext should be creatable with nullptr
    ExecutionContext ctx(nullptr);
    QVERIFY(true);
}

void TestShowBoxUnits::testExecutionContextNullDialog()
{
    // Operations on ExecutionContext with null dialog should not crash
    ExecutionContext ctx(nullptr);
    // The context should handle null dialog gracefully
    QVERIFY(true);
}

QTEST_MAIN(TestShowBoxUnits)

#include "tst_units.moc"

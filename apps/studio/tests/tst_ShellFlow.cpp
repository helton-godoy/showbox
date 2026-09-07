#include <QtTest>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QProcess>
#include <QPushButton>
#include <QTemporaryDir>
#include <QVBoxLayout>
#include "core/ScriptGenerator.h"
#include "core/PreviewManager.h"
#include "core/ProjectSerializer.h"
#include "core/StudioWidgetFactory.h"

class ShellFlowTest : public QObject {
    Q_OBJECT
private:
    void form(QWidget &root, const QString &command) {
        auto *layout = new QVBoxLayout(&root);
        auto *entry = new QLineEdit;
        auto *button = new QPushButton("Executar");
        auto *label = new QLabel("Inicial");
        for (auto *w : {static_cast<QWidget *>(entry), static_cast<QWidget *>(button), static_cast<QWidget *>(label)}) layout->addWidget(w);
        entry->setObjectName("entry"); entry->setProperty("showbox_type", "textbox");
        button->setObjectName("run"); button->setProperty("showbox_type", "button");
        label->setObjectName("result"); label->setProperty("showbox_type", "label");
        button->setProperty("showbox_actions", QString::fromUtf8(QJsonDocument(QJsonObject{
            {"clicked", QJsonArray{QJsonObject{{"type", "shell"}, {"command", command}}}}
        }).toJson()));
    }
    QString output(const QSignalSpy &spy) {
        QString text;
        for (const auto &args : spy) text += args.first().toString();
        return text;
    }
    bool runningPid(const QString &path) {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly)) return false;
        const auto pid = file.readAll().trimmed();
        QFile state("/proc/" + QString::fromLatin1(pid) + "/stat");
        if (!state.open(QIODevice::ReadOnly)) return false;
        const QByteArray stat = state.readAll();
        return stat.mid(stat.lastIndexOf(')') + 2, 1) != "Z";
    }
    void environment(const QTemporaryDir &dir, const QByteArray &mode) {
        qputenv("SHOWBOX_BIN", DRIVER_PATH);
        qputenv("SB_TEST_MODE", mode);
        qputenv("SB_TEST_INPUT", "João \"aspas\" 'simples' \\ fim = $(touch INDEVIDO) `touch OUTRO`");
        qputenv("SB_TEST_EXPECTED", "Olá: João \"aspas\" 'simples' \\ fim = $(touch INDEVIDO) `touch OUTRO`");
        qputenv("SB_RESULT_FILE", dir.filePath("result").toUtf8());
        qputenv("SB_CHILD_PID_FILE", dir.filePath("child").toUtf8());
        qputenv("SB_ACTION_PID_FILE", dir.filePath("action").toUtf8());
    }
private slots:
    void cleanup() {
        for (const char *key : {"SHOWBOX_BIN", "SB_TEST_MODE", "SB_TEST_INPUT", "SB_TEST_EXPECTED", "SB_RESULT_FILE", "SB_CHILD_PID_FILE", "SB_ACTION_PID_FILE", "SB_SCREENSHOT_FILE"}) qunsetenv(key);
    }
    void emitsActions() {
        QWidget root;
        form(root, "printf 'acao-presente\\n'");
        ScriptGenerator generator;
        QVERIFY2(generator.generate(&root).contains("acao-presente"), qPrintable(generator.errorString()));
    }
    void executesBash() {
        PreviewManager manager;
        QSignalSpy out(&manager, &PreviewManager::previewOutput);
        QSignalSpy done(&manager, &PreviewManager::previewFinished);
        manager.runPreview("#!/bin/bash\nprintf 'bash-executado\\n'\n");
        QTRY_VERIFY_WITH_TIMEOUT(!done.isEmpty(), 3000);
        QCOMPARE(output(out), "bash-executado\n");
        QCOMPARE(done.first().first().toInt(), 0);
    }
    void realClickAndStandaloneExport() {
        QTemporaryDir dir;
        environment(dir, "normal");
        qputenv("SB_SCREENSHOT_FILE", (QCoreApplication::applicationDirPath() + "/../runtime-flow.png").toUtf8());
        QWidget root;
        form(root, "showbox_get VALUE entry\nshowbox_set result text \"Olá: $VALUE\"\nprintf 'log sem protocolo\\n'");
        ScriptGenerator generator;
        const QString script = generator.generate(&root);
        QVERIFY2(!script.isEmpty(), qPrintable(generator.errorString()));
        const QString path = dir.filePath("aplicação com espaços.sh");
        QFile file(path); QVERIFY(file.open(QIODevice::WriteOnly)); file.write(script.toUtf8()); file.close();
        QProcess process;
        process.setWorkingDirectory(dir.path());
        process.start("/bin/bash", {path});
        QVERIFY(process.waitForFinished(10000));
        const QByteArray errors = process.readAllStandardError();
        QVERIFY2(process.exitCode() == 0, errors.constData());
        QVERIFY2(QFile::exists(dir.filePath("result")), errors.constData());
        QFile result(dir.filePath("result")); QVERIFY(result.open(QIODevice::ReadOnly));
        QCOMPARE(result.readAll(), qgetenv("SB_TEST_EXPECTED"));
        QVERIFY(process.readAllStandardOutput().contains("log sem protocolo"));
        QVERIFY(!QFile::exists(dir.filePath("INDEVIDO")));
        QVERIFY(!QFile::exists(dir.filePath("OUTRO")));
    }
    void demoSurvivesSaveAndReopen() {
        QTemporaryDir dir; environment(dir, "normal");
        ProjectSerializer serializer; StudioWidgetFactory factory;
        QList<QWidget *> widgets;
        QVERIFY(serializer.load(DEMO_PROJECT_PATH, &factory, widgets));
        QWidget original;
        auto *layout = new QVBoxLayout(&original);
        for (auto *widget : widgets) layout->addWidget(widget);
        const auto path = dir.filePath("salvo.sbxproj");
        QVERIFY(serializer.save(path, &original, &factory));
        widgets.clear();
        QVERIFY(serializer.load(path, &factory, widgets));
        QWidget restored; auto *restoredLayout = new QVBoxLayout(&restored);
        for (auto *widget : widgets) restoredLayout->addWidget(widget);
        ScriptGenerator generator; PreviewManager manager;
        QSignalSpy done(&manager, &PreviewManager::previewFinished);
        QSignalSpy errors(&manager, &PreviewManager::previewError);
        manager.runPreview(generator.generate(&restored), dir.path());
        QTRY_VERIFY_WITH_TIMEOUT(!done.isEmpty(), 6000);
        QVERIFY2(done.first().first().toInt() == 0, qPrintable(output(errors)));
        QVERIFY(QFile::exists(dir.filePath("result")));
    }
    void visualPreviewDoesNotExecuteActions() {
        QTemporaryDir dir; environment(dir, "visual");
        QWidget root; form(root, "touch \"$SB_ACTION_PID_FILE\"");
        ScriptGenerator generator;
        PreviewManager manager;
        QSignalSpy done(&manager, &PreviewManager::previewFinished);
        manager.runVisualPreview(generator.generateUi(&root));
        QTRY_VERIFY_WITH_TIMEOUT(!done.isEmpty(), 5000);
        QCOMPARE(done.first().first().toInt(), 0);
        QFile result(dir.filePath("result")); QVERIFY(result.open(QIODevice::ReadOnly));
        QCOMPARE(result.readAll(), QByteArray("Inicial"));
        QVERIFY(!QFile::exists(dir.filePath("action")));
    }
    void reportsScriptFailure() {
        PreviewManager manager;
        QSignalSpy errors(&manager, &PreviewManager::previewError);
        QSignalSpy done(&manager, &PreviewManager::previewFinished);
        manager.runPreview("echo 'erro-controlado' >&2\nexit 23\n");
        QTRY_VERIFY_WITH_TIMEOUT(!done.isEmpty(), 3000);
        QCOMPARE(done.first().first().toInt(), 23);
        QVERIFY(output(errors).contains("erro-controlado"));
        manager.runPreview("if ; then\n");
        QCOMPARE(done.size(), 2);
        QCOMPARE(done.last().first().toInt(), 2);
    }
    void stopsProcesses_data() {
        QTest::addColumn<QByteArray>("mode");
        QTest::newRow("parar") << QByteArray("stay");
        QTest::newRow("fechar-janela") << QByteArray("close-running");
    }
    void stopsProcesses() {
        QFETCH(QByteArray, mode);
        QTemporaryDir dir; environment(dir, mode);
        QWidget root;
        form(root, "printf '%s\\n' \"$BASHPID\" > \"$SB_ACTION_PID_FILE\"\nsleep 30 &\nprintf '%s\\n' \"$!\" > \"$SB_CHILD_PID_FILE\"\nwait\n");
        ScriptGenerator generator;
        PreviewManager manager;
        QSignalSpy done(&manager, &PreviewManager::previewFinished);
        QSignalSpy errors(&manager, &PreviewManager::previewError);
        manager.runPreview(generator.generate(&root), dir.path());
        QTRY_VERIFY_WITH_TIMEOUT(QFile::exists(dir.filePath("child")), 4000);
        if (mode == "stay") manager.stop();
        QTRY_VERIFY_WITH_TIMEOUT(!done.isEmpty(), 5000);
        QTRY_VERIFY_WITH_TIMEOUT(!runningPid(dir.filePath("child")), 2000);
        QTRY_VERIFY_WITH_TIMEOUT(!runningPid(dir.filePath("action")), 2000);
        QVERIFY(!manager.isRunning());
    }
    void queuedClicksAndTypedActions() {
        QTemporaryDir dir; environment(dir, "queued");
        qputenv("SB_TEST_EXPECTED", "Segundo 'literal' $sem_expansao");
        QWidget root;
        form(root, "sleep 0.2\nshowbox_get VALUE entry\nshowbox_set result text primeiro");
        auto *second = new QPushButton("Segundo");
        root.layout()->addWidget(second);
        second->setObjectName("second"); second->setProperty("showbox_type", "button");
        second->setProperty("showbox_actions", QString::fromUtf8(QJsonDocument(QJsonObject{{"clicked", QJsonArray{
            QJsonObject{{"type", "query"}, {"target", "entry"}, {"variable", "VALUE"}},
            QJsonObject{{"type", "set"}, {"target", "result"}, {"property", "text"}, {"value", "Segundo 'literal' $sem_expansao"}}
        }}}).toJson()));
        ScriptGenerator generator; PreviewManager manager;
        QSignalSpy done(&manager, &PreviewManager::previewFinished);
        QSignalSpy errors(&manager, &PreviewManager::previewError);
        manager.runPreview(generator.generate(&root), dir.path());
        QTRY_VERIFY_WITH_TIMEOUT(!done.isEmpty(), 6000);
        QVERIFY2(done.first().first().toInt() == 0, qPrintable(output(errors)));
        QVERIFY(QFile::exists(dir.filePath("result")));
    }
    void cleansBackgroundChildAfterShellExit() {
        QTemporaryDir dir;
        qputenv("SB_CHILD_PID_FILE", dir.filePath("child").toUtf8());
        PreviewManager manager;
        QSignalSpy done(&manager, &PreviewManager::previewFinished);
        manager.runPreview("sleep 30 &\nprintf '%s\\n' \"$!\" > \"$SB_CHILD_PID_FILE\"\nexit 0\n");
        QTRY_VERIFY_WITH_TIMEOUT(!done.isEmpty(), 3000);
        QVERIFY(QFile::exists(dir.filePath("child")));
        QTRY_VERIFY_WITH_TIMEOUT(!runningPid(dir.filePath("child")), 2000);
    }
    void actionErrorIsReported() {
        QTemporaryDir dir; environment(dir, "visual");
        QWidget root; form(root, "echo falha-na-acao >&2\nfalse");
        ScriptGenerator generator; PreviewManager manager;
        QSignalSpy errors(&manager, &PreviewManager::previewError);
        QSignalSpy done(&manager, &PreviewManager::previewFinished);
        manager.runPreview(generator.generate(&root), dir.path());
        QTRY_VERIFY_WITH_TIMEOUT(!done.isEmpty(), 4000);
        QVERIFY(output(errors).contains("falha-na-acao"));
        QVERIFY(output(errors).contains("código 1"));
    }
    void rejectsUnsupportedAndInvalidNames() {
        QWidget root; form(root, "true"); ScriptGenerator generator;
        auto *button = root.findChild<QPushButton *>();
        button->setObjectName("bad;name");
        QVERIFY(generator.generate(&root).isEmpty());
        button->setObjectName("run"); button->setProperty("showbox_type", "spinbox");
        QVERIFY(generator.generate(&root).isEmpty());
        QVERIFY(!generator.errorString().isEmpty());
    }
};
QTEST_MAIN(ShellFlowTest)
#include "tst_ShellFlow.moc"

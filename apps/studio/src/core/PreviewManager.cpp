#include "PreviewManager.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QPointer>
#include <QStandardPaths>
#include <QTimer>
#include <csignal>
#include <unistd.h>

PreviewManager::PreviewManager(QObject *parent) : QObject(parent) {}

PreviewManager::~PreviewManager() {
    if (m_process) {
        disconnect(m_process, nullptr, this, nullptr);
        if (m_process->state() == QProcess::Starting) m_process->waitForStarted(1000);
        const auto pid = m_process->processId();
        if (pid > 0) ::kill(-pid, SIGTERM);
        if (!m_process->waitForFinished(1000)) {
            if (pid > 0) ::kill(-pid, SIGKILL);
            m_process->kill();
            m_process->waitForFinished(1000);
        }
    }
}

void PreviewManager::runPreview(const QString &script, const QString &directory) {
    start(script, true, directory);
}
void PreviewManager::runVisualPreview(const QString &commands) {
    start(commands, false, {});
}

void PreviewManager::start(const QString &content, bool shell, const QString &directory) {
    if (isRunning()) { emit previewError("Pare a execução atual antes de iniciar outra."); return; }
    QString executable = qEnvironmentVariable("SHOWBOX_BIN");
    if (executable.isEmpty()) {
        executable = QCoreApplication::applicationDirPath() + "/showbox";
        if (!QFileInfo(executable).isExecutable()) executable = QStandardPaths::findExecutable("showbox");
    } else if (!executable.contains('/')) {
        executable = QStandardPaths::findExecutable(executable);
    }
    if (executable.isEmpty() || !QFileInfo(executable).isExecutable()) {
        emit previewError("Motor showbox não encontrado. Configure SHOWBOX_BIN ou instale-o ao lado do Studio.");
        emit previewFinished(127); return;
    }
    executable = QFileInfo(executable).absoluteFilePath();
    m_temp = std::make_unique<QTemporaryDir>();
    QFile file(m_temp->filePath(shell ? "application.sh" : "interface.txt"));
    if (!m_temp->isValid() || !file.open(QIODevice::WriteOnly) ||
        file.write(content.toUtf8()) != content.toUtf8().size()) {
        m_temp.reset();
        emit previewError("Não foi possível preparar o arquivo temporário.");
        emit previewFinished(74); return;
    }
    const QString path = file.fileName();
    file.close();
    if (shell) {
        QProcess syntax;
        syntax.start("/bin/bash", {"-n", path});
        if (!syntax.waitForFinished(2000) || syntax.exitCode() != 0) {
            syntax.kill(); syntax.waitForFinished(1000);
            emit previewError("Erro de sintaxe Bash:\n" + QString::fromUtf8(syntax.readAllStandardError()));
            m_temp.reset(); emit previewFinished(2); return;
        }
    }
    m_group = 0;
    m_stopping = false;
    m_process = new QProcess(this);
    connect(m_process, &QProcess::started, this, [this] {
        m_group = m_process->processId();
        if (m_stopping) stop();
    });
    m_process->setChildProcessModifier([] { if (::setsid() < 0) ::_exit(127); });
    auto environment = QProcessEnvironment::systemEnvironment();
    environment.insert("SHOWBOX_BIN", executable);
    m_process->setProcessEnvironment(environment);
    if (!directory.isEmpty()) m_process->setWorkingDirectory(directory);
    connect(m_process, &QProcess::readyReadStandardOutput, this, [this] {
        emit previewOutput(QString::fromUtf8(m_process->readAllStandardOutput()));
    });
    connect(m_process, &QProcess::readyReadStandardError, this, [this] {
        emit previewError(QString::fromUtf8(m_process->readAllStandardError()));
    });
    connect(m_process, &QProcess::errorOccurred, this, [this](QProcess::ProcessError error) {
        if (error == QProcess::FailedToStart) {
            emit previewError(m_process->errorString()); finish(127);
        }
    });
    connect(m_process, qOverload<int, QProcess::ExitStatus>(&QProcess::finished), this,
            [this](int code, QProcess::ExitStatus status) { finish(status == QProcess::CrashExit ? 128 : code); });
    emit runningChanged(true);
    if (shell) m_process->start("/bin/bash", {path});
    else {
        m_process->setStandardInputFile(path);
        m_process->start(executable, {"--hidden"});
    }
}

void PreviewManager::stop() {
    if (!m_process) return;
    m_stopping = true;
    const qint64 pid = m_process->processId();
    if (pid > 0) ::kill(-pid, SIGTERM);
    QPointer<QProcess> process(m_process);
    QTimer::singleShot(500, this, [process, pid] {
        if (process && process->state() != QProcess::NotRunning) {
            if (pid > 0) ::kill(-pid, SIGKILL);
            process->kill();
        }
    });
}

void PreviewManager::finish(int code) {
    if (!m_process) return;
    emit previewOutput(QString::fromUtf8(m_process->readAllStandardOutput()));
    emit previewError(QString::fromUtf8(m_process->readAllStandardError()));
    if (m_group > 0) { ::kill(-m_group, SIGTERM); ::kill(-m_group, SIGKILL); }
    m_group = 0;
    m_process->deleteLater();
    m_process = nullptr;
    m_temp.reset();
    emit runningChanged(false);
    emit previewFinished(code);
}

#include "PreviewManager.h"
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QProcess>
#include <QTemporaryFile>

PreviewManager::PreviewManager(QObject *parent) : QObject(parent) {}

void PreviewManager::runPreview(const QString &scriptContent) {
  QTemporaryFile tempFile;
  if (!tempFile.open()) {
    emit previewError("Failed to create temporary file for preview.");
    return;
  }

  tempFile.write(scriptContent.toUtf8());
  tempFile.flush();
  tempFile.setAutoRemove(false);
  QString tempPath = tempFile.fileName();
  tempFile.close();

  QProcess *process = new QProcess(this);

  // Assumes running from 'build' directory adjacent to sourcedir or usual cmake
  // build Try to find the executable in likely locations
  QString executablePath = "../showbox/build/bin/showbox";

  // Verify absolute path resolution from current working directory
  QString absPath = QDir::current().absoluteFilePath(executablePath);

  if (!QFile::exists(absPath)) {
    // Fallback: try to find relative to the application dir if not running from
    // build root
    QDir appDir(QCoreApplication::applicationDirPath());
    // build/bin -> ../../../showbox/build/bin (approximate guess based on
    // typical nesting) Let's rely on the user running from 'build' as per
    // instructions for now, but log path.
    emit previewError("Executable not found at: " + absPath);
    QFile::remove(tempPath);
    process->deleteLater();
    return;
  }

  connect(process, &QProcess::readyReadStandardOutput, this, [this, process]() {
    emit previewOutput(process->readAllStandardOutput());
  });

  connect(process, &QProcess::readyReadStandardError, this, [this, process]() {
    emit previewError(process->readAllStandardError());
  });

  connect(process,
          QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
          [this, process, tempPath](int exitCode, QProcess::ExitStatus status) {
            if (status == QProcess::CrashExit) {
              emit previewError(
                  "Process Crashed (Segmentation Fault or Aborted).");
              emit previewFinished(139); // Typical SIGSEGV code
            } else {
              emit previewFinished(exitCode);
            }

            QFile::remove(tempPath);
            process->deleteLater();
          });

  // poc_modern_cli reads from stdin
  process->setStandardInputFile(tempPath);
  process->start(absPath, QStringList() << "--keep-open");
}

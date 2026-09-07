#ifndef PREVIEWMANAGER_H
#define PREVIEWMANAGER_H
#include <QObject>
#include <QProcess>
#include <QTemporaryDir>
#include <memory>

class PreviewManager : public QObject {
    Q_OBJECT
public:
    explicit PreviewManager(QObject *parent = nullptr);
    ~PreviewManager() override;
    void runPreview(const QString &scriptContent, const QString &workingDirectory = {});
    void runVisualPreview(const QString &commands);
    void stop();
    bool isRunning() const { return m_process != nullptr; }
signals:
    void previewOutput(const QString &output);
    void previewError(const QString &error);
    void previewFinished(int exitCode);
    void runningChanged(bool running);
private:
    void start(const QString &content, bool shell, const QString &workingDirectory);
    void finish(int code);
    QProcess *m_process = nullptr;
    qint64 m_group = 0;
    bool m_stopping = false;
    std::unique_ptr<QTemporaryDir> m_temp;
};
#endif

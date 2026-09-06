#ifndef PREVIEWMANAGER_H
#define PREVIEWMANAGER_H

#include <QObject>
#include <QString>

class PreviewManager : public QObject {
  Q_OBJECT
public:
  explicit PreviewManager(QObject *parent = nullptr);
  void runPreview(const QString &scriptContent);

signals:
  void previewOutput(const QString &output);
  void previewError(const QString &error);
  void previewFinished(int exitCode);
};

#endif // PREVIEWMANAGER_H

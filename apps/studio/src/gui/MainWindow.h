#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJsonArray>
#include <QJsonObject>

class Canvas;
class IStudioWidgetFactory;
class ObjectInspector;
class PropertyEditor;
class ActionEditor;
class StudioController;
class ScriptGenerator;
class PreviewManager;
class AbstractToolbox;
class QTextEdit;
class QDockWidget;
class QActionGroup;
class QTabWidget;
class QCloseEvent;

class MainWindow : public QMainWindow {
  Q_OBJECT
public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  bool hasUnsavedChanges() const;
  void markDocumentSaved();

  // Facade estável da Showbox Studio Automation Interface. Essas operações
  // devolvem dados/erros públicos e não expõem objetos QWidget ao protocolo.
  QJsonObject automationProjectSnapshot() const;
  QJsonObject automationUiTree() const;
  QJsonArray automationDiagnostics() const;
  QJsonObject automationExportValidate() const;
  QString automationPreviewLogs() const { return m_automationPreviewLogs; }
  bool automationPreviewRunning() const;
  bool automationNew(QString *error = nullptr);
  bool automationOpen(const QString &fileName, QString *error = nullptr);
  bool automationSave(const QString &fileName, QString *error = nullptr);
  bool automationAddWidget(const QString &type, const QString &name,
                           const QString &parentName, QString *error = nullptr);
  bool automationRemoveWidget(const QString &name, QString *error = nullptr);
  bool automationSelectWidget(const QString &name, QString *error = nullptr);
  bool automationMoveWidget(const QString &name, const QString &parentName,
                            int index, QString *error = nullptr);
  bool automationSetProperty(const QString &name, const QString &property,
                             const QJsonValue &value, QString *error = nullptr);
  bool automationSetActions(const QString &name, const QJsonObject &actions,
                            QString *error = nullptr);
  bool automationUndo(QString *error = nullptr);
  bool automationRedo(QString *error = nullptr);
  bool automationStartPreview(QString *error = nullptr);
  bool automationStopPreview(QString *error = nullptr);
  bool automationExport(const QString &fileName, QJsonObject *result = nullptr,
                        QString *error = nullptr);

protected:
  void closeEvent(QCloseEvent *event) override;

private slots:
  void onNewClicked();
  void onRunClicked();
  void onVisualPreview();
  void onExportClicked();
  void onDemoClicked();
  void onSaveClicked();
  void onOpenClicked();
  void onDeleteClicked();
  void onGroupRequested(const QString &containerType);
  void onToolboxStyleChanged(int style);
  void onAddPageRequested(QWidget *tabs);
  void onRemovePageRequested(QWidget *tabs);


private:
  void setupUI();
  void createSampleWidgets();
  void createToolbox(int style);
  void populateToolbox(AbstractToolbox *toolbox);
  bool confirmDiscardIfModified();
  void onUndoIndexChanged();

  Canvas *m_canvas;
  IStudioWidgetFactory *m_factory;
  ObjectInspector *m_inspector;
  StudioController *m_controller;
  PropertyEditor *m_propEditor;
  ActionEditor *m_actionEditor;

  // Toolbox
  QDockWidget *m_dockToolbox;
  AbstractToolbox *m_toolbox;
  int m_toolboxStyle; // 0 = Classic, 1 = Tree

  QString m_projectDirectory;
  bool m_actionsModified = false;

  // Live Preview
  PreviewManager *m_previewManager;
  QTextEdit *m_previewLog;
  QString m_automationPreviewLogs;
};

#endif // MAINWINDOW_H

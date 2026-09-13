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
class QUndoCommand;

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
  bool automationNew(bool force, QString *error);
  bool automationOpen(const QString &fileName, QString *error = nullptr);
  bool automationOpen(const QString &fileName, bool force, QString *error);
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

signals:
  void automationEvent(const QString &name, const QJsonObject &data);

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
  // Empilha com atribuição de origem para o project.changed do caminho comum:
  // o QUndoStack emite indexChanged de forma síncrona, então o handler lê e
  // consome o marcador antes do retorno. Operações da GUI empilham direto
  // (marcador vazio → source "gui").
  void pushUndoCommand(QUndoCommand *cmd, const QString &operation);

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
  // Operação de automação em voo para o project.changed do caminho comum.
  // Consumido e limpo sincronamente pelo indexChanged de cada push/undo/redo.
  QString m_pendingStackOperation;
  // Transação de documento (new/open/demo): suprime SOMENTE o project.changed
  // do caminho comum durante a reconstrução. Os sinais nativos do QUndoStack
  // (indexChanged, cleanChanged, canUndo/RedoChanged) continuam fluindo, de
  // modo que dirty e as QActions de Undo/Redo se atualizam. O project.changed
  // sai explicitamente após o estado final instalado (snapshot consistente).
  bool m_suppressProjectChanged = false;
  bool saveProjectTo(const QString &fileName, const QString &source,
                     QString *error);

  // Live Preview
  PreviewManager *m_previewManager;
  QTextEdit *m_previewLog;
  QString m_automationPreviewLogs;
};

#endif // MAINWINDOW_H

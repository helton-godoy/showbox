#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

class MainWindow : public QMainWindow {
  Q_OBJECT
public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:
  void onRunClicked();
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

  // Live Preview
  PreviewManager *m_previewManager;
  QTextEdit *m_previewLog;
};

#endif // MAINWINDOW_H

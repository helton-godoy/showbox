#include "MainWindow.h"
#include "ActionEditor.h"
#include "Canvas.h"
#include "ObjectInspector.h"
#include "PropertyEditor.h"
#include "core/PreviewManager.h"
#include "core/ProjectSerializer.h"
#include "core/ScriptGenerator.h"
#include "core/StudioCommands.h"
#include "core/StudioController.h"
#include "core/StudioWidgetFactory.h"
#include "toolbox/ToolboxClassic.h"
#include "toolbox/ToolboxTree.h"
#include <QActionGroup>
#include <QDockWidget>
#include <QFile>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QListWidget>
#include <QMenuBar>
#include <QProcess>
#include <QSettings>
#include <QStatusBar>
#include <QTabWidget>
#include <QTextEdit>
#include <QTextStream>
#include <QTime>
#include <QToolBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_dockToolbox(nullptr), m_toolbox(nullptr),
      m_toolboxStyle(0) {
  setWindowTitle("Showbox Studio");

  // Carregar preferência de estilo do toolbox
  QSettings settings("Showbox", "ShowboxStudio");
  m_toolboxStyle = settings.value("toolboxStyle", 0).toInt();
  resize(1200, 800);

  m_factory = new StudioWidgetFactory();
  m_controller = new StudioController(this);
  m_previewManager = new PreviewManager(this);

  setupUI();
  m_canvas->setController(m_controller);
  m_propEditor->setController(m_controller);
  m_inspector->setController(m_controller);
  createSampleWidgets();

  // Sincronizar UI com Undo/Redo
  connect(m_controller->undoStack(), &QUndoStack::indexChanged, [this]() {
    if (m_controller->selectedWidget()) {
      m_propEditor->setTargetWidget(m_controller->selectedWidget());
    }
    m_inspector->updateHierarchy(m_canvas);
  });

  // Sincronizar seleção: Canvas -> Inspector & Property Editor

  // MenuBar
  QMenu *fileMenu = menuBar()->addMenu("&File");
  fileMenu->addAction("Open", this, &MainWindow::onOpenClicked,
                      QKeySequence::Open);
  fileMenu->addAction("Save", this, &MainWindow::onSaveClicked,
                      QKeySequence::Save);
  fileMenu->addSeparator();
  fileMenu->addAction("Exit", this, &QWidget::close, QKeySequence::Quit);

  QMenu *editMenu = menuBar()->addMenu("&Edit");
  editMenu->addAction(m_controller->undoStack()->createUndoAction(this));
  editMenu->addAction(m_controller->undoStack()->createRedoAction(this));

  // Sincronizar seleção: Canvas -> Inspector & Property Editor & Action Editor
  connect(m_controller, &StudioController::widgetSelected, m_inspector,
          &ObjectInspector::selectItemForWidget);
  connect(m_controller, &StudioController::widgetSelected, m_propEditor,
          &PropertyEditor::setTargetWidget);
  connect(m_controller, &StudioController::widgetSelected, m_actionEditor,
          &ActionEditor::setTargetWidget);

  // Conectar novos widgets do Canvas ao Controller e atualizar árvore
  connect(m_canvas, &Canvas::widgetAdded, m_controller,
          &StudioController::manageWidget);
  connect(m_canvas, &Canvas::widgetSelected, m_controller,
          &StudioController::selectWidget); // Novo
  // NOTA: onWidgetAdded foi removido - updateHierarchy (via indexChanged) já
  // reconstrói a árvore completa, evitando duplicação.

  connect(m_canvas, &Canvas::requestGrouping, this,
          &MainWindow::onGroupRequested);
  connect(m_canvas, &Canvas::requestDelete, this, &MainWindow::onDeleteClicked);
  connect(m_canvas, &Canvas::requestAddPage, this,
          &MainWindow::onAddPageRequested);
  connect(m_canvas, &Canvas::requestRemovePage, this,
          &MainWindow::onRemovePageRequested);

  connect(m_inspector, &ObjectInspector::requestGrouping, this,
          &MainWindow::onGroupRequested);
  connect(m_inspector, &ObjectInspector::requestDelete, this,
          &MainWindow::onDeleteClicked);

  // Bidirecional: Inspector -> Controller
  connect(m_inspector, &ObjectInspector::itemSelected, m_controller,
          &StudioController::selectWidget);

  // Atualizar hierarquia inicial
  m_inspector->updateHierarchy(m_canvas);
}

MainWindow::~MainWindow() { delete m_factory; }

void MainWindow::setupUI() {
  statusBar()->showMessage("Pronto");

  // Toolbar
  QToolBar *toolBar = addToolBar("Main Toolbar");

  QAction *openAction = toolBar->addAction("Open");
  connect(openAction, &QAction::triggered, this, &MainWindow::onOpenClicked);

  QAction *saveAction = toolBar->addAction("Save");
  connect(saveAction, &QAction::triggered, this, &MainWindow::onSaveClicked);

  toolBar->addSeparator();

  // Undo/Redo Actions (via QUndoStack)
  QAction *undoAction =
      m_controller->undoStack()->createUndoAction(this, "Undo");
  undoAction->setShortcut(QKeySequence::Undo);
  undoAction->setIcon(QIcon::fromTheme("edit-undo"));
  toolBar->addAction(undoAction);

  QAction *redoAction =
      m_controller->undoStack()->createRedoAction(this, "Redo");
  redoAction->setShortcut(QKeySequence::Redo);
  redoAction->setIcon(QIcon::fromTheme("edit-redo"));
  toolBar->addAction(redoAction);

  toolBar->addSeparator();

  QAction *runAction = toolBar->addAction("Run Preview");
  runAction->setShortcut(QKeySequence("F5"));
  connect(runAction, &QAction::triggered, this, &MainWindow::onRunClicked);

  // Atalho global para Delete
  QAction *deleteAction = new QAction(this);
  deleteAction->setShortcut(QKeySequence::Delete);
  connect(deleteAction, &QAction::triggered, this,
          &MainWindow::onDeleteClicked);
  addAction(deleteAction);

  // Central Widget: CANVAS
  m_canvas = new Canvas(m_factory, this);
  setCentralWidget(m_canvas);

  // Left Dock: TOOLBOX & INSPECTOR (em abas)
  setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::North);

  QDockWidget *dockInspector = new QDockWidget("Hierarquia", this);
  m_inspector = new ObjectInspector(dockInspector);
  dockInspector->setWidget(m_inspector);
  addDockWidget(Qt::LeftDockWidgetArea, dockInspector);

  // Criar Toolbox com estilo salvo
  m_dockToolbox = new QDockWidget("Toolbox", this);
  createToolbox(m_toolboxStyle);
  addDockWidget(Qt::LeftDockWidgetArea, m_dockToolbox);

  tabifyDockWidget(dockInspector, m_dockToolbox);

  // Menu View para alternar estilo do Toolbox
  QMenu *viewMenu = menuBar()->addMenu("&View");
  QMenu *toolboxStyleMenu = viewMenu->addMenu("Toolbox Style");

  QActionGroup *styleGroup = new QActionGroup(this);
  styleGroup->setExclusive(true);

  QAction *classicAction =
      toolboxStyleMenu->addAction("Classic (Uma aba por vez)");
  classicAction->setCheckable(true);
  classicAction->setChecked(m_toolboxStyle == 0);
  classicAction->setData(0);
  styleGroup->addAction(classicAction);

  QAction *treeAction = toolboxStyleMenu->addAction("Tree (Múltiplas seções)");
  treeAction->setCheckable(true);
  treeAction->setChecked(m_toolboxStyle == 1);
  treeAction->setData(1);
  styleGroup->addAction(treeAction);

  connect(styleGroup, &QActionGroup::triggered, this, [this](QAction *action) {
    onToolboxStyleChanged(action->data().toInt());
  });

  // Right Dock: PROPERTIES & ACTIONS (Tabbed)
  QDockWidget *dockProps = new QDockWidget("Propriedades", this);
  QTabWidget *propsTabWidget = new QTabWidget(dockProps);

  m_propEditor = new PropertyEditor(propsTabWidget);
  m_actionEditor = new ActionEditor(propsTabWidget);
  m_actionEditor->setController(m_controller);

  propsTabWidget->addTab(m_propEditor, "Propriedades");
  propsTabWidget->addTab(m_actionEditor, "Ações");

  dockProps->setWidget(propsTabWidget);
  addDockWidget(Qt::RightDockWidgetArea, dockProps);

  // Bottom Dock: PREVIEW LOG
  QDockWidget *dockPreview = new QDockWidget("Live Preview Output", this);
  dockPreview->setObjectName("PreviewDock"); // For saving state later
  m_previewLog = new QTextEdit(dockPreview);
  m_previewLog->setReadOnly(true);
  m_previewLog->setStyleSheet(
      "background-color: #1e1e1e; color: #00ff00; font-family: Monospace;");
  dockPreview->setWidget(m_previewLog);
  addDockWidget(Qt::BottomDockWidgetArea, dockPreview);

  // Connect Preview Signals
  connect(m_previewManager, &PreviewManager::previewOutput, this,
          [this](const QString &out) {
            m_previewLog->append(out);
            // Auto scroll
            m_previewLog->moveCursor(QTextCursor::End);
          });
  connect(m_previewManager, &PreviewManager::previewError, this,
          [this](const QString &err) {
            m_previewLog->append("<span style='color:red'>" + err + "</span>");
          });
  connect(m_previewManager, &PreviewManager::previewFinished, this,
          [this](int code) {
            QString status;
            if (code == 0) {
              status = "<span style='color:lime'>Finished Successfully</span>";
            } else if (code == 139) {
              status = "<span style='color:red; font-weight:bold'>PROCESS "
                       "CRASHED (Segfault)</span>";
            } else {
              status =
                  QString(
                      "<span style='color:red'>Failed with exit code %1</span>")
                      .arg(code);
            }
            m_previewLog->append(QString("<br/><b>%1</b><hr/>").arg(status));
          });
}

void MainWindow::createSampleWidgets() {
  // Teste de criação via Factory - Ainda útil para ver algo na tela ao iniciar
  QWidget *w1 = m_factory->createWidget("Label", "lbl_welcome");
  if (w1) {
    w1->setProperty("text", "<h1>Bem-vindo ao Showbox Studio</h1>");
    m_canvas->addWidget(w1);
    m_controller->manageWidget(w1);
  }
}

void MainWindow::onGroupRequested(const QString &containerType) {
  auto selected = m_controller->selectedWidgets();
  if (selected.isEmpty())
    return;

  m_controller->undoStack()->push(new GroupWidgetsCommand(
      m_canvas, m_factory, m_controller, selected, containerType));

  m_inspector->updateHierarchy(m_canvas);
  statusBar()->showMessage("Widgets agrupados em " + containerType);
}

void MainWindow::onDeleteClicked() {
  auto selected = m_controller->selectedWidgets();
  if (selected.isEmpty())
    return;

  // Não permitir deletar o Canvas em si ou algo vital
  if (selected.contains(m_canvas))
    return;

  m_controller->undoStack()->push(new DeleteWidgetCommand(m_canvas, selected));

  for (QWidget *w : selected) {
    m_inspector->onWidgetRemoved(w);
  }

  m_controller->selectWidget(nullptr);
  statusBar()->showMessage(
      QString("Removidos %1 componentes.").arg(selected.size()));
}

void MainWindow::onRunClicked() {
  ScriptGenerator gen;
  QString script = gen.generate(m_canvas);

  if (script.isEmpty()) {
    statusBar()->showMessage("Erro: Nada para exportar.");
    m_previewLog->append(
        "<span style='color:orange'>Warning: Nothing to export.</span>");
    return;
  }

  statusBar()->showMessage("Iniciando Preview...");
  m_previewLog->clear();
  m_previewLog->append(QString("<b>Starting Preview...</b> [%1]")
                           .arg(QTime::currentTime().toString()));
  m_previewLog->append("<i>Generating script...</i>");

  // Ensure dock is visible
  QDockWidget *dock = findChild<QDockWidget *>("PreviewDock");
  if (dock)
    dock->show();

  // Run via Manager
  m_previewManager->runPreview(script);
}

void MainWindow::onSaveClicked() {
  QString fileName = QFileDialog::getSaveFileName(
      this, "Save Project", "", "Showbox Project (*.sbxproj)");
  if (fileName.isEmpty())
    return;

  if (!fileName.endsWith(".sbxproj"))
    fileName += ".sbxproj";

  ProjectSerializer serializer;
  if (serializer.save(fileName, m_canvas, m_factory)) {
    statusBar()->showMessage("Projeto salvo com sucesso: " + fileName);
  } else {
    statusBar()->showMessage("Erro ao salvar projeto.");
  }
}

void MainWindow::onOpenClicked() {
  QString fileName = QFileDialog::getOpenFileName(
      this, "Open Project", "", "Showbox Project (*.sbxproj)");
  if (fileName.isEmpty())
    return;

  m_canvas->clear();
  m_inspector->updateHierarchy(nullptr);

  ProjectSerializer serializer;
  QList<QWidget *> widgets;

  if (serializer.load(fileName, m_factory, widgets)) {
    for (QWidget *w : widgets) {
      m_canvas->addWidget(w);
      m_controller->manageWidget(w);
    }
    m_inspector->updateHierarchy(m_canvas);
    statusBar()->showMessage("Projeto carregado: " + fileName);
  } else {
    statusBar()->showMessage("Erro ao carregar projeto.");
  }
}

void MainWindow::onAddPageRequested(QWidget *tabs) {
  if (!tabs)
    return;
  QString name =
      QString("%1_page%2").arg(tabs->objectName()).arg(tabs->children().size());
  QWidget *page = m_factory->createWidget("Page", name);
  m_controller->undoStack()->push(new AddWidgetCommand(m_canvas, page, tabs));
  m_inspector->updateHierarchy(m_canvas);
  statusBar()->showMessage("Nova aba adicionada.");
}

void MainWindow::onRemovePageRequested(QWidget *tabs) {
  if (auto *tabWidget = qobject_cast<QTabWidget *>(tabs)) {
    QWidget *curr = tabWidget->currentWidget();
    if (curr) {
      if (tabWidget->count() <= 1) {
        statusBar()->showMessage("Não é possível remover a última aba.");
        return;
      }
      m_controller->undoStack()->push(new DeleteWidgetCommand(m_canvas, {curr}));
      m_inspector->updateHierarchy(m_canvas);
      statusBar()->showMessage("Aba removida.");
    }
  }
}

void MainWindow::createToolbox(int style) {
  // Remover toolbox atual se existir
  if (m_toolbox) {
    m_dockToolbox->setWidget(nullptr);
    delete m_toolbox;
    m_toolbox = nullptr;
  }

  // Criar novo toolbox baseado no estilo
  if (style == 0) {
    m_toolbox = new ToolboxClassic(m_dockToolbox);
  } else {
    m_toolbox = new ToolboxTree(m_dockToolbox);
  }

  populateToolbox(m_toolbox);
  m_dockToolbox->setWidget(m_toolbox);
}

void MainWindow::populateToolbox(AbstractToolbox *toolbox) {
  // 0. Layouts
  toolbox->addCategory(
      "Layouts", {"HBoxLayout", "VBoxLayout", "GridLayout", "FormLayout"});

  // 1. Spacers
  toolbox->addCategory("Spacers", {"HorizontalSpacer", "VerticalSpacer"});

  // 2. Controles Básicos (usando nomes CLI)
  toolbox->addCategory("Básico", {"Label", "Button", "CheckBox",
                                  "RadioButton", "Separator"});

  // 3. Entrada de Dados (usando nomes CLI)
  toolbox->addCategory("Entrada", {"TextBox", "TextView", "SpinBox", "Slider",
                                   "ComboBox", "Calendar"});

  // 4. Dados & Visualização
  toolbox->addCategory("Dados", {"ListBox", "Table", "ProgressBar", "Chart"});

  // 5. Containers (usando nomes CLI)
  toolbox->addCategory("Containers",
                       {"GroupBox", "Frame", "Tabs", "Page", "ScrollArea"});
}

void MainWindow::onToolboxStyleChanged(int style) {
  if (style == m_toolboxStyle)
    return;

  m_toolboxStyle = style;
  createToolbox(style);

  // Salvar preferência
  QSettings settings("Showbox", "ShowboxStudio");
  settings.setValue("toolboxStyle", style);

  QString styleName = (style == 0) ? "Classic" : "Tree";
  statusBar()->showMessage("Estilo do Toolbox alterado para: " + styleName);
}

#include "MainWindow.h"
#include "ActionEditor.h"
#include "Canvas.h"
#include "Catalog.h"
#include "ObjectInspector.h"
#include "PropertyEditor.h"
#include "core/PreviewManager.h"
#include "core/ProjectSerializer.h"
#include "core/ProjectWidgetMapper.h"
#include "core/ScriptGenerator.h"
#include "core/StudioCommands.h"
#include "core/StudioController.h"
#include "core/StudioWidgetFactory.h"
#include "toolbox/ToolboxClassic.h"
#include "toolbox/ToolboxTree.h"
#include <QActionGroup>
#include <QApplication>
#include <QCloseEvent>
#include <QCoreApplication>
#include <QDockWidget>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QSaveFile>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QListWidget>
#include <QMenuBar>
#include <QMessageBox>
#include <QProcess>
#include <QSettings>
#include <QStatusBar>
#include <QTabWidget>
#include <QTextEdit>
#include <QTextStream>
#include <QTime>
#include <QToolBar>

namespace {

QWidget *findAutomationWidget(QWidget *root, const QString &name) {
  if (!root || name.isEmpty())
    return nullptr;
  const auto widgets = root->findChildren<QWidget *>();
  for (QWidget *widget : widgets) {
    if (widget->objectName() == name &&
        widget->property("showbox_type").isValid())
      return widget;
  }
  return nullptr;
}

QJsonValue automationValue(const QVariant &value) {
  if (value.typeId() == QMetaType::Bool)
    return value.toBool();
  if (value.canConvert<double>() &&
      (value.typeId() == QMetaType::Int || value.typeId() == QMetaType::UInt ||
       value.typeId() == QMetaType::LongLong ||
       value.typeId() == QMetaType::Double))
    return value.toDouble();
  return value.toString();
}

QVariant automationVariant(const QJsonValue &value,
                           const QVariant &oldValue) {
  if (value.isBool())
    return value.toBool();
  if (value.isDouble()) {
    if (oldValue.typeId() == QMetaType::Int)
      return value.toInt();
    if (oldValue.typeId() == QMetaType::LongLong)
      return static_cast<qlonglong>(value.toDouble());
    return value.toDouble();
  }
  if (value.isNull())
    return QVariant();
  return value.toString();
}

QJsonObject automationDiagnostic(const QString &message,
                                 const QString &code = "validation_failed") {
  return QJsonObject{{"code", code},
                     {"severity", "error"},
                     {"component", "studio"},
                     {"message", message},
                     {"context", QJsonObject{}},
                     {"location", QJsonObject{}},
                     {"suggestion", "Corrija o projeto e tente novamente."}};
}

} // namespace

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
  connect(m_controller->undoStack(), &QUndoStack::indexChanged, this,
          &MainWindow::onUndoIndexChanged);

  // Edições de ações não geram comandos de undo; rastreá-las à parte.
  connect(m_actionEditor, &ActionEditor::actionsChanged, this,
          [this]() { m_actionsModified = true; });

  // Sincronizar seleção: Canvas -> Inspector & Property Editor

  m_projectDirectory = QDir::currentPath();

  // MenuBar
  QMenu *fileMenu = menuBar()->addMenu("&File");
  fileMenu->addAction("New Project", QKeySequence::New, this,
                      &MainWindow::onNewClicked);
  fileMenu->addAction("Open", this, &MainWindow::onOpenClicked,
                      QKeySequence::Open);
  fileMenu->addAction("Save", this, &MainWindow::onSaveClicked,
                      QKeySequence::Save);
  fileMenu->addAction("Exportar aplicação Bash…", this, &MainWindow::onExportClicked);
  fileMenu->addAction("Nova demonstração campo/botão/rótulo", this, &MainWindow::onDemoClicked);
  fileMenu->addSeparator();
  fileMenu->addAction("Exit", this, &QWidget::close, QKeySequence::Quit);

  QMenu *editMenu = menuBar()->addMenu("&Edit");
  editMenu->addAction(m_controller->undoStack()->createUndoAction(this));
  editMenu->addAction(m_controller->undoStack()->createRedoAction(this));

  // Menu View para alternar estilo do Toolbox. Criado aqui (e não no
  // setupUI) para garantir a ordem padrão File, Edit, View, Help.
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

  // Menu Help (padrão File, Edit, View, Help)
  QMenu *helpMenu = menuBar()->addMenu("&Help");
  helpMenu->addAction("About Showbox Studio", this, [this]() {
    // Versão definida em main.cpp via setApplicationVersion().
    const QString version = QCoreApplication::applicationVersion();
    QMessageBox::about(
        this, "About Showbox Studio",
        QString("Showbox Studio %1\n\nAmbiente visual Qt6 para criar "
                "ferramentas com shell script como back-end.\n"
                "Documentação: docs/user/README.md")
            .arg(version.isEmpty() ? QStringLiteral("dev") : version));
  });
  helpMenu->addAction("About Qt", qApp, &QApplication::aboutQt);

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

MainWindow::~MainWindow() {
  // Teardown ordenado (SB-015): parar a prévia e soltar referências antes de
  // destruir, para que nenhum sinal alcance objetos em destruição parcial.
  if (m_previewManager) {
    disconnect(m_previewManager, nullptr, this, nullptr);
    m_previewManager->stop();
  }
  if (m_canvas) {
    m_canvas->setController(nullptr);
  }
  // O QUndoStack é destruído por último (filho do controller, criado cedo);
  // desconectar antecipa as notificações que tocariam editores já liberados.
  if (m_controller && m_controller->undoStack()) {
    disconnect(m_controller->undoStack(), &QUndoStack::indexChanged, this,
               &MainWindow::onUndoIndexChanged);
  }
  delete m_previewManager;
  m_previewManager = nullptr;
  delete m_factory;
  m_factory = nullptr;
}

void MainWindow::onUndoIndexChanged() {
  if (m_controller->selectedWidget()) {
    m_propEditor->setTargetWidget(m_controller->selectedWidget());
  }
  m_inspector->updateHierarchy(m_canvas);
}

void MainWindow::setupUI() {
  statusBar()->showMessage("Pronto");

  // Toolbar
  QToolBar *toolBar = addToolBar("Main Toolbar");

  QAction *newAction = toolBar->addAction("New");
  connect(newAction, &QAction::triggered, this, &MainWindow::onNewClicked);

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

  QAction *visualAction = toolBar->addAction("Prévia visual");
  connect(visualAction, &QAction::triggered, this, &MainWindow::onVisualPreview);
  QAction *runAction = toolBar->addAction("Executar aplicação");
  runAction->setShortcut(QKeySequence("F5"));
  connect(runAction, &QAction::triggered, this, &MainWindow::onRunClicked);

  QAction *stopAction = toolBar->addAction("Parar");
  stopAction->setEnabled(false);
  connect(stopAction, &QAction::triggered, m_previewManager, &PreviewManager::stop);
  connect(m_previewManager, &PreviewManager::runningChanged, this,
          [runAction, visualAction, stopAction](bool running) {
    runAction->setEnabled(!running); visualAction->setEnabled(!running);
    stopAction->setEnabled(running);
  });

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

  // Right Dock: PROPERTIES & ACTIONS (Tabbed)
  QDockWidget *dockProps = new QDockWidget("Propriedades", this);
  QTabWidget *propsTabWidget = new QTabWidget(dockProps);

  m_propEditor = new PropertyEditor(propsTabWidget);
  m_actionEditor = new ActionEditor(propsTabWidget);
  m_actionEditor->setController(m_controller);
  connect(m_actionEditor, &ActionEditor::executionRequested, this, &MainWindow::onRunClicked);

  propsTabWidget->addTab(m_propEditor, "Propriedades");
  propsTabWidget->addTab(m_actionEditor, "Ações");

  dockProps->setWidget(propsTabWidget);
  addDockWidget(Qt::RightDockWidgetArea, dockProps);

  // Bottom Dock: PREVIEW LOG
  QDockWidget *dockPreview = new QDockWidget("Live Preview Output", this);
  dockPreview->setObjectName("PreviewDock"); // For saving state later
  m_previewLog = new QTextEdit(dockPreview);
  m_previewLog->setReadOnly(true);
  m_previewLog->document()->setMaximumBlockCount(2000);
  m_previewLog->setStyleSheet(
      "background-color: #1e1e1e; color: #00ff00; font-family: Monospace;");
  dockPreview->setWidget(m_previewLog);
  addDockWidget(Qt::BottomDockWidgetArea, dockPreview);

  // Connect Preview Signals
  connect(m_previewManager, &PreviewManager::previewOutput, this,
          [this](const QString &out) {
            m_automationPreviewLogs += out;
            m_previewLog->moveCursor(QTextCursor::End);
            m_previewLog->insertPlainText(out);
            // Auto scroll
            m_previewLog->moveCursor(QTextCursor::End);
          });
  connect(m_previewManager, &PreviewManager::previewError, this,
          [this](const QString &err) {
            m_automationPreviewLogs += err;
            m_previewLog->moveCursor(QTextCursor::End);
            m_previewLog->insertPlainText(err);
          });
  connect(m_previewManager, &PreviewManager::previewFinished, this,
          [this](int code) {
            m_automationPreviewLogs +=
                QString("\n[preview.finished] exitCode=%1\n").arg(code);
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

void MainWindow::onNewClicked() {
  if (!confirmDiscardIfModified())
    return;

  m_previewManager->stop();
  m_controller->selectWidget(nullptr);
  m_controller->undoStack()->clear();
  m_canvas->clear();
  m_actionEditor->setTargetWidget(nullptr);
  m_propEditor->setTargetWidget(nullptr);
  m_inspector->updateHierarchy(m_canvas);
  m_actionsModified = false;
  m_projectDirectory = QDir::currentPath();
  markDocumentSaved();
  statusBar()->showMessage("Novo projeto criado.");
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
    statusBar()->showMessage(gen.errorString());
    m_previewLog->append(
        gen.errorString().toHtmlEscaped());
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
  m_previewManager->runPreview(script, m_projectDirectory);
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
    markDocumentSaved();
    m_projectDirectory = QFileInfo(fileName).absolutePath();
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

  if (!confirmDiscardIfModified())
    return;

  ProjectSerializer serializer;
  QList<QWidget *> widgets;

  if (serializer.load(fileName, m_factory, widgets)) {
    m_actionsModified = false;
    m_controller->selectWidget(nullptr);
    m_controller->undoStack()->clear();
    m_canvas->clear();
    m_inspector->updateHierarchy(nullptr);

    for (QWidget *w : widgets) {
      m_canvas->addWidget(w);
      m_controller->manageWidget(w);
    }
    m_inspector->updateHierarchy(m_canvas);
    m_projectDirectory = QFileInfo(fileName).absolutePath();
    const QStringList errors = serializer.errors();
    statusBar()->showMessage(errors.isEmpty()
                                 ? "Projeto carregado: " + fileName
                                 : "Projeto carregado com ressalvas: " +
                                       fileName);
  } else {
    const QStringList errors = serializer.errors();
    const QString message = errors.isEmpty()
                                ? "Erro ao carregar projeto."
                                : errors.join("\n");
    QMessageBox::warning(this, "Showbox Studio", message);
    statusBar()->showMessage("Não foi possível carregar o projeto.");
  }
}

bool MainWindow::confirmDiscardIfModified() {
  if (!hasUnsavedChanges())
    return true;

  const QMessageBox::StandardButton answer = QMessageBox::question(
      this, "Showbox Studio",
      "O projeto atual tem alterações não salvas. Descartar?",
      QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
  return answer == QMessageBox::Yes;
}

bool MainWindow::hasUnsavedChanges() const {
  return m_actionsModified || !m_controller->undoStack()->isClean();
}

void MainWindow::markDocumentSaved() {
  m_controller->undoStack()->setClean();
  m_actionsModified = false;
}

void MainWindow::closeEvent(QCloseEvent *event) {
  if (!confirmDiscardIfModified()) {
    event->ignore();
    return;
  }
  // Parar a prévia antes do teardown para que o destrutor não precise
  // finalizar um processo ainda em Starting/Running.
  if (m_previewManager)
    m_previewManager->stop();
  event->accept();
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

QJsonObject MainWindow::automationProjectSnapshot() const {
  const ProjectModel model = ProjectWidgetMapper::toModel(m_canvas);
  QJsonObject snapshot = model.toJson();
  snapshot["projectDirectory"] = m_projectDirectory;
  snapshot["dirty"] = hasUnsavedChanges();
  snapshot["selected"] = m_controller->selectedWidget()
                             ? m_controller->selectedWidget()->objectName()
                             : QString();
  snapshot["previewRunning"] = automationPreviewRunning();
  return snapshot;
}

QJsonObject MainWindow::automationUiTree() const {
  const ProjectModel model = ProjectWidgetMapper::toModel(m_canvas);
  return QJsonObject{{"version", 1},
                     {"nodes", model.toJson().value("widgets")},
                     {"selected", m_controller->selectedWidget()
                                      ? m_controller->selectedWidget()->objectName()
                                      : QString()}};
}

QJsonArray MainWindow::automationDiagnostics() const {
  QJsonArray diagnostics;
  const ProjectModel model = ProjectWidgetMapper::toModel(m_canvas);
  for (const QString &issue : model.validate())
    diagnostics.append(automationDiagnostic(issue));
  return diagnostics;
}

QJsonObject MainWindow::automationExportValidate() const {
  ScriptGenerator generator;
  const QString script = generator.generate(m_canvas);
  QJsonArray diagnostics = automationDiagnostics();
  if (script.isEmpty())
    diagnostics.append(automationDiagnostic(generator.errorString(),
                                            "export_not_available"));
  return QJsonObject{{"valid", !script.isEmpty() && diagnostics.isEmpty()},
                     {"diagnostics", diagnostics},
                     {"bytes", script.toUtf8().size()}};
}

bool MainWindow::automationPreviewRunning() const {
  return m_previewManager && m_previewManager->isRunning();
}

bool MainWindow::automationNew(QString *error) {
  Q_UNUSED(error);
  if (m_previewManager)
    m_previewManager->stop();
  m_controller->selectWidget(nullptr);
  m_controller->undoStack()->clear();
  m_canvas->clear();
  m_actionEditor->setTargetWidget(nullptr);
  m_propEditor->setTargetWidget(nullptr);
  m_inspector->updateHierarchy(m_canvas);
  m_actionsModified = false;
  m_projectDirectory = QDir::currentPath();
  markDocumentSaved();
  return true;
}

bool MainWindow::automationOpen(const QString &fileName, QString *error) {
  if (fileName.isEmpty()) {
    if (error)
      *error = "O caminho do projeto não pode ser vazio.";
    return false;
  }
  ProjectSerializer serializer;
  QList<QWidget *> widgets;
  if (!serializer.load(fileName, m_factory, widgets)) {
    if (error)
      *error = serializer.errors().join("\n");
    qDeleteAll(widgets);
    return false;
  }
  m_previewManager->stop();
  m_controller->selectWidget(nullptr);
  m_controller->undoStack()->clear();
  m_canvas->clear();
  for (QWidget *widget : widgets) {
    m_canvas->addWidget(widget);
    m_controller->manageWidget(widget);
  }
  m_inspector->updateHierarchy(m_canvas);
  m_projectDirectory = QFileInfo(fileName).absolutePath();
  markDocumentSaved();
  return true;
}

bool MainWindow::automationSave(const QString &fileName, QString *error) {
  if (fileName.isEmpty()) {
    if (error)
      *error = "O caminho do projeto não pode ser vazio.";
    return false;
  }
  ProjectSerializer serializer;
  if (!serializer.save(fileName, m_canvas, m_factory)) {
    if (error)
      *error = "Não foi possível salvar o projeto: " + fileName;
    return false;
  }
  m_projectDirectory = QFileInfo(fileName).absolutePath();
  markDocumentSaved();
  return true;
}

bool MainWindow::automationAddWidget(const QString &type, const QString &name,
                                     const QString &parentName, QString *error) {
  if (!showbox::catalog::isKnownType(type)) {
    if (error)
      *error = "Tipo de componente desconhecido: " + type;
    return false;
  }
  if (name.isEmpty()) {
    if (error)
      *error = "O nome do componente não pode ser vazio.";
    return false;
  }
  if (findAutomationWidget(m_canvas, name)) {
    if (error)
      *error = "Já existe um componente com o nome: " + name;
    return false;
  }
  QWidget *parent = parentName.isEmpty() ? static_cast<QWidget *>(m_canvas)
                                         : findAutomationWidget(m_canvas, parentName);
  if (!parent) {
    if (error)
      *error = "Componente pai não encontrado: " + parentName;
    return false;
  }
  if (parent != m_canvas && !parent->layout() &&
      !qobject_cast<QTabWidget *>(parent)) {
    if (error)
      *error = "O componente pai não aceita filhos: " + parentName;
    return false;
  }
  QWidget *widget = m_factory->createWidget(type, name);
  if (!widget) {
    if (error)
      *error = "Não foi possível criar o componente: " + type;
    return false;
  }
  m_controller->undoStack()->push(new AddWidgetCommand(m_canvas, widget, parent));
  m_controller->manageWidget(widget);
  m_inspector->updateHierarchy(m_canvas);
  return true;
}

bool MainWindow::automationRemoveWidget(const QString &name, QString *error) {
  QWidget *widget = findAutomationWidget(m_canvas, name);
  if (!widget) {
    if (error)
      *error = "Componente não encontrado: " + name;
    return false;
  }
  m_controller->undoStack()->push(new DeleteWidgetCommand(m_canvas, {widget}));
  m_controller->selectWidget(nullptr);
  m_inspector->updateHierarchy(m_canvas);
  return true;
}

bool MainWindow::automationSelectWidget(const QString &name, QString *error) {
  QWidget *widget = name.isEmpty() ? nullptr : findAutomationWidget(m_canvas, name);
  if (!name.isEmpty() && !widget) {
    if (error)
      *error = "Componente não encontrado: " + name;
    return false;
  }
  m_controller->selectWidget(widget);
  return true;
}

bool MainWindow::automationMoveWidget(const QString &name,
                                       const QString &parentName, int index,
                                       QString *error) {
  QWidget *widget = findAutomationWidget(m_canvas, name);
  QWidget *parent = parentName.isEmpty() ? static_cast<QWidget *>(m_canvas)
                                         : findAutomationWidget(m_canvas, parentName);
  if (!widget || !parent) {
    if (error)
      *error = "Componente ou pai não encontrado.";
    return false;
  }
  if (widget == parent || widget->isAncestorOf(parent)) {
    if (error)
      *error = "Não é permitido mover um componente para dentro de si mesmo.";
    return false;
  }
  if (parent != m_canvas && !parent->layout()) {
    if (error)
      *error = "O componente pai não possui um layout.";
    return false;
  }
  m_controller->undoStack()->push(new MoveWidgetCommand(widget, parent, index));
  m_inspector->updateHierarchy(m_canvas);
  return true;
}

bool MainWindow::automationSetProperty(const QString &name,
                                       const QString &property,
                                       const QJsonValue &value,
                                       QString *error) {
  QWidget *widget = findAutomationWidget(m_canvas, name);
  if (!widget || property.isEmpty()) {
    if (error)
      *error = "Componente ou propriedade inválida.";
    return false;
  }
  const QVariant oldValue = widget->property(property.toUtf8().constData());
  if (!oldValue.isValid() && widget->metaObject()->indexOfProperty(
                                  property.toUtf8().constData()) < 0) {
    if (error)
      *error = "Propriedade não encontrada: " + property;
    return false;
  }
  const QVariant newValue = automationVariant(value, oldValue);
  m_controller->undoStack()->push(new PropertyChangeCommand(
      widget, property, oldValue, newValue));
  m_propEditor->setTargetWidget(widget);
  return true;
}

bool MainWindow::automationSetActions(const QString &name,
                                      const QJsonObject &actions,
                                      QString *error) {
  QWidget *widget = findAutomationWidget(m_canvas, name);
  if (!widget) {
    if (error)
      *error = "Componente não encontrado: " + name;
    return false;
  }
  const QString encoded =
      QString::fromUtf8(QJsonDocument(actions).toJson(QJsonDocument::Compact));
  const QString oldValue = widget->property("showbox_actions").toString();
  m_controller->undoStack()->push(new PropertyChangeCommand(
      widget, "showbox_actions", oldValue, encoded));
  m_actionsModified = true;
  m_actionEditor->setTargetWidget(widget);
  return true;
}

bool MainWindow::automationUndo(QString *error) {
  if (!m_controller->undoStack()->canUndo()) {
    if (error)
      *error = "Não há alterações para desfazer.";
    return false;
  }
  m_controller->undoStack()->undo();
  onUndoIndexChanged();
  return true;
}

bool MainWindow::automationRedo(QString *error) {
  if (!m_controller->undoStack()->canRedo()) {
    if (error)
      *error = "Não há alterações para refazer.";
    return false;
  }
  m_controller->undoStack()->redo();
  onUndoIndexChanged();
  return true;
}

bool MainWindow::automationStartPreview(QString *error) {
  ScriptGenerator generator;
  const QString script = generator.generate(m_canvas);
  if (script.isEmpty()) {
    if (error)
      *error = generator.errorString();
    return false;
  }
  m_automationPreviewLogs.clear();
  m_previewLog->clear();
  m_previewManager->runPreview(script, m_projectDirectory);
  return true;
}

bool MainWindow::automationStopPreview(QString *error) {
  Q_UNUSED(error);
  if (m_previewManager)
    m_previewManager->stop();
  return true;
}

bool MainWindow::automationExport(const QString &fileName, QJsonObject *result,
                                  QString *error) {
  ScriptGenerator generator;
  const QString script = generator.generate(m_canvas);
  if (script.isEmpty()) {
    if (error)
      *error = generator.errorString();
    return false;
  }
  if (!fileName.isEmpty()) {
    QSaveFile file(fileName);
    const QByteArray bytes = script.toUtf8();
    if (!file.open(QIODevice::WriteOnly) || file.write(bytes) != bytes.size() ||
        !file.commit()) {
      if (error)
        *error = "Não foi possível exportar o arquivo Bash: " + fileName;
      return false;
    }
    QFile::setPermissions(fileName,
                          QFile::permissions(fileName) | QFileDevice::ExeOwner);
  }
  if (result)
    *result = QJsonObject{{"path", fileName}, {"script", script},
                          {"bytes", script.toUtf8().size()}};
  return true;
}

void MainWindow::createToolbox(int style) {
  // Remover toolbox atual se existir. deleteLater() em vez de delete
  // síncrono: uma troca de estilo durante um drag ativo destruiria a origem
  // do evento (QListWidget/QTreeWidget) ainda em uso pelo Canvas.
  if (m_toolbox) {
    m_dockToolbox->setWidget(nullptr);
    m_toolbox->deleteLater();
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
  // Categorias e itens vêm do catálogo compartilhado, na ordem declarada.
  for (const QString &group : showbox::catalog::toolboxGroups()) {
    QStringList items;
    for (const auto &info : showbox::catalog::widgetCatalog()) {
      if (info.toolboxGroup == group) items << info.displayName;
    }
    toolbox->addCategory(group, items);
  }
  // Honestidade de exportação (SB-016): o que o motor não renderiza
  // (scriptable=false) continua visível, mas desabilitado com aviso — a
  // decisão vem do catálogo, sem lista duplicada aqui.
  for (const auto &info : showbox::catalog::widgetCatalog()) {
    if (!info.toolboxGroup.isEmpty() && !info.scriptable) {
      toolbox->markItemExperimental(
          info.displayName,
          QString("%1 ainda não é exportável nesta versão — suporte em "
                  "breve. Prévia e exportação recusam este componente.")
              .arg(info.displayName));
    }
  }
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

void MainWindow::onVisualPreview() {
  ScriptGenerator generator;
  const auto commands = generator.generateUi(m_canvas);
  if (commands.isEmpty()) { statusBar()->showMessage(generator.errorString()); return; }
  m_previewManager->runVisualPreview(commands);
}

void MainWindow::onExportClicked() {
  ScriptGenerator generator;
  const auto script = generator.generate(m_canvas);
  if (script.isEmpty()) { statusBar()->showMessage(generator.errorString()); return; }
  const auto path = QFileDialog::getSaveFileName(this, "Exportar aplicação Bash", {}, "Bash (*.sh)");
  if (path.isEmpty()) return;
  QSaveFile file(path);
  const auto bytes = script.toUtf8();
  if (!file.open(QIODevice::WriteOnly) || file.write(bytes) != bytes.size() || !file.commit()) {
    statusBar()->showMessage("Não foi possível exportar a aplicação."); return;
  }
  QFile::setPermissions(path, QFile::permissions(path) | QFileDevice::ExeOwner);
  statusBar()->showMessage("Aplicação exportada: " + path);
}

void MainWindow::onDemoClicked() {
  if (!confirmDiscardIfModified())
    return;
  m_controller->selectWidget(nullptr);
  m_controller->undoStack()->clear();
  m_canvas->clear();
  m_actionsModified = false;
  auto *entry = m_factory->createWidget("textbox", "entry");
  auto *button = m_factory->createWidget("pushbutton", "run");
  auto *result = m_factory->createWidget("label", "result");
  button->setProperty("text", "Saudar");
  result->setProperty("text", "Preencha o campo e clique em Saudar.");
  const QJsonObject action{{"type", "shell"}, {"command",
    "showbox_get VALUE entry\nshowbox_set result text \"Olá: $VALUE\"\nprintf 'Ação concluída\\n'"}};
  button->setProperty("showbox_actions", QString::fromUtf8(QJsonDocument(
    QJsonObject{{"clicked", QJsonArray{action}}}).toJson()));
  for (auto *widget : {entry, button, result}) {
    m_canvas->addWidget(widget); m_controller->manageWidget(widget);
  }
  m_inspector->updateHierarchy(m_canvas);
  m_controller->selectWidget(button);
  m_actionsModified = true;
  statusBar()->showMessage("Demonstração criada. Use Executar aplicação ou Prévia visual.");
}

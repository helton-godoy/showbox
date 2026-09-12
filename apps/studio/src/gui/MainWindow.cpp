#include "MainWindow.h"
#include "ActionEditor.h"
#include "automation/AutomationDescriptors.h"
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
#include "custom_table_widget.h"
#include <QAbstractButton>
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
#include <QComboBox>
#include <QGroupBox>
#include <QListWidget>
#include <QProgressBar>
#include <QSlider>
#include <QSpinBox>
#include <QTableWidget>
#include <cmath>
#include <functional>
#include <limits>

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

ProjectNode *findAutomationModelNode(QList<ProjectNode> *nodes,
                                     const QString &name) {
  if (!nodes)
    return nullptr;
  for (ProjectNode &node : *nodes) {
    if (node.name == name)
      return &node;
    if (ProjectNode *child = findAutomationModelNode(&node.children, name))
      return child;
  }
  return nullptr;
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

bool jsonInteger(const QJsonValue &value, int *out) {
  if (!value.isDouble())
    return false;
  const double number = value.toDouble();
  if (!std::isfinite(number) || std::floor(number) != number ||
      number < std::numeric_limits<int>::min() ||
      number > std::numeric_limits<int>::max())
    return false;
  if (out)
    *out = static_cast<int>(number);
  return true;
}

bool jsonStringArray(const QJsonValue &value) {
  if (!value.isArray())
    return false;
  for (const QJsonValue &item : value.toArray()) {
    if (!item.isString())
      return false;
  }
  return true;
}

bool jsonStringMatrix(const QJsonValue &value) {
  if (!value.isArray())
    return false;
  for (const QJsonValue &row : value.toArray()) {
    if (!jsonStringArray(row))
      return false;
  }
  return true;
}

QLineEdit *automationLineEdit(QWidget *widget) {
  if (!widget)
    return nullptr;
  if (auto *edit = qobject_cast<QLineEdit *>(widget->focusProxy()))
    return edit;
  return widget->findChild<QLineEdit *>();
}

QJsonValue nodePropertyValue(const ProjectNode &node, const QString &property) {
  if (property == "items")
    return node.items;
  if (property == "headers")
    return node.headers;
  if (property == "rows")
    return node.rows;
  return node.properties.value(property);
}

struct AutomationTableState {
  QStringList headers;
  QList<QStringList> rows;
};

struct AutomationComboState {
  QStringList items;
  int currentIndex = -1;
};

QJsonArray stringListToJson(const QStringList &list) {
  QJsonArray result;
  for (const QString &item : list)
    result.append(item);
  return result;
}

QJsonArray tableRowsToJson(const QList<QStringList> &rows) {
  QJsonArray result;
  for (const QStringList &row : rows)
    result.append(stringListToJson(row));
  return result;
}

AutomationTableState readAutomationTableState(QTableWidget *table) {
  AutomationTableState state;
  if (!table)
    return state;
  for (int column = 0; column < table->columnCount(); ++column) {
    const QTableWidgetItem *header = table->horizontalHeaderItem(column);
    state.headers.append(header ? header->text() : QString());
  }
  for (int row = 0; row < table->rowCount(); ++row) {
    QStringList cells;
    for (int column = 0; column < table->columnCount(); ++column) {
      const QTableWidgetItem *item = table->item(row, column);
      cells.append(item ? item->text() : QString());
    }
    state.rows.append(cells);
  }
  return state;
}

void applyAutomationTableState(QTableWidget *table,
                               const AutomationTableState &state) {
  if (!table)
    return;
  const int columns = qMax(state.headers.size(),
                           state.rows.isEmpty()
                               ? 0
                               : [&state] {
                                   int width = 0;
                                   for (const QStringList &row : state.rows)
                                     width = qMax(width, row.size());
                                   return width;
                                 }());
  table->clear();
  table->setRowCount(state.rows.size());
  table->setColumnCount(qMax(0, columns));
  table->setHorizontalHeaderLabels(state.headers);
  for (int row = 0; row < state.rows.size(); ++row) {
    const QStringList &cells = state.rows.at(row);
    for (int column = 0; column < cells.size(); ++column)
      table->setItem(row, column, new QTableWidgetItem(cells.at(column)));
  }
}

AutomationComboState readAutomationComboState(QComboBox *combo) {
  AutomationComboState state;
  if (!combo)
    return state;
  for (int i = 0; i < combo->count(); ++i)
    state.items.append(combo->itemText(i));
  state.currentIndex = combo->currentIndex();
  return state;
}

void applyAutomationComboState(QComboBox *combo,
                               const AutomationComboState &state) {
  if (!combo)
    return;
  combo->blockSignals(true);
  combo->clear();
  for (const QString &item : state.items)
    combo->addItem(item);
  if (state.items.isEmpty())
    combo->setCurrentIndex(-1);
  else
    combo->setCurrentIndex(qBound(0, state.currentIndex, state.items.size() - 1));
  combo->blockSignals(false);
}

QTableWidget *automationTableWidget(QWidget *widget) {
  auto *custom = qobject_cast<CustomTableWidget *>(widget);
  return custom ? custom->table() : nullptr;
}

bool applyTypedAutomationProperty(QWidget *widget, const QString &property,
                                  const QJsonValue &value, QString *error) {
  const QString type = showbox::catalog::canonicalType(
      widget ? widget->property("showbox_type").toString() : QString());
  if (!widget || !showbox::automation::isMutableProperty(type, property)) {
    if (error)
      *error = "Propriedade não é mutável no contrato público: " + property;
    return false;
  }

  if (property == "items" || property == "headers") {
    if (!jsonStringArray(value)) {
      if (error)
        *error = "A propriedade " + property + " exige uma lista de strings.";
      return false;
    }
  } else if (property == "rows") {
    if (!jsonStringMatrix(value)) {
      if (error)
        *error = "A propriedade rows exige uma matriz de strings.";
      return false;
    }
  } else if (showbox::automation::propertyType(type, property) == "boolean") {
    if (!value.isBool()) {
      if (error)
        *error = "A propriedade " + property + " exige booleano.";
      return false;
    }
  } else if (showbox::automation::propertyType(type, property) == "string") {
    if (!value.isString()) {
      if (error)
        *error = "A propriedade " + property + " exige string.";
      return false;
    }
  } else {
    int ignored = 0;
    if (!jsonInteger(value, &ignored)) {
      if (error)
        *error = "A propriedade " + property + " exige número inteiro.";
      return false;
    }
  }

  if (property == "enabled") {
    widget->setEnabled(value.toBool());
  } else if (property == "width" || property == "height") {
    int number = 0;
    jsonInteger(value, &number);
    if (number < 0) {
      if (error)
        *error = "Dimensões não podem ser negativas.";
      return false;
    }
    QSize size = widget->size();
    if (property == "width")
      size.setWidth(number);
    else
      size.setHeight(number);
    widget->resize(size);
  } else if (type == "textbox") {
    QLineEdit *edit = automationLineEdit(widget);
    if (!edit) {
      if (error)
        *error = "Textbox sem campo de entrada associado.";
      return false;
    }
    if (property == "text")
      edit->setText(value.toString());
    else if (property == "placeholder")
      edit->setPlaceholderText(value.toString());
    else if (property == "readOnly")
      edit->setReadOnly(value.toBool());
    else if (property == "echoMode")
      edit->setEchoMode(static_cast<QLineEdit::EchoMode>(value.toInt()));
  } else if (type == "textview") {
    auto *text = qobject_cast<QTextEdit *>(widget);
    if (!text) {
      if (error)
        *error = "TextView sem controle associado.";
      return false;
    }
    if (property == "plainText")
      text->setPlainText(value.toString());
    else if (property == "readOnly")
      text->setReadOnly(value.toBool());
  } else if (type == "combobox") {
    auto *combo = widget->findChild<QComboBox *>();
    if (!combo) {
      if (error)
        *error = "ComboBox sem controle associado.";
      return false;
    }
    if (property == "items") {
      combo->clear();
      for (const QJsonValue &item : value.toArray())
        combo->addItem(item.toString());
    } else {
      int index = 0;
      jsonInteger(value, &index);
      if (index < -1 || index >= combo->count()) {
        if (error)
          *error = "currentIndex fora dos limites do ComboBox.";
        return false;
      }
      combo->setCurrentIndex(index);
    }
  } else if (type == "listbox") {
    auto *list = widget->findChild<QListWidget *>();
    if (!list) {
      if (error)
        *error = "ListBox sem controle associado.";
      return false;
    }
    list->clear();
    for (const QJsonValue &item : value.toArray())
      list->addItem(item.toString());
  } else if (type == "table") {
    auto *custom = qobject_cast<CustomTableWidget *>(widget);
    QTableWidget *table = custom ? custom->table() : nullptr;
    if (!table) {
      if (error)
        *error = "Tabela sem controle associado.";
      return false;
    }
    if (property == "headers") {
      const QStringList headers = [&value] {
        QStringList result;
        for (const QJsonValue &item : value.toArray())
          result.append(item.toString());
        return result;
      }();
      table->setColumnCount(headers.size());
      table->setHorizontalHeaderLabels(headers);
    } else {
      const QJsonArray rows = value.toArray();
      int columns = table->columnCount();
      for (const QJsonValue &row : rows)
        columns = qMax(columns, row.toArray().size());
      table->clearContents();
      table->setColumnCount(columns);
      table->setRowCount(rows.size());
      for (int row = 0; row < rows.size(); ++row) {
        const QJsonArray cells = rows[row].toArray();
        for (int column = 0; column < cells.size(); ++column)
          table->setItem(row, column, new QTableWidgetItem(cells[column].toString()));
      }
    }
  } else if (property == "text") {
    if (auto *button = qobject_cast<QAbstractButton *>(widget))
      button->setText(value.toString());
    else if (auto *label = qobject_cast<QLabel *>(widget))
      label->setText(value.toString());
  } else if (property == "checked" || property == "checkable") {
    if (auto *button = qobject_cast<QAbstractButton *>(widget)) {
      if (property == "checked")
        button->setChecked(value.toBool());
      else
        button->setCheckable(value.toBool());
    } else if (auto *group = qobject_cast<QGroupBox *>(widget)) {
      if (property == "checked")
        group->setChecked(value.toBool());
      else
        group->setCheckable(value.toBool());
    }
  } else if (property == "title") {
    if (auto *group = qobject_cast<QGroupBox *>(widget))
      group->setTitle(value.toString());
    else if (type == "page")
      widget->setProperty("title", value.toString());
  } else if (auto *spin = qobject_cast<QSpinBox *>(widget)) {
    int number = 0;
    jsonInteger(value, &number);
    if (property == "value") spin->setValue(number);
    else if (property == "minimum") spin->setMinimum(number);
    else if (property == "maximum") spin->setMaximum(number);
    else if (property == "singleStep") spin->setSingleStep(number);
  } else if (auto *slider = qobject_cast<QSlider *>(widget)) {
    int number = 0;
    jsonInteger(value, &number);
    if (property == "value") slider->setValue(number);
    else if (property == "minimum") slider->setMinimum(number);
    else if (property == "maximum") slider->setMaximum(number);
    else if (property == "orientation") slider->setOrientation(
        number == 2 ? Qt::Vertical : Qt::Horizontal);
  } else if (auto *progress = qobject_cast<QProgressBar *>(widget)) {
    int number = 0;
    jsonInteger(value, &number);
    if (property == "value") progress->setValue(number);
    else if (property == "minimum") progress->setMinimum(number);
    else if (property == "maximum") progress->setMaximum(number);
    else if (property == "orientation") progress->setOrientation(
        number == 2 ? Qt::Vertical : Qt::Horizontal);
  }
  return true;
}

class AutomationPropertyCommand final : public QUndoCommand {
public:
  using Apply = std::function<bool(QWidget *, const QString &, const QJsonValue &, QString *)>;
  AutomationPropertyCommand(QWidget *widget, QString property,
                             QJsonValue oldValue, QJsonValue newValue,
                             Apply apply)
      : m_widget(widget), m_property(std::move(property)),
        m_oldValue(std::move(oldValue)), m_newValue(std::move(newValue)),
        m_apply(std::move(apply)) {
    setText("Automation property " + m_property);
  }
  void undo() override { if (m_widget) m_apply(m_widget, m_property, m_oldValue, nullptr); }
  void redo() override { if (m_widget) m_apply(m_widget, m_property, m_newValue, nullptr); }

private:
  QPointer<QWidget> m_widget;
  QString m_property;
  QJsonValue m_oldValue;
  QJsonValue m_newValue;
  Apply m_apply;
};

// Comando atômico para tabela: guarda headers+rows antes e depois, de modo
// que reduzir colunas nunca destrua células de forma irreversível. O undo
// restaura o estado completo, não apenas a propriedade tocada.
class AutomationTableCommand final : public QUndoCommand {
public:
  AutomationTableCommand(QWidget *widget, QString property,
                         AutomationTableState oldState,
                         AutomationTableState newState)
      : m_widget(widget), m_property(std::move(property)),
        m_oldState(std::move(oldState)), m_newState(std::move(newState)) {
    setText("Automation table " + m_property);
  }
  void undo() override {
    if (m_widget)
      if (QTableWidget *table = automationTableWidget(m_widget))
        applyAutomationTableState(table, m_oldState);
  }
  void redo() override {
    if (m_widget)
      if (QTableWidget *table = automationTableWidget(m_widget))
        applyAutomationTableState(table, m_newState);
  }

private:
  QPointer<QWidget> m_widget;
  QString m_property;
  AutomationTableState m_oldState;
  AutomationTableState m_newState;
};

// Comando atômico para combobox: items e currentIndex viajam juntos. Trocar
// items sem guardar o índice perdia a seleção no undo; aqui ambos são
// restaurados.
class AutomationComboCommand final : public QUndoCommand {
public:
  AutomationComboCommand(QWidget *widget, QString property,
                         AutomationComboState oldState,
                         AutomationComboState newState)
      : m_widget(widget), m_property(std::move(property)),
        m_oldState(std::move(oldState)), m_newState(std::move(newState)) {
    setText("Automation combobox " + m_property);
  }
  void undo() override {
    if (m_widget)
      if (QComboBox *combo = m_widget->findChild<QComboBox *>())
        applyAutomationComboState(combo, m_oldState);
  }
  void redo() override {
    if (m_widget)
      if (QComboBox *combo = m_widget->findChild<QComboBox *>())
        applyAutomationComboState(combo, m_newState);
  }

private:
  QPointer<QWidget> m_widget;
  QString m_property;
  AutomationComboState m_oldState;
  AutomationComboState m_newState;
};

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
          [this]() {
            m_actionsModified = true;
            emit automationEvent("project.changed",
                                 QJsonObject{{"source", "gui"},
                                             {"kind", "actions"}});
            emit automationEvent("dirty.changed",
                                 QJsonObject{{"dirty", true}});
          });

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
  connect(m_controller, &StudioController::widgetSelected, this,
          [this](QWidget *widget) {
            emit automationEvent(
                "selection.changed",
                QJsonObject{{"name", widget ? widget->objectName() : QString()}});
          });
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
  emit automationEvent("dirty.changed", QJsonObject{{"dirty", hasUnsavedChanges()}});
  emit automationEvent("diagnostics.changed",
                       QJsonObject{{"count", automationDiagnostics().size()}});
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
          [this, runAction, visualAction, stopAction](bool running) {
    runAction->setEnabled(!running); visualAction->setEnabled(!running);
    stopAction->setEnabled(running);
    // O término é publicado exclusivamente por previewFinished (com
    // exitCode). Emitir preview.finished aqui duplicaria o evento: finish()
    // emite runningChanged(false) seguido de previewFinished(code).
    if (running)
      emit automationEvent("preview.started",
                           QJsonObject{{"running", true}});
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
            emit automationEvent("preview.output",
                                 QJsonObject{{"stream", "stdout"},
                                             {"text", out}});
            m_previewLog->moveCursor(QTextCursor::End);
            m_previewLog->insertPlainText(out);
            // Auto scroll
            m_previewLog->moveCursor(QTextCursor::End);
          });
  connect(m_previewManager, &PreviewManager::previewError, this,
          [this](const QString &err) {
            m_automationPreviewLogs += err;
            emit automationEvent("preview.output",
                                 QJsonObject{{"stream", "stderr"},
                                             {"text", err}});
            m_previewLog->moveCursor(QTextCursor::End);
            m_previewLog->insertPlainText(err);
          });
  connect(m_previewManager, &PreviewManager::previewFinished, this,
          [this](int code) {
            m_automationPreviewLogs +=
                QString("\n[preview.finished] exitCode=%1\n").arg(code);
            emit automationEvent("preview.finished",
                                 QJsonObject{{"running", false},
                                             {"exitCode", code}});
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
  return automationNew(false, error);
}

bool MainWindow::automationNew(bool force, QString *error) {
  if (hasUnsavedChanges() && !force) {
    if (error)
      *error = "O projeto possui alterações não salvas; use force=true para descartar.";
    return false;
  }
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
  emit automationEvent("project.changed", QJsonObject{{"source", "automation"},
                                                       {"operation", "new"},
                                                       {"discarded", force}});
  return true;
}

bool MainWindow::automationOpen(const QString &fileName, QString *error) {
  return automationOpen(fileName, false, error);
}

bool MainWindow::automationOpen(const QString &fileName, bool force,
                                QString *error) {
  if (fileName.isEmpty()) {
    if (error)
      *error = "O caminho do projeto não pode ser vazio.";
    return false;
  }
  if (hasUnsavedChanges() && !force) {
    if (error)
      *error = "O projeto possui alterações não salvas; use force=true para descartar.";
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
  emit automationEvent("project.changed", QJsonObject{{"source", "automation"},
                                                       {"operation", "open"},
                                                       {"discarded", force}});
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
  emit automationEvent("project.changed", QJsonObject{{"source", "automation"},
                                                       {"operation", "save"}});
  return true;
}

bool MainWindow::automationAddWidget(const QString &type, const QString &name,
                                     const QString &parentName, QString *error) {
  if (!showbox::catalog::isKnownType(type)) {
    if (error)
      *error = "Tipo de componente desconhecido: " + type;
    return false;
  }
  QString nameError;
  if (!ProjectModel::isValidWidgetName(name, &nameError)) {
    if (error)
      *error = nameError;
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
  const QString type = showbox::catalog::canonicalType(
      widget->property("showbox_type").toString());
  if (!showbox::automation::isMutableProperty(type, property)) {
    if (error)
      *error = "Propriedade não é mutável no contrato público: " + property;
    return false;
  }
  // Propriedades compostas viajam com seus dependentes para que o undo seja
  // atômico: tabela guarda headers+rows; combobox guarda items+currentIndex.
  if (type == "table" && (property == "headers" || property == "rows")) {
    QTableWidget *table = automationTableWidget(widget);
    if (!table) {
      if (error)
        *error = "Tabela sem controle associado.";
      return false;
    }
    const AutomationTableState oldState = readAutomationTableState(table);
    AutomationTableState newState = oldState;
    if (property == "headers") {
      if (!jsonStringArray(value)) {
        if (error)
          *error = "A propriedade headers exige uma lista de strings.";
        return false;
      }
      QStringList headers;
      for (const QJsonValue &item : value.toArray())
        headers.append(item.toString());
      newState.headers = headers;
      // Ajustar linhas à nova largura sem perder dados além do truncamento
      // explícito; o estado antigo completo permanece no comando para undo.
      for (QStringList &row : newState.rows) {
        while (row.size() > headers.size())
          row.removeLast();
        while (row.size() < headers.size())
          row.append(QString());
      }
    } else {
      if (!jsonStringMatrix(value)) {
        if (error)
          *error = "A propriedade rows exige uma matriz de strings.";
        return false;
      }
      QList<QStringList> rows;
      int width = 0;
      for (const QJsonValue &rowValue : value.toArray()) {
        QStringList cells;
        for (const QJsonValue &cell : rowValue.toArray())
          cells.append(cell.toString());
        width = qMax(width, cells.size());
        rows.append(cells);
      }
      newState.rows = rows;
      // Expandir cabeçalhos quando as linhas exigirem mais colunas; nunca
      // encolher cabeçalhos aqui para não perder rótulos.
      while (newState.headers.size() < width)
        newState.headers.append(QString());
      for (QStringList &row : newState.rows) {
        while (row.size() < qMax(newState.headers.size(), width))
          row.append(QString());
      }
    }
    m_controller->undoStack()->push(
        new AutomationTableCommand(widget, property, oldState, newState));
    m_propEditor->setTargetWidget(widget);
    if (!automationDiagnostics().isEmpty()) {
      m_controller->undoStack()->undo();
      if (error)
        *error = "A alteração produziria um projeto inválido.";
      return false;
    }
    return true;
  }
  if (type == "combobox" && (property == "items" || property == "currentIndex")) {
    QComboBox *combo = widget->findChild<QComboBox *>();
    if (!combo) {
      if (error)
        *error = "ComboBox sem controle associado.";
      return false;
    }
    const AutomationComboState oldState = readAutomationComboState(combo);
    AutomationComboState newState = oldState;
    if (property == "items") {
      if (!jsonStringArray(value)) {
        if (error)
          *error = "A propriedade items exige uma lista de strings.";
        return false;
      }
      QStringList items;
      for (const QJsonValue &item : value.toArray())
        items.append(item.toString());
      newState.items = items;
      if (items.isEmpty())
        newState.currentIndex = -1;
      else if (oldState.currentIndex < 0)
        newState.currentIndex = 0;
      else
        newState.currentIndex = qMin(oldState.currentIndex, items.size() - 1);
    } else {
      int index = 0;
      if (!jsonInteger(value, &index)) {
        if (error)
          *error = "A propriedade currentIndex exige número inteiro.";
        return false;
      }
      if (index < -1 || index >= combo->count()) {
        if (error)
          *error = "currentIndex fora dos limites do ComboBox.";
        return false;
      }
      newState.currentIndex = index;
    }
    m_controller->undoStack()->push(
        new AutomationComboCommand(widget, property, oldState, newState));
    m_propEditor->setTargetWidget(widget);
    if (!automationDiagnostics().isEmpty()) {
      m_controller->undoStack()->undo();
      if (error)
        *error = "A alteração produziria um projeto inválido.";
      return false;
    }
    return true;
  }
  const ProjectNode node = ProjectWidgetMapper::toNode(widget);
  const QJsonValue oldValue = nodePropertyValue(node, property);
  if (oldValue.isUndefined()) {
    if (error)
      *error = "Propriedade não está disponível no snapshot: " + property;
    return false;
  }
  QString applyError;
  if (!applyTypedAutomationProperty(widget, property, value, &applyError)) {
    if (error)
      *error = applyError;
    return false;
  }
  // Reverter a aplicação imediata; o comando realiza a alteração e registra
  // undo/redo com valores do modelo, não com QVariant do QWidget externo.
  applyTypedAutomationProperty(widget, property, oldValue, nullptr);
  m_controller->undoStack()->push(new AutomationPropertyCommand(
      widget, property, oldValue, value,
      [](QWidget *target, const QString &key, const QJsonValue &newValue,
         QString *applyError) {
        return applyTypedAutomationProperty(target, key, newValue, applyError);
      }));
  m_propEditor->setTargetWidget(widget);
  if (!automationDiagnostics().isEmpty()) {
    m_controller->undoStack()->undo();
    if (error)
      *error = "A alteração produziria um projeto inválido.";
    return false;
  }
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
  // Validar o modelo proposto antes de mutar: evita gravar ações que deixam
  // o projeto inválido (shell sem command, set/query incompletos, destinos
  // inexistentes). Nenhum comando entra na pilha quando inválido.
  {
    ProjectModel proposed = ProjectWidgetMapper::toModel(m_canvas);
    ProjectNode *node = findAutomationModelNode(&proposed.widgets, name);
    if (!node) {
      if (error)
        *error = "Componente não encontrado no modelo: " + name;
      return false;
    }
    node->actions = encoded;
    const QStringList issues = proposed.validate();
    if (!issues.isEmpty()) {
      if (error)
        *error = issues.first();
      return false;
    }
  }
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

#include "ActionEditor.h"
#include "../core/StudioController.h"
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>

ActionEditor::ActionEditor(QWidget *parent) : QWidget(parent) {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(5, 5, 5, 5);
  mainLayout->setSpacing(8);

  // === Seletor de Evento ===
  QHBoxLayout *eventLayout = new QHBoxLayout();
  eventLayout->addWidget(new QLabel("Evento:"));
  m_eventCombo = new QComboBox();
  m_eventCombo->addItems({"clicked", "changed", "textChanged", "valueChanged"});
  eventLayout->addWidget(m_eventCombo, 1);
  mainLayout->addLayout(eventLayout);

  connect(m_eventCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, [this](int) { updateActionsList(); });

  // === Lista de Ações ===
  QGroupBox *actionsGroup = new QGroupBox("Ações");
  QVBoxLayout *actionsLayout = new QVBoxLayout(actionsGroup);

  m_actionsList = new QListWidget();
  m_actionsList->setMaximumHeight(120);
  actionsLayout->addWidget(m_actionsList);

  connect(m_actionsList, &QListWidget::currentRowChanged, this,
          &ActionEditor::onActionSelected);

  // Botões de controle
  QHBoxLayout *btnLayout = new QHBoxLayout();
  QPushButton *addBtn = new QPushButton("+");
  addBtn->setFixedWidth(30);
  QPushButton *removeBtn = new QPushButton("-");
  removeBtn->setFixedWidth(30);
  QPushButton *upBtn = new QPushButton("↑");
  upBtn->setFixedWidth(30);
  QPushButton *downBtn = new QPushButton("↓");
  downBtn->setFixedWidth(30);

  btnLayout->addWidget(addBtn);
  btnLayout->addWidget(removeBtn);
  btnLayout->addWidget(upBtn);
  btnLayout->addWidget(downBtn);
  btnLayout->addStretch();
  actionsLayout->addLayout(btnLayout);

  connect(addBtn, &QPushButton::clicked, this, &ActionEditor::onAddAction);
  connect(removeBtn, &QPushButton::clicked, this,
          &ActionEditor::onRemoveAction);
  connect(upBtn, &QPushButton::clicked, this, &ActionEditor::onMoveUp);
  connect(downBtn, &QPushButton::clicked, this, &ActionEditor::onMoveDown);

  mainLayout->addWidget(actionsGroup);

  // === Detalhes da Ação ===
  QGroupBox *detailsGroup = new QGroupBox("Configuração");
  QVBoxLayout *detailsLayout = new QVBoxLayout(detailsGroup);

  // Tipo de ação
  QHBoxLayout *typeLayout = new QHBoxLayout();
  typeLayout->addWidget(new QLabel("Tipo:"));
  m_actionType = new QComboBox();
  m_actionType->addItems({"Comando Shell", "Script Externo", "Modificar Widget",
                          "Consultar Widget", "Callback"});
  typeLayout->addWidget(m_actionType, 1);
  detailsLayout->addLayout(typeLayout);

  connect(m_actionType, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, &ActionEditor::onActionTypeChanged);

  // Painel de detalhes (muda conforme o tipo)
  m_detailsPanel = new QWidget();
  QVBoxLayout *panelLayout = new QVBoxLayout(m_detailsPanel);
  panelLayout->setContentsMargins(0, 0, 0, 0);

  // Comando/Caminho (para shell, script, callback)
  QHBoxLayout *cmdLayout = new QHBoxLayout();
  cmdLayout->addWidget(new QLabel("Comando:"));
  m_commandLine = new QLineEdit();
  m_commandLine->setPlaceholderText("ex: notify-send \"Clicado!\"");
  cmdLayout->addWidget(m_commandLine, 1);
  panelLayout->addLayout(cmdLayout);

  connect(m_commandLine, &QLineEdit::textChanged, this,
          &ActionEditor::onCommandChanged);

  // Widget alvo (para set/query)
  QHBoxLayout *targetLayout = new QHBoxLayout();
  targetLayout->addWidget(new QLabel("Widget:"));
  m_targetWidget = new QLineEdit();
  m_targetWidget->setPlaceholderText("nome_do_widget");
  targetLayout->addWidget(m_targetWidget, 1);
  panelLayout->addLayout(targetLayout);

  // Propriedade (para set)
  QHBoxLayout *propLayout = new QHBoxLayout();
  propLayout->addWidget(new QLabel("Propriedade:"));
  m_propertyName = new QLineEdit();
  m_propertyName->setPlaceholderText("text");
  propLayout->addWidget(m_propertyName, 1);
  panelLayout->addLayout(propLayout);

  // Valor (para set)
  QHBoxLayout *valueLayout = new QHBoxLayout();
  valueLayout->addWidget(new QLabel("Valor:"));
  m_valueLine = new QLineEdit();
  valueLayout->addWidget(m_valueLine, 1);
  panelLayout->addLayout(valueLayout);

  // Variável (para query)
  QHBoxLayout *varLayout = new QHBoxLayout();
  varLayout->addWidget(new QLabel("Variável:"));
  m_variableName = new QLineEdit();
  m_variableName->setPlaceholderText("$RESULT");
  varLayout->addWidget(m_variableName, 1);
  panelLayout->addLayout(varLayout);

  detailsLayout->addWidget(m_detailsPanel);

  // Indicador de segurança
  m_securityLabel = new QLabel();
  m_securityLabel->setWordWrap(true);
  m_securityLabel->setStyleSheet("padding: 5px; border-radius: 3px;");
  m_securityLabel->hide();
  detailsLayout->addWidget(m_securityLabel);

  mainLayout->addWidget(detailsGroup);

  // Botões de ação
  QHBoxLayout *actionBtnLayout = new QHBoxLayout();

  QPushButton *testBtn = new QPushButton("🧪 Testar");
  connect(testBtn, &QPushButton::clicked, this, &ActionEditor::onTestAction);
  actionBtnLayout->addWidget(testBtn);

  QPushButton *helpBtn = new QPushButton("❓ Variáveis");
  connect(helpBtn, &QPushButton::clicked, this, [this]() {
    QMessageBox::information(
        this, "Variáveis Disponíveis",
        "<h3>Variáveis Predefinidas</h3>"
        "<p>Use estas variáveis nos seus comandos:</p>"
        "<table border='1' cellpadding='5'>"
        "<tr><td><b>$WIDGET_NAME</b></td><td>Nome do widget</td></tr>"
        "<tr><td><b>$WIDGET_VALUE</b></td><td>Valor atual do widget</td></tr>"
        "<tr><td><b>$EVENT</b></td><td>Nome do evento (clicked, etc.)</td></tr>"
        "<tr><td><b>$TIMESTAMP</b></td><td>Data/hora da execução</td></tr>"
        "</table>"
        "<h4>Exemplo:</h4>"
        "<code>notify-send \"$WIDGET_NAME clicado em $TIMESTAMP\"</code>");
  });
  actionBtnLayout->addWidget(helpBtn);

  mainLayout->addLayout(actionBtnLayout);

  // Botões de importar/exportar
  QHBoxLayout *ioBtnLayout = new QHBoxLayout();

  QPushButton *exportBtn = new QPushButton("📤 Exportar");
  connect(exportBtn, &QPushButton::clicked, this, [this]() {
    if (m_actionsData.isEmpty()) {
      QMessageBox::information(this, "Exportar", "Nenhuma ação para exportar.");
      return;
    }
    QString fileName = QFileDialog::getSaveFileName(this, "Exportar Ações",
                                                    QString(), "JSON (*.json)");
    if (fileName.isEmpty())
      return;

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
      QJsonDocument doc(m_actionsData);
      file.write(doc.toJson(QJsonDocument::Indented));
      file.close();
      QMessageBox::information(this, "Exportar",
                               "Ações exportadas com sucesso!");
    }
  });
  ioBtnLayout->addWidget(exportBtn);

  QPushButton *importBtn = new QPushButton("📥 Importar");
  connect(importBtn, &QPushButton::clicked, this, [this]() {
    QString fileName = QFileDialog::getOpenFileName(this, "Importar Ações",
                                                    QString(), "JSON (*.json)");
    if (fileName.isEmpty())
      return;

    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
      QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
      file.close();

      if (doc.isObject()) {
        m_actionsData = doc.object();
        updateActionsList();
        saveActionsToWidget();
        QMessageBox::information(this, "Importar",
                                 "Ações importadas com sucesso!");
      } else {
        QMessageBox::warning(this, "Erro", "Arquivo JSON inválido.");
      }
    }
  });
  ioBtnLayout->addWidget(importBtn);

  mainLayout->addLayout(ioBtnLayout);

  mainLayout->addStretch();

  // Estado inicial
  onActionTypeChanged(0);
}

void ActionEditor::setTargetWidget(QWidget *widget) {
  m_target = widget;
  clearEditor();

  if (!m_target)
    return;

  loadActionsFromWidget();
  updateActionsList();
}

void ActionEditor::loadActionsFromWidget() {
  if (!m_target)
    return;

  QVariant actionsVar = m_target->property("showbox_actions");
  if (!actionsVar.isValid() || actionsVar.toString().isEmpty()) {
    m_actionsData = QJsonObject();
    return;
  }

  QJsonDocument doc = QJsonDocument::fromJson(actionsVar.toString().toUtf8());
  m_actionsData = doc.object();
}

void ActionEditor::saveActionsToWidget() {
  if (!m_target)
    return;

  QJsonDocument doc(m_actionsData);
  m_target->setProperty("showbox_actions",
                        QString(doc.toJson(QJsonDocument::Compact)));

  emit actionsChanged();
}

void ActionEditor::clearEditor() {
  m_actionsList->clear();
  m_commandLine->clear();
  m_targetWidget->clear();
  m_propertyName->clear();
  m_valueLine->clear();
  m_variableName->clear();
  m_actionsData = QJsonObject();
}

void ActionEditor::updateActionsList() {
  m_actionsList->clear();

  QString event = m_eventCombo->currentText();
  QJsonArray actions = m_actionsData[event].toArray();

  for (const QJsonValue &val : actions) {
    QJsonObject action = val.toObject();
    QString type = action["type"].toString();
    QString desc;

    if (type == "shell") {
      desc = QString("🖥️ %1").arg(action["command"].toString());
    } else if (type == "script") {
      desc = QString("📜 %1").arg(action["command"].toString());
    } else if (type == "set") {
      desc =
          QString("✏️ %1.%2 = %3")
              .arg(action["target"].toString(), action["property"].toString(),
                   action["value"].toString());
    } else if (type == "query") {
      desc =
          QString("❓ %1 → $%2")
              .arg(action["target"].toString(), action["variable"].toString());
    } else if (type == "callback") {
      desc = QString("📤 %1").arg(action["command"].toString());
    }

    m_actionsList->addItem(desc);
  }
}

void ActionEditor::onAddAction() {
  QString event = m_eventCombo->currentText();
  QJsonArray actions = m_actionsData[event].toArray();

  QJsonObject newAction;
  newAction["type"] = "shell";
  newAction["command"] = "echo \"Nova ação\"";

  actions.append(newAction);
  m_actionsData[event] = actions;

  updateActionsList();
  saveActionsToWidget();

  m_actionsList->setCurrentRow(m_actionsList->count() - 1);
}

void ActionEditor::onRemoveAction() {
  int row = m_actionsList->currentRow();
  if (row < 0)
    return;

  QString event = m_eventCombo->currentText();
  QJsonArray actions = m_actionsData[event].toArray();
  actions.removeAt(row);
  m_actionsData[event] = actions;

  updateActionsList();
  saveActionsToWidget();
}

void ActionEditor::onMoveUp() {
  int row = m_actionsList->currentRow();
  if (row <= 0)
    return;

  QString event = m_eventCombo->currentText();
  QJsonArray actions = m_actionsData[event].toArray();

  QJsonValue temp = actions[row];
  actions[row] = actions[row - 1];
  actions[row - 1] = temp;

  m_actionsData[event] = actions;
  updateActionsList();
  saveActionsToWidget();
  m_actionsList->setCurrentRow(row - 1);
}

void ActionEditor::onMoveDown() {
  int row = m_actionsList->currentRow();
  if (row < 0 || row >= m_actionsList->count() - 1)
    return;

  QString event = m_eventCombo->currentText();
  QJsonArray actions = m_actionsData[event].toArray();

  QJsonValue temp = actions[row];
  actions[row] = actions[row + 1];
  actions[row + 1] = temp;

  m_actionsData[event] = actions;
  updateActionsList();
  saveActionsToWidget();
  m_actionsList->setCurrentRow(row + 1);
}

void ActionEditor::onActionSelected(int row) {
  if (row < 0)
    return;

  QString event = m_eventCombo->currentText();
  QJsonArray actions = m_actionsData[event].toArray();

  if (row >= actions.count())
    return;

  QJsonObject action = actions[row].toObject();
  QString type = action["type"].toString();

  // Mapear tipo para índice do combo
  int typeIndex = 0;
  if (type == "shell")
    typeIndex = 0;
  else if (type == "script")
    typeIndex = 1;
  else if (type == "set")
    typeIndex = 2;
  else if (type == "query")
    typeIndex = 3;
  else if (type == "callback")
    typeIndex = 4;

  m_actionType->setCurrentIndex(typeIndex);

  m_commandLine->setText(action["command"].toString());
  m_targetWidget->setText(action["target"].toString());
  m_propertyName->setText(action["property"].toString());
  m_valueLine->setText(action["value"].toString());
  m_variableName->setText(action["variable"].toString());
}

void ActionEditor::onActionTypeChanged(int index) {
  // Mostrar/esconder campos conforme o tipo
  bool isShellOrScript = (index == 0 || index == 1 || index == 4);
  bool isSet = (index == 2);
  bool isQuery = (index == 3);

  m_commandLine->setVisible(isShellOrScript);
  m_commandLine->parentWidget()->findChild<QLabel *>()->setVisible(
      isShellOrScript);

  m_targetWidget->setVisible(isSet || isQuery);
  m_propertyName->setVisible(isSet);
  m_valueLine->setVisible(isSet);
  m_variableName->setVisible(isQuery);

  // Atualizar placeholder
  if (index == 0)
    m_commandLine->setPlaceholderText("ex: notify-send \"Clicado!\"");
  else if (index == 1)
    m_commandLine->setPlaceholderText("ex: ./scripts/handler.sh");
  else if (index == 4)
    m_commandLine->setPlaceholderText("ex: button_clicked");

  updateSecurityIndicator();
}

void ActionEditor::onCommandChanged() {
  int row = m_actionsList->currentRow();
  if (row < 0)
    return;

  QString event = m_eventCombo->currentText();
  QJsonArray actions = m_actionsData[event].toArray();

  if (row >= actions.count())
    return;

  QJsonObject action = actions[row].toObject();

  // Mapear índice para tipo
  QString types[] = {"shell", "script", "set", "query", "callback"};
  action["type"] = types[m_actionType->currentIndex()];
  action["command"] = m_commandLine->text();
  action["target"] = m_targetWidget->text();
  action["property"] = m_propertyName->text();
  action["value"] = m_valueLine->text();
  action["variable"] = m_variableName->text();

  actions[row] = action;
  m_actionsData[event] = actions;

  saveActionsToWidget();
  updateActionsList();
  updateSecurityIndicator();
  m_actionsList->setCurrentRow(row);
}

void ActionEditor::onTestAction() {
  int row = m_actionsList->currentRow();
  if (row < 0) {
    QMessageBox::information(this, "Teste", "Selecione uma ação para testar.");
    return;
  }

  QString event = m_eventCombo->currentText();
  QJsonArray actions = m_actionsData[event].toArray();

  if (row >= actions.count())
    return;

  QJsonObject action = actions[row].toObject();
  QString type = action["type"].toString();

  if (type == "shell" || type == "script") {
    QString cmd = action["command"].toString();

    // Verificar nível de segurança
    SecurityLevel level = analyzeCommand(cmd);

    if (level == Dangerous) {
      int ret = QMessageBox::critical(
          this, "⚠️ Comando Perigoso",
          "Este comando é considerado PERIGOSO:\n\n" + cmd +
              "\n\nRiscos identificados:\n" + getSecurityWarning(cmd) +
              "\n\nTem CERTEZA que deseja executar?",
          QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
      if (ret != QMessageBox::Yes)
        return;
    } else if (level == Warning) {
      int ret = QMessageBox::warning(
          this, "⚠️ Aviso de Segurança",
          "Este comando requer atenção:\n\n" + cmd + "\n\nAvisos:\n" +
              getSecurityWarning(cmd) + "\n\nDeseja executar?",
          QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
      if (ret != QMessageBox::Yes)
        return;
    }

    QProcess::startDetached("/bin/bash", QStringList() << "-c" << cmd);
    QMessageBox::information(this, "Teste", "Comando executado:\n" + cmd);
  } else {
    QMessageBox::information(this, "Teste",
                             "Ações do tipo '" + type +
                                 "' só podem ser testadas no Preview.");
  }
}

ActionEditor::SecurityLevel ActionEditor::analyzeCommand(const QString &cmd) {
  // Padrões PERIGOSOS - podem causar perda de dados ou comprometer segurança
  QStringList dangerousPatterns = {"rm -rf",      "rm -r",       "rm -f",
                                   "mkfs",        "dd if=",      "format",
                                   "> /dev/",     "chmod 777",   "chmod -R",
                                   ":(){:|:&};:", // Fork bomb
                                   "curl | bash", "wget | bash", "eval ",
                                   "$(",          "`", // Command injection
                                   "/etc/passwd", "/etc/shadow"};

  for (const QString &pattern : dangerousPatterns) {
    if (cmd.contains(pattern, Qt::CaseInsensitive)) {
      return Dangerous;
    }
  }

  // Padrões de AVISO - requerem atenção mas podem ser legítimos
  QStringList warningPatterns = {
      "sudo", "su ",  "rm ",    "mv ",       "cp ",      ">",      ">>",
      "|",    "kill", "pkill",  "killall",   "shutdown", "reboot", "apt",
      "yum",  "dnf",  "pacman", "systemctl", "service"};

  for (const QString &pattern : warningPatterns) {
    if (cmd.contains(pattern, Qt::CaseInsensitive)) {
      return Warning;
    }
  }

  return Safe;
}

QString ActionEditor::getSecurityWarning(const QString &cmd) {
  QStringList warnings;

  // Verificações específicas
  if (cmd.contains("rm", Qt::CaseInsensitive)) {
    if (cmd.contains("-rf") || cmd.contains("-r")) {
      warnings << "❌ Remoção recursiva de arquivos";
    } else {
      warnings << "⚠️ Pode deletar arquivos";
    }
  }

  if (cmd.contains("sudo")) {
    warnings << "⚠️ Execução com privilégios de root";
  }

  if (cmd.contains(">")) {
    if (cmd.contains(">>")) {
      warnings << "⚠️ Append para arquivo";
    } else {
      warnings << "⚠️ Sobrescrita de arquivo";
    }
  }

  if (cmd.contains("|")) {
    warnings << "⚠️ Pipe para outro comando";
  }

  if (cmd.contains("dd ")) {
    warnings << "❌ Escrita direta em dispositivo de bloco";
  }

  if (cmd.contains("curl") || cmd.contains("wget")) {
    warnings << "⚠️ Download de conteúdo externo";
    if (cmd.contains("| bash") || cmd.contains("| sh")) {
      warnings << "❌ Execução de código remoto!";
    }
  }

  if (cmd.contains("eval") || cmd.contains("$(") || cmd.contains("`")) {
    warnings << "⚠️ Avaliação dinâmica de código";
  }

  return warnings.isEmpty() ? "Nenhum risco específico identificado"
                            : warnings.join("\n");
}

void ActionEditor::updateSecurityIndicator() {
  int typeIndex = m_actionType->currentIndex();
  bool isShellOrScript = (typeIndex == 0 || typeIndex == 1);

  if (!isShellOrScript) {
    m_securityLabel->hide();
    return;
  }

  QString cmd = m_commandLine->text();
  if (cmd.isEmpty()) {
    m_securityLabel->hide();
    return;
  }

  SecurityLevel level = analyzeCommand(cmd);

  switch (level) {
  case Safe:
    m_securityLabel->setText("✅ Comando seguro");
    m_securityLabel->setStyleSheet("background-color: #1a4d1a; color: #90EE90; "
                                   "padding: 5px; border-radius: 3px;");
    break;
  case Warning:
    m_securityLabel->setText("⚠️ " +
                             getSecurityWarning(cmd).split("\n").first());
    m_securityLabel->setStyleSheet("background-color: #4d4d1a; color: #FFD700; "
                                   "padding: 5px; border-radius: 3px;");
    break;
  case Dangerous:
    m_securityLabel->setText("❌ PERIGOSO: " +
                             getSecurityWarning(cmd).split("\n").first());
    m_securityLabel->setStyleSheet("background-color: #4d1a1a; color: #FF6B6B; "
                                   "padding: 5px; border-radius: 3px;");
    break;
  }

  m_securityLabel->show();
}

QString ActionEditor::getActionsJson() const {
  QJsonDocument doc(m_actionsData);
  return QString(doc.toJson(QJsonDocument::Compact));
}

#include "ActionEditor.h"
#include <QComboBox>
#include <QFile>
#include <QFileDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSignalBlocker>
#include <QVBoxLayout>

ActionEditor::ActionEditor(QWidget *parent) : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);
    m_event = new QComboBox;
    m_event->setObjectName("actionEvent");
    layout->addWidget(new QLabel("Evento do componente:"));
    layout->addWidget(m_event);
    m_list = new QListWidget;
    m_list->setObjectName("actionList");
    m_list->setMaximumHeight(120);
    layout->addWidget(m_list);
    auto *buttons = new QHBoxLayout;
    for (const QString &label : {QString("Adicionar"), QString("Remover"), QString("↑"), QString("↓")}) {
        auto *button = new QPushButton(label);
        buttons->addWidget(button);
        connect(button, &QPushButton::clicked, this, [this, label] {
            if (!m_target || m_event->currentText().isEmpty()) return;
            auto actions = m_actions[m_event->currentText()].toArray();
            if (label == "Adicionar") {
                actions.append(QJsonObject{{"type", "shell"}, {"command", "printf 'Olá!\\n'"}});
                m_actions[m_event->currentText()] = actions;
                saveToWidget(); reloadList(actions.size() - 1);
            } else if (label == "Remover") {
                const int row = m_list->currentRow();
                if (row < 0) return;
                actions.removeAt(row); m_actions[m_event->currentText()] = actions;
                saveToWidget(); reloadList(qMin(row, int(actions.size()) - 1));
            } else moveAction(label == "↑" ? -1 : 1);
        });
    }
    layout->addLayout(buttons);
    auto *form = new QFormLayout;
    m_type = new QComboBox;
    m_type->setObjectName("actionType");
    m_type->addItem("Comando Bash", "shell");
    m_type->addItem("Modificar componente", "set");
    m_type->addItem("Consultar componente", "query");
    form->addRow("Tipo:", m_type);
    m_command = new QPlainTextEdit;
    m_command->setObjectName("actionCommand");
    m_command->setPlaceholderText("showbox_get VALUE entry\nshowbox_set result text \"Olá: $VALUE\"");
    form->addRow("Código Bash:", m_command);
    m_targetName = new QLineEdit; m_targetName->setObjectName("actionTarget");
    m_property = new QLineEdit; m_property->setObjectName("actionProperty");
    m_value = new QLineEdit; m_value->setObjectName("actionValue");
    m_variable = new QLineEdit; m_variable->setObjectName("actionVariable");
    m_variable->setPlaceholderText("VALUE (sem $)");
    form->addRow("Componente:", m_targetName);
    form->addRow("Propriedade:", m_property);
    form->addRow("Valor literal:", m_value);
    form->addRow("Variável:", m_variable);
    layout->addLayout(form);
    auto *help = new QLabel("Bash: use $WIDGET_NAME, $WIDGET_VALUE e $EVENT.\n"
                           "showbox_get VALUE nome consulta um campo;\n"
                           "showbox_set nome text \"$VALUE\" atualiza texto.\n"
                           "As ações executam somente em Executar aplicação.");
    help->setWordWrap(true); layout->addWidget(help);
    auto *run = new QPushButton("Executar aplicação");
    layout->addWidget(run);
    connect(run, &QPushButton::clicked, this, &ActionEditor::executionRequested);
    auto *io = new QHBoxLayout;
    auto *import = new QPushButton("Importar ações");
    auto *exportButton = new QPushButton("Exportar ações");
    io->addWidget(import); io->addWidget(exportButton); layout->addLayout(io);
    connect(exportButton, &QPushButton::clicked, this, [this] {
        const QString path = QFileDialog::getSaveFileName(this, "Exportar ações", {}, "JSON (*.json)");
        if (path.isEmpty()) return;
        QFile file(path);
        if (!file.open(QIODevice::WriteOnly) || file.write(getActionsJson().toUtf8()) < 0)
            QMessageBox::warning(this, "Erro", "Não foi possível salvar as ações.");
    });
    connect(import, &QPushButton::clicked, this, [this] {
        const QString path = QFileDialog::getOpenFileName(this, "Importar ações", {}, "JSON (*.json)");
        if (path.isEmpty()) return;
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly)) return;
        const auto doc = QJsonDocument::fromJson(file.readAll());
        if (!doc.isObject()) { QMessageBox::warning(this, "Erro", "JSON inválido."); return; }
        m_actions = doc.object(); saveToWidget(); reloadList();
    });
    connect(m_event, &QComboBox::currentTextChanged, this, [this] { if (!m_loading) reloadList(); });
    connect(m_list, &QListWidget::currentRowChanged, this, &ActionEditor::loadAction);
    connect(m_command, &QPlainTextEdit::textChanged, this, &ActionEditor::saveAction);
    for (auto *edit : {m_targetName, m_property, m_value, m_variable})
        connect(edit, &QLineEdit::textChanged, this, &ActionEditor::saveAction);
    connect(m_type, &QComboBox::currentIndexChanged, this, [this] {
        const auto type = m_type->currentData().toString();
        m_command->setEnabled(type == "shell");
        m_targetName->setEnabled(type == "set" || type == "query");
        m_property->setEnabled(type == "set"); m_value->setEnabled(type == "set");
        m_variable->setEnabled(type == "query");
        saveAction();
    });
    setTargetWidget(nullptr);
}

void ActionEditor::setTargetWidget(QWidget *widget) {
    m_loading = true;
    m_target = widget;
    m_actions = widget ? QJsonDocument::fromJson(widget->property("showbox_actions").toString().toUtf8()).object() : QJsonObject();
    m_event->clear();
    if (widget) {
        const auto type = widget->property("showbox_type").toString();
        if (type == "button" || type == "pushbutton") {
            if (widget->property("checkable").toBool()) m_event->addItems({"pressed", "released"});
            else m_event->addItem("clicked");
        } else if (type == "slider") m_event->addItem("changed");
    }
    // Preserva eventos importados para que possam ser inspecionados/removidos.
    for (auto it = m_actions.begin(); it != m_actions.end(); ++it)
        if (m_event->findText(it.key()) < 0) m_event->addItem(it.key());
    setEnabled(widget != nullptr);
    m_loading = false; reloadList();
}

QString ActionEditor::getActionsJson() const {
    return QString::fromUtf8(QJsonDocument(m_actions).toJson(QJsonDocument::Compact));
}
void ActionEditor::saveToWidget() {
    if (!m_target) return;
    m_target->setProperty("showbox_actions", getActionsJson());
    emit actionsChanged();
}
void ActionEditor::reloadList(int row) {
    const QSignalBlocker blocker(m_list);
    m_list->clear();
    const auto actions = m_actions[m_event->currentText()].toArray();
    for (const auto &action : actions) m_list->addItem(action.toObject()["type"].toString());
    m_list->setCurrentRow(row);
    loadAction(row);
}
void ActionEditor::loadAction(int row) {
    m_loading = true;
    const auto actions = m_actions[m_event->currentText()].toArray();
    const auto action = row >= 0 && row < actions.size() ? actions[row].toObject() : QJsonObject();
    m_type->setCurrentIndex(m_type->findData(action["type"].toString()));
    m_command->setPlainText(action["command"].toString());
    m_targetName->setText(action["target"].toString()); m_property->setText(action["property"].toString());
    m_value->setText(action["value"].toString()); m_variable->setText(action["variable"].toString());
    m_loading = false;
}
void ActionEditor::saveAction() {
    if (m_loading || !m_target || m_list->currentRow() < 0) return;
    auto actions = m_actions[m_event->currentText()].toArray();
    const int row = m_list->currentRow();
    if (row >= actions.size() || m_type->currentIndex() < 0) return;
    actions[row] = QJsonObject{{"type", m_type->currentData().toString()},
        {"command", m_command->toPlainText()}, {"target", m_targetName->text()},
        {"property", m_property->text()}, {"value", m_value->text()}, {"variable", m_variable->text()}};
    m_actions[m_event->currentText()] = actions;
    m_list->item(row)->setText(m_type->currentData().toString());
    saveToWidget();
}
void ActionEditor::moveAction(int delta) {
    auto actions = m_actions[m_event->currentText()].toArray();
    const int row = m_list->currentRow(), next = row + delta;
    if (row < 0 || next < 0 || next >= actions.size()) return;
    const auto value = actions[row]; actions[row] = actions[next]; actions[next] = value;
    m_actions[m_event->currentText()] = actions; saveToWidget(); reloadList(next);
}

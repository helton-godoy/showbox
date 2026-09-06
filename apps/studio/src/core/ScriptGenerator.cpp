#include "ScriptGenerator.h"
#include <QAbstractButton>
#include <QCheckBox>
#include <QCryptographicHash>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLayout>
#include <QBoxLayout>
#include <QLineEdit>
#include <QRegularExpression>
#include <QTabWidget>

namespace {
QString shellQuote(QString value) {
    value.replace("'", "'\\''");
    return "'" + value + "'";
}
QString token(QString value) {
    value.replace("\\", "\\\\");
    value.replace("\"", "\\\"");
    return "\"" + value + "\"";
}
bool identifier(const QString &value) {
    static const QRegularExpression pattern("^[A-Za-z_][A-Za-z0-9_]*$");
    return pattern.match(value).hasMatch();
}
QList<QWidget *> childrenOf(QWidget *root) {
    QList<QWidget *> children;
    if (auto *tabs = qobject_cast<QTabWidget *>(root)) {
        for (int i = 0; i < tabs->count(); ++i) children.append(tabs->widget(i));
    } else if (root->layout()) {
        for (int i = 0; i < root->layout()->count(); ++i)
            if (auto *widget = root->layout()->itemAt(i)->widget()) children.append(widget);
    } else {
        children = root->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly);
    }
    return children;
}
}

ScriptGenerator::ScriptGenerator() {
    Q_INIT_RESOURCE(shell);
}

QString ScriptGenerator::buildUi(QWidget *root, bool actions) {
    m_error.clear();
    m_names.clear();
    m_types.clear();
    m_references.clear();
    m_functions.clear();
    m_dispatch.clear();
    if (!root) { m_error = "Projeto vazio."; return {}; }
    QStringList lines{"set title \"Showbox\""};
    for (auto *widget : childrenOf(root)) processWidget(widget, lines, actions);
    if (m_names.isEmpty() && m_error.isEmpty()) m_error = "Adicione componentes ao projeto.";
    for (const auto &reference : m_references) {
        const auto targetType = m_types.value(reference.first);
        if (targetType.isEmpty()) {
            m_error = "Componente de destino não encontrado: " + reference.first; break;
        }
        if (reference.second == "query" && targetType != "textbox" && targetType != "slider" &&
            targetType != "checkbox" && targetType != "radiobutton") {
            m_error = "Este componente não oferece consulta escalar: " + reference.first; break;
        }
    }
    lines.append("show");
    return m_error.isEmpty() ? lines.join('\n') + '\n' : QString();
}

QString ScriptGenerator::generateUi(QWidget *root) {
    return buildUi(root, false);
}

QString ScriptGenerator::generate(QWidget *root) {
    const QString ui = buildUi(root, true);
    if (!m_error.isEmpty()) return {};
    QFile support(":/showbox/runtime.sh");
    if (!support.open(QIODevice::ReadOnly)) {
        m_error = "Não foi possível carregar o suporte Bash.";
        return {};
    }
    const QString delimiter = "SHOWBOX_UI_" + QString::fromLatin1(
        QCryptographicHash::hash(ui.toUtf8(), QCryptographicHash::Sha256).toHex());
    QString result = "#!/usr/bin/env bash\n# Gerado pelo Showbox Studio.\n";
    result += QString::fromUtf8(support.readAll()) + '\n';
    result += "_sb_ui() {\ncat <<'" + delimiter + "'\n" + ui + delimiter + "\n}\n\n";
    result += m_functions.join('\n');
    result += "\n_sb_dispatch() {\n    case \"$1\" in\n";
    result += m_dispatch.join('\n');
    result += "\n    esac\n}\n\n_sb_run\n";
    return result;
}

void ScriptGenerator::processWidget(QWidget *widget, QStringList &lines, bool actions) {
    if (!m_error.isEmpty() || widget->property("showbox_ignore").toBool()) return;
    QString type = widget->property("showbox_type").toString().toLower();
    if (type == "button") type = "pushbutton";
    const QSet<QString> supported{"pushbutton", "label", "textbox", "checkbox", "radiobutton",
        "slider", "progressbar", "groupbox", "frame", "tabs", "page"};
    const QString name = widget->objectName();
    if (!supported.contains(type)) {
        m_error = QString("Componente ainda não exportável: %1 (%2).").arg(name, type);
        return;
    }
    if (!identifier(name) || name == "main" || name == "showbox" || m_names.contains(name)) {
        m_error = QString("Nome inválido, reservado ou repetido: %1.").arg(name);
        return;
    }
    m_names.insert(name);
    m_types.insert(name, type);
    QString title = widget->property("title").toString();
    if (type == "label" || qobject_cast<QAbstractButton *>(widget))
        title = widget->property("text").toString();
    if (title.contains('\n') || title.contains('\r') || title.contains(QChar(0))) {
        m_error = "Esta versão exporta somente textos em uma linha.";
        return;
    }
    const bool nameOnly = type == "frame" || type == "tabs" || type == "progressbar";
    QString line = "add " + type + " ";
    if (!nameOnly) line += token(title) + " ";
    line += name;
    if (type == "checkbox" || type == "radiobutton") {
        if (widget->property("checked").toBool()) line += " checked";
    }
    if (type == "pushbutton" && widget->property("checkable").toBool()) {
        line += " checkable";
        if (widget->property("checked").toBool()) line += " checked";
    }
    if (type == "slider" || type == "progressbar") {
        for (const QString &prop : {QString("minimum"), QString("maximum"), QString("value")})
            line += " " + prop + " " + QString::number(widget->property(prop.toUtf8()).toInt());
        if (widget->property("orientation").toInt() == Qt::Vertical) line += " vertical";
    }
    if (type == "textbox") {
        QWidget *input = widget->focusProxy() ? widget->focusProxy() : widget;
        const QString text = input->property("text").toString();
        if (text.contains('\n') || text.contains('\r') || text.contains(QChar(0))) {
            m_error = "Esta versão exporta somente valores em uma linha.";
            return;
        }
        line += " text " + token(text);
        if (input->property("echoMode").toInt() == QLineEdit::Password) line += " password";
        if (input->property("readOnly").toBool()) line += " readonly";
    }
    if (type == "groupbox" || type == "frame" || type == "page") {
        auto *box = qobject_cast<QBoxLayout *>(widget->layout());
        if (widget->layout() && !box) {
            m_error = "Este exportador suporta layouts lineares, não grade/formulário.";
            return;
        }
        if (box && (box->direction() == QBoxLayout::BottomToTop || box->direction() == QBoxLayout::RightToLeft)) {
            m_error = "Layout invertido ainda não é exportável."; return;
        }
        if (box && box->direction() == QBoxLayout::TopToBottom) line += " vertical";
    }
    lines.append(line);
    // set não aplica a conversão especial de \\n que add faz em títulos legados.
    if (type == "label" || type == "pushbutton")
        lines.append("set " + name + " text " + token(title));
    if (!widget->isEnabled()) lines.append("disable " + name);
    if (actions) collectActions(widget, type);
    if (type == "groupbox" || type == "frame" || type == "tabs" || type == "page") {
        for (auto *child : childrenOf(widget)) processWidget(child, lines, actions);
        lines.append("end " + type);
    }
}

void ScriptGenerator::collectActions(QWidget *widget, const QString &type) {
    const QString json = widget->property("showbox_actions").toString();
    if (json.isEmpty()) return;
    QJsonParseError parseError;
    const auto document = QJsonDocument::fromJson(json.toUtf8(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        m_error = "JSON de ações inválido em " + widget->objectName(); return;
    }
    const auto events = document.object();
    for (auto event = events.begin(); event != events.end(); ++event) {
        if (!event.value().isArray()) { m_error = "Ações devem formar uma lista."; return; }
        const auto actions = event.value().toArray();
        if (actions.isEmpty()) continue;
        const bool button = type == "pushbutton";
        const bool toggle = button && widget->property("checkable").toBool();
        const bool changed = type == "slider";
        if (!((button && !toggle && event.key() == "clicked") ||
              (toggle && (event.key() == "pressed" || event.key() == "released")) ||
              (changed && event.key() == "changed"))) {
            m_error = QString("Evento %1 não suportado para %2.").arg(event.key(), widget->objectName()); return;
        }
        const QString callback = QString("_sb_action_%1").arg(m_functions.size());
        QString function = callback + "() {\n";
        for (const auto &value : actions) {
            const auto action = value.toObject();
            const QString actionType = action["type"].toString();
            if (actionType == "set" || actionType == "query") {
                const auto target = action["target"].toString();
                if (!identifier(target)) { m_error = "Destino inválido: " + target; return; }
                m_references.append(qMakePair(target, actionType));
            }
            if (actionType == "shell") {
                const QString command = action["command"].toString();
                if (command.trimmed().isEmpty() || command.contains(QChar(0))) {
                    m_error = "Ação shell vazia ou inválida."; return;
                }
                function += command + '\n';
            } else if (actionType == "set") {
                const auto literal = action["value"].toString();
                if (!identifier(action["property"].toString()) || literal.contains('\n') ||
                    literal.contains('\r') || literal.contains(QChar(0))) {
                    m_error = "Propriedade ou valor literal inválido."; return;
                }
                function += "showbox_set " + shellQuote(action["target"].toString()) + " " +
                    shellQuote(action["property"].toString()) + " " + shellQuote(action["value"].toString()) + '\n';
            } else if (actionType == "query") {
                const QString variable = action["variable"].toString();
                if (!QRegularExpression("^[A-Za-z][A-Za-z0-9_]*$").match(variable).hasMatch()) {
                    m_error = "Variável de consulta inválida: use um nome sem $."; return;
                }
                function += "showbox_get " + shellQuote(variable) + " " + shellQuote(action["target"].toString()) + '\n';
            } else {
                m_error = "Tipo de ação ainda não exportável: " + actionType; return;
            }
        }
        function += "}\n";
        m_functions.append(function);
        const QString name = widget->objectName();
        const QString pattern = shellQuote(name + "=" + (changed ? "" : event.key())) + (changed ? "*" : "");
        m_dispatch.append("        " + pattern + ") _sb_launch " + callback + " " + shellQuote(name) +
            " " + shellQuote(event.key()) + " \"${1#*=}\" ;; ");
    }
}

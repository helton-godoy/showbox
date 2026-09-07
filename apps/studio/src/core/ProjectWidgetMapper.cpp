#include "ProjectWidgetMapper.h"
#include "Catalog.h"
#include "IStudioWidgetFactory.h"
#include "custom_table_widget.h"

#include <QAbstractButton>
#include <QBoxLayout>
#include <QComboBox>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QProgressBar>
#include <QScrollArea>
#include <QSlider>
#include <QSpinBox>
#include <QTabWidget>
#include <QTextEdit>

namespace {

QString resolvedType(const ProjectNode &node) {
    return showbox::catalog::canonicalType(node.type);
}

void appendFormItem(ProjectNode *node, QLayoutItem *item, int row,
                    const QString &role) {
    if (!item) {
        return;
    }
    if (QWidget *child = item->widget()) {
        ProjectNode childNode = ProjectWidgetMapper::toNode(child);
        childNode.positionRow = row;
        childNode.formRole = role;
        node->children.append(childNode);
    }
}

QWidget *buildChildWidget(const ProjectNode &childNode,
                          IStudioWidgetFactory *factory) {
    return ProjectWidgetMapper::toWidget(childNode, factory);
}

} // namespace

ProjectModel ProjectWidgetMapper::toModel(QWidget *root) {
    ProjectModel model;
    if (!root) {
        return model;
    }

    const QObjectList children = root->children();
    for (QObject *child : children) {
        QWidget *widget = qobject_cast<QWidget *>(child);
        if (!widget || !widget->property("showbox_type").isValid()) {
            continue;
        }
        if (widget->property("showbox_ignore").toBool()) {
            continue;
        }
        model.widgets.append(toNode(widget));
    }
    return model;
}

ProjectNode ProjectWidgetMapper::toNode(QWidget *widget) {
    ProjectNode node;
    if (!widget) {
        return node;
    }

    node.type = widget->property("showbox_type").toString();
    node.name = widget->objectName();
    if (node.type.isEmpty()) {
        return node;
    }

    readData(widget, &node);
    if (node.isContainer()) {
        fillChildren(widget, &node);
    }
    return node;
}

void ProjectWidgetMapper::readData(QWidget *widget, ProjectNode *node) {
    const QString type = node->type;

    // Ações: o texto exato escrito pelo editor é preservado byte a byte.
    node->actions = widget->property("showbox_actions").toString();

    QJsonObject props;
    auto addStr = [&props](const char *key, const QString &value) {
        if (!value.isNull()) {
            props[QLatin1String(key)] = value;
        }
    };
    auto addBool = [&props](const char *key, bool value) {
        props[QLatin1String(key)] = value;
    };
    auto addInt = [&props](const char *key, int value) {
        props[QLatin1String(key)] = value;
    };

    if (type == "textbox") {
        QLineEdit *edit = qobject_cast<QLineEdit *>(widget->focusProxy());
        if (!edit) {
            edit = widget->findChild<QLineEdit *>();
        }
        if (edit) {
            addStr("text", edit->text());
            addInt("echoMode", edit->echoMode());
            addBool("readOnly", edit->isReadOnly());
            addStr("placeholder", edit->placeholderText());
        }
        readTitleLabel(widget, &props);
    } else if (type == "textview") {
        if (auto *text = qobject_cast<QTextEdit *>(widget)) {
            addStr("plainText", text->toPlainText());
            addBool("readOnly", text->isReadOnly());
        }
    } else if (type == "combobox") {
        if (auto *combo = widget->findChild<QComboBox *>()) {
            QJsonArray items;
            for (int i = 0; i < combo->count(); ++i) {
                items.append(combo->itemText(i));
            }
            node->items = items;
            addInt("currentIndex", combo->currentIndex());
        }
        readTitleLabel(widget, &props);
    } else if (type == "listbox") {
        if (auto *list = widget->findChild<QListWidget *>()) {
            QJsonArray items;
            for (int i = 0; i < list->count(); ++i) {
                items.append(list->item(i)->text());
            }
            node->items = items;
        }
        readTitleLabel(widget, &props);
    } else if (type == "table") {
        readTable(widget, node);
    } else if (type == "page") {
        const QVariant title = widget->property("title");
        if (title.isValid()) {
            addStr("title", title.toString());
        }
    }

    if (auto *group = qobject_cast<QGroupBox *>(widget)) {
        addStr("title", group->title());
        if (group->isCheckable()) {
            addBool("checkable", true);
            addBool("checked", group->isChecked());
        }
    } else if (auto *button = qobject_cast<QAbstractButton *>(widget)) {
        addStr("text", button->text());
        addBool("checkable", button->isCheckable());
        addBool("checked", button->isChecked());
    } else if (auto *label = qobject_cast<QLabel *>(widget)) {
        addStr("text", label->text());
    }

    if (auto *spin = qobject_cast<QSpinBox *>(widget)) {
        addInt("value", spin->value());
        addInt("minimum", spin->minimum());
        addInt("maximum", spin->maximum());
        addInt("singleStep", spin->singleStep());
    }
    if (auto *slider = qobject_cast<QSlider *>(widget)) {
        addInt("value", slider->value());
        addInt("minimum", slider->minimum());
        addInt("maximum", slider->maximum());
        addInt("orientation",
               slider->orientation() == Qt::Vertical ? 2 : 1);
    }
    if (auto *progress = qobject_cast<QProgressBar *>(widget)) {
        addInt("value", progress->value());
        addInt("minimum", progress->minimum());
        addInt("maximum", progress->maximum());
        addInt("orientation",
               progress->orientation() == Qt::Vertical ? 2 : 1);
    }

    addBool("enabled", widget->isEnabled());
    addInt("x", widget->x());
    addInt("y", widget->y());
    addInt("width", widget->width());
    addInt("height", widget->height());

    node->properties = props;
}

void ProjectWidgetMapper::readTable(QWidget *widget, ProjectNode *node) {
    CustomTableWidget *custom = qobject_cast<CustomTableWidget *>(widget);
    QTableWidget *table = custom ? custom->table() : nullptr;
    if (!table) {
        return;
    }

    QJsonArray headers;
    for (int column = 0; column < table->columnCount(); ++column) {
        const QTableWidgetItem *headerItem =
            table->horizontalHeaderItem(column);
        headers.append(headerItem ? headerItem->text() : QString());
    }
    node->headers = headers;

    QJsonArray rows;
    for (int row = 0; row < table->rowCount(); ++row) {
        QJsonArray cells;
        for (int column = 0; column < table->columnCount(); ++column) {
            const QTableWidgetItem *item = table->item(row, column);
            cells.append(item ? item->text() : QString());
        }
        rows.append(cells);
    }
    node->rows = rows;
}

void ProjectWidgetMapper::readTitleLabel(QWidget *widget, QJsonObject *props) {
    if (QLabel *label = widget->findChild<QLabel *>()) {
        const QString text = label->text();
        if (!text.isEmpty()) {
            (*props)["title"] = text;
        }
    }
}

void ProjectWidgetMapper::fillChildren(QWidget *widget, ProjectNode *node) {
    if (auto *tabs = qobject_cast<QTabWidget *>(widget)) {
        node->layoutType = "tabs";
        for (int i = 0; i < tabs->count(); ++i) {
            node->children.append(toNode(tabs->widget(i)));
        }
        return;
    }

    QWidget *content = widget;
    if (auto *scroll = qobject_cast<QScrollArea *>(widget)) {
        node->layoutType = "scroll-area";
        content = scroll->widget();
    }

    QLayout *layout = content ? content->layout() : nullptr;
    if (!layout) {
        return;
    }

    if (auto *grid = qobject_cast<QGridLayout *>(layout)) {
        node->layoutType = "grid";
        for (int i = 0; i < grid->count(); ++i) {
            QWidget *child = grid->itemAt(i)->widget();
            if (!child) {
                continue;
            }
            ProjectNode childNode = toNode(child);
            int row = 0, column = 0, rowSpan = 1, columnSpan = 1;
            grid->getItemPosition(i, &row, &column, &rowSpan, &columnSpan);
            childNode.positionRow = row;
            childNode.positionColumn = column;
            childNode.positionRowSpan = rowSpan;
            childNode.positionColumnSpan = columnSpan;
            node->children.append(childNode);
        }
    } else if (auto *form = qobject_cast<QFormLayout *>(layout)) {
        node->layoutType = "form";
        for (int row = 0; row < form->rowCount(); ++row) {
            appendFormItem(node, form->itemAt(row, QFormLayout::LabelRole), row,
                           "label");
            appendFormItem(node, form->itemAt(row, QFormLayout::FieldRole), row,
                           "field");
        }
    } else {
        if (auto *box = qobject_cast<QBoxLayout *>(layout)) {
            node->layoutType = "box";
            node->layoutOrientation =
                (box->direction() == QBoxLayout::TopToBottom ||
                 box->direction() == QBoxLayout::BottomToTop)
                    ? "vertical"
                    : "horizontal";
        }
        for (int i = 0; i < layout->count(); ++i) {
            if (QWidget *child = layout->itemAt(i)->widget()) {
                node->children.append(toNode(child));
            }
        }
    }
}

QWidget *ProjectWidgetMapper::toWidget(const ProjectNode &node,
                                       IStudioWidgetFactory *factory) {
    if (!factory) {
        return nullptr;
    }
    QWidget *widget = factory->createWidget(node.type, node.name);
    if (!widget) {
        return nullptr;
    }

    applyData(widget, node);

    if (!node.children.isEmpty() && node.isContainer()) {
        QWidget *target = widget;

        // Tabs entram via addTab; páginas carregam o título da aba. A
        // factory adiciona uma página padrão, que é removida quando o modelo
        // traz páginas próprias.
        if (auto *tabs = qobject_cast<QTabWidget *>(target)) {
            while (tabs->count()) {
                QWidget *defaultPage = tabs->widget(0);
                tabs->removeTab(0);
                delete defaultPage;
            }
            for (const ProjectNode &child : node.children) {
                QWidget *page = buildChildWidget(child, factory);
                if (!page) {
                    continue;
                }
                QString title = child.properties["title"].toString();
                if (title.isEmpty()) {
                    title = child.name;
                }
                tabs->addTab(page, title);
            }
            return widget;
        }

        if (auto *scroll = qobject_cast<QScrollArea *>(target)) {
            target = scroll->widget();
        }
        QLayout *layout = target->layout();
        if (!layout) {
            return widget;
        }

        if (auto *form = qobject_cast<QFormLayout *>(layout)) {
            // Formulário: linhas explícitas com papéis label/field.
            QMap<int, QWidget *> labels;
            QMap<int, QWidget *> fields;
            int maxRow = -1;
            for (const ProjectNode &child : node.children) {
                QWidget *childWidget = buildChildWidget(child, factory);
                if (!childWidget) {
                    continue;
                }
                maxRow = qMax(maxRow, child.positionRow);
                if (child.formRole == "label") {
                    labels.insert(child.positionRow, childWidget);
                } else {
                    fields.insert(child.positionRow, childWidget);
                }
            }
            for (int row = 0; row <= maxRow; ++row) {
                QWidget *label = labels.value(row, nullptr);
                QWidget *field = fields.value(row, nullptr);
                if (label && field) {
                    form->addRow(label, field);
                } else if (label) {
                    form->addRow(label);
                } else if (field) {
                    form->addRow(field);
                }
            }
            return widget;
        }

        bool gridMode = qobject_cast<QGridLayout *>(layout) != nullptr;
        for (const ProjectNode &child : node.children) {
            if (child.type.isEmpty()) {
                continue;
            }
            QWidget *childWidget = buildChildWidget(child, factory);
            if (!childWidget) {
                continue;
            }
            if (gridMode) {
                auto *grid = qobject_cast<QGridLayout *>(layout);
                grid->addWidget(childWidget, child.positionRow,
                                child.positionColumn, child.positionRowSpan,
                                child.positionColumnSpan);
            } else {
                layout->addWidget(childWidget);
            }
            childWidget->show();
        }
    }
    return widget;
}

void ProjectWidgetMapper::applyData(QWidget *widget, const ProjectNode &node) {
    if (!node.actions.isEmpty()) {
        widget->setProperty("showbox_actions", node.actions);
    }
    applyProperties(widget, node);

    const QString type = resolvedType(node);
    if (type == "combobox") {
        if (auto *combo = widget->findChild<QComboBox *>()) {
            combo->clear();
            for (const QJsonValue &value : node.items) {
                combo->addItem(value.toString());
            }
            const int current = node.properties["currentIndex"].toInt(-1);
            if (current >= 0 && current < combo->count()) {
                combo->setCurrentIndex(current);
            }
        }
    } else if (type == "listbox") {
        if (auto *list = widget->findChild<QListWidget *>()) {
            list->clear();
            for (const QJsonValue &value : node.items) {
                list->addItem(value.toString());
            }
        }
    } else if (type == "table") {
        CustomTableWidget *custom = qobject_cast<CustomTableWidget *>(widget);
        QTableWidget *table = custom ? custom->table() : nullptr;
        if (table) {
            QStringList headers;
            for (const QJsonValue &value : node.headers) {
                headers.append(value.toString());
            }
            table->setColumnCount(headers.size());
            table->setRowCount(node.rows.size());
            table->setHorizontalHeaderLabels(headers);
            for (int row = 0; row < node.rows.size(); ++row) {
                const QJsonArray cells = node.rows[row].toArray();
                for (int column = 0; column < cells.size(); ++column) {
                    const QString text = cells[column].toString();
                    auto *item = new QTableWidgetItem(text);
                    table->setItem(row, column, item);
                }
            }
        }
    }
}

void ProjectWidgetMapper::setTitleLabel(QWidget *widget, const QString &text) {
    if (QLabel *label = widget->findChild<QLabel *>()) {
        label->setText(text);
    }
}

void ProjectWidgetMapper::applyProperties(QWidget *widget,
                                          const ProjectNode &node) {
    const QJsonObject props = node.properties;
    const QString type = resolvedType(node);
    QStringList handled;

    auto isHandled = [&handled](const QString &key) {
        return handled.contains(key);
    };

    if (type == "textbox") {
        QLineEdit *edit = qobject_cast<QLineEdit *>(widget->focusProxy());
        if (!edit) {
            edit = widget->findChild<QLineEdit *>();
        }
        if (edit) {
            if (props.contains("text")) {
                edit->setText(props["text"].toString());
                handled << "text";
            }
            if (props.contains("echoMode")) {
                edit->setEchoMode(
                    static_cast<QLineEdit::EchoMode>(props["echoMode"].toInt()));
                handled << "echoMode";
            }
            if (props.contains("readOnly")) {
                edit->setReadOnly(props["readOnly"].toBool());
                handled << "readOnly";
            }
            if (props.contains("placeholder")) {
                edit->setPlaceholderText(props["placeholder"].toString());
                handled << "placeholder";
            }
        }
        if (props.contains("title")) {
            setTitleLabel(widget, props["title"].toString());
            handled << "title";
        }
    } else if (type == "textview") {
        if (auto *text = qobject_cast<QTextEdit *>(widget)) {
            if (props.contains("plainText")) {
                text->setPlainText(props["plainText"].toString());
                handled << "plainText";
            }
            if (props.contains("readOnly")) {
                text->setReadOnly(props["readOnly"].toBool());
                handled << "readOnly";
            }
        }
    } else if (type == "combobox") {
        if (props.contains("title")) {
            setTitleLabel(widget, props["title"].toString());
            handled << "title";
        }
    } else if (type == "listbox") {
        if (props.contains("title")) {
            setTitleLabel(widget, props["title"].toString());
            handled << "title";
        }
    }

    if (type == "slider") {
        if (auto *slider = qobject_cast<QSlider *>(widget)) {
            if (props.contains("orientation")) {
                slider->setOrientation(props["orientation"].toInt() == 2
                                           ? Qt::Vertical
                                           : Qt::Horizontal);
                handled << "orientation";
            }
        }
    } else if (type == "progressbar") {
        if (auto *progress = qobject_cast<QProgressBar *>(widget)) {
            if (props.contains("orientation")) {
                progress->setOrientation(props["orientation"].toInt() == 2
                                             ? Qt::Vertical
                                             : Qt::Horizontal);
                handled << "orientation";
            }
        }
    }

    for (auto it = props.begin(); it != props.end(); ++it) {
        const QString key = it.key();
        if (key == "x" || key == "y" || key == "visible" ||
            isHandled(key)) {
            continue;
        }
        widget->setProperty(key.toUtf8().constData(), it.value().toVariant());
    }

    if (props.contains("width") && props.contains("height")) {
        widget->resize(props["width"].toInt(), props["height"].toInt());
    }
    if (props.contains("enabled")) {
        widget->setEnabled(props["enabled"].toBool());
    }
}
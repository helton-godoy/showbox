#include "PropertyEditor.h"
#include "../core/StudioCommands.h"
#include "../core/StudioController.h"
#include <QColorDialog>
#include <QComboBox>
#include <QDebug>
#include <QFontDialog>
#include <QHeaderView>
#include <QMetaProperty>
#include <QPushButton>

PropertyEditor::PropertyEditor(QWidget *parent) : QTableWidget(parent) {
  setColumnCount(2);
  setHorizontalHeaderLabels({"Propriedade", "Valor"});
  horizontalHeader()->setStretchLastSection(true);
  setAlternatingRowColors(true);

  connect(this, &QTableWidget::cellChanged, this,
          &PropertyEditor::onCellValueChanged);
}

void PropertyEditor::setTargetWidget(QWidget *widget) {
  m_target = widget;
  m_isLoading = true;

  clearContents();
  setRowCount(0);

  if (!widget) {
    m_isLoading = false;
    return;
  }

  const QMetaObject *meta = widget->metaObject();

  // Iterar sobre todas as propriedades (incluindo herdadas)
  for (int i = 0; i < meta->propertyCount(); ++i) {
    QMetaProperty prop = meta->property(i);

    // Ignorar propriedades não editáveis ou internas
    if (!prop.isWritable() || !prop.isDesignable())
      continue;

    // Vamos focar nas propriedades mais úteis por enquanto
    QVariant value = prop.read(widget);

    addPropertyRow(prop, value);
  }

  // --- Propriedade Virtual: LAYOUT (apenas para containers) ---
  QString type = widget->property("showbox_type").toString();
  if (type == "window" || type == "groupbox" || type == "frame" ||
      type == "page") {
    addLayoutPropertyRow(widget);
  }

  // --- Propriedades Virtuais: ESTILO (Cores) ---
  addStylePropertyRows(widget);

  // --- Propriedade Virtual: TITULO/TEXTO (para widgets que não têm via Meta) ---
  if (type == "page") {
    addDynamicPropertyRow(widget, "title", "Title");
  }

  m_isLoading = false;
}

void PropertyEditor::addStylePropertyRows(QWidget *widget) {
  // Adicionar Background Color
  int rowBg = rowCount();
  insertRow(rowBg);
  setItem(rowBg, 0, new QTableWidgetItem("background-color"));
  item(rowBg, 0)->setFlags(item(rowBg, 0)->flags() ^ Qt::ItemIsEditable);

  QVariant bgValue = widget->property("background-color");
  if (!bgValue.isValid())
    bgValue = widget->palette().color(QPalette::Window);

  addColorButtonForRow(rowBg, "background-color", bgValue);

  // Adicionar Text Color
  int rowFg = rowCount();
  insertRow(rowFg);
  setItem(rowFg, 0, new QTableWidgetItem("text-color"));
  item(rowFg, 0)->setFlags(item(rowFg, 0)->flags() ^ Qt::ItemIsEditable);

  QVariant fgValue = widget->property("text-color");
  if (!fgValue.isValid())
    fgValue = widget->palette().color(QPalette::WindowText);

  addColorButtonForRow(rowFg, "text-color", fgValue);
}

void PropertyEditor::addColorButtonForRow(int row, const QString &name,
                                          const QVariant &value) {
  QTableWidgetItem *valueItem = new QTableWidgetItem();
  valueItem->setData(Qt::UserRole, name);

  QColor color = value.value<QColor>();
  if (!color.isValid() && value.canConvert<QString>()) {
    color = QColor(value.toString());
  }

  QPushButton *btn = new QPushButton();
  btn->setFlat(true);
  btn->setAutoFillBackground(true);

  auto updateBtn = [btn](const QColor &c) {
    if (c.isValid()) {
      btn->setText(c.name());
      btn->setStyleSheet(QString("background-color: %1; color: %2; border: 1px "
                                 "solid #ccc; font-weight: bold;")
                             .arg(c.name())
                             .arg(c.lightness() > 128 ? "black" : "white"));
    }
  };

  updateBtn(color);

  connect(btn, &QPushButton::clicked, this, [this, name, updateBtn]() {
    if (!m_target)
      return;
    QColor current =
        m_target->property(name.toUtf8().constData()).value<QColor>();
    if (!current.isValid())
      current = Qt::white;

    QColor newColor = QColorDialog::getColor(current, this, "Escolha uma Cor");

    if (newColor.isValid()) {
      if (m_controller) {
        m_controller->undoStack()->push(
            new PropertyChangeCommand(m_target, name, current, newColor));
      } else {
        m_target->setProperty(name.toUtf8().constData(), newColor);
      }
      updateBtn(newColor);

      // Aplicar estilo visualmente ao widget (Simulação rápida via StyleSheet)
      QString style = m_target->styleSheet();
      if (name == "background-color") {
        m_target->setStyleSheet(
            style + QString("; background-color: %1;").arg(newColor.name()));
      } else if (name == "text-color") {
        m_target->setStyleSheet(style +
                                QString("; color: %1;").arg(newColor.name()));
      }
    }
  });

  setCellWidget(row, 1, btn);
  setItem(row, 1, valueItem);
}

void PropertyEditor::addLayoutPropertyRow(QWidget *widget) {
  int row = rowCount();
  insertRow(row);

  setItem(row, 0, new QTableWidgetItem("layout"));
  item(row, 0)->setFlags(item(row, 0)->flags() ^ Qt::ItemIsEditable);

  QComboBox *combo = new QComboBox();
  combo->addItem("Vertical", "vbox");
  combo->addItem("Horizontal", "hbox");

  // Detectar layout atual
  if (widget->layout()) {
    if (qobject_cast<QHBoxLayout *>(widget->layout())) {
      combo->setCurrentIndex(1);
    } else {
      combo->setCurrentIndex(0);
    }
  }

  connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
          [this, widget, combo](int index) {
            if (m_isLoading)
              return;
            QString type = combo->itemData(index).toString();

            if (m_controller) {
              m_controller->undoStack()->push(
                  new ChangeLayoutCommand(widget, type));
            }
          });

  setCellWidget(row, 1, combo);
}

void PropertyEditor::addPropertyRow(const QMetaProperty &prop,
                                    const QVariant &value) {
  int row = rowCount();
  insertRow(row);
  QString name = prop.name();

  // Nome da Propriedade (Read-only)
  QTableWidgetItem *nameItem = new QTableWidgetItem(name);
  nameItem->setFlags(nameItem->flags() ^ Qt::ItemIsEditable);
  setItem(row, 0, nameItem);

  // Item base para a coluna de valor
  QTableWidgetItem *valueItem = new QTableWidgetItem();
  valueItem->setData(Qt::UserRole, name);

  if (prop.isEnumType()) {
    QComboBox *combo = new QComboBox();
    QMetaEnum metaEnum = prop.enumerator();
    for (int i = 0; i < metaEnum.keyCount(); ++i) {
      combo->addItem(metaEnum.key(i), metaEnum.value(i));
    }

    // Selecionar valor atual
    // Para flags ou enums combinados, isso pode ser complexo, mas para enums
    // simples funciona
    int index = combo->findData(value.toInt());
    if (index >= 0)
      combo->setCurrentIndex(index);

    // Conectar sinal
    connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            [this, name, combo](int) {
              if (m_isLoading || !m_target)
                return;
              int val = combo->currentData().toInt();
              QVariant oldVal = m_target->property(name.toUtf8().constData());

              if (m_controller) {
                m_controller->undoStack()->push(
                    new PropertyChangeCommand(m_target, name, oldVal, val));
              } else {
                m_target->setProperty(name.toUtf8().constData(), val);
              }
            });

    setCellWidget(row, 1, combo);
    setItem(row, 1, valueItem); // Dummy item por trás

  } else if (value.typeId() == QMetaType::QColor ||
             name.contains("color", Qt::CaseInsensitive) ||
             name.contains("background", Qt::CaseInsensitive)) {
    addColorButtonForRow(row, name, value);

  } else if (value.typeId() == QMetaType::QFont ||
             name.contains("font", Qt::CaseInsensitive)) {
    QFont font = value.value<QFont>();

    QPushButton *btn = new QPushButton(
        QString("%1, %2pt").arg(font.family()).arg(font.pointSize()));
    btn->setFlat(true);

    connect(btn, &QPushButton::clicked, this, [this, name, font, btn]() {
      if (!m_target)
        return;
      bool ok;
      QFont current =
          m_target->property(name.toUtf8().constData()).value<QFont>();
      QFont newFont =
          QFontDialog::getFont(&ok, current, this, "Escolha uma Fonte");

      if (ok) {
        if (m_controller) {
          m_controller->undoStack()->push(
              new PropertyChangeCommand(m_target, name, current, newFont));
        } else {
          m_target->setProperty(name.toUtf8().constData(), newFont);
        }
        btn->setText(
            QString("%1, %2pt").arg(newFont.family()).arg(newFont.pointSize()));
      }
    });

    setCellWidget(row, 1, btn);
    setItem(row, 1, valueItem);

  } else if (value.typeId() == QMetaType::Bool) {
    valueItem->setCheckState(value.toBool() ? Qt::Checked : Qt::Unchecked);
    valueItem->setText(value.toBool() ? "true" : "false");
    setItem(row, 1, valueItem);
  } else {
    valueItem->setText(value.toString());
    setItem(row, 1, valueItem);
  }
}

void PropertyEditor::onCellValueChanged(int row, int col) {
  // Se estivermos carregando a tabela, ignorar (evita loops)
  if (m_isLoading || !m_target || col != 1)
    return;

  QTableWidgetItem *valueItem = item(row, 1);
  QString propName = valueItem->data(Qt::UserRole).toString();

  // Ler o tipo da propriedade original para converter corretamente
  QVariant oldValue = m_target->property(propName.toUtf8().constData());
  QVariant newValue;

  if (oldValue.typeId() == QMetaType::Bool) {
    // Se o usuário mudou o texto ou o check state
    bool b = (valueItem->text().toLower() == "true" ||
              valueItem->checkState() == Qt::Checked);
    newValue = b;

    // Sincronizar texto e checkstate visualmente
    m_isLoading = true;
    valueItem->setCheckState(b ? Qt::Checked : Qt::Unchecked);
    valueItem->setText(b ? "true" : "false");
    m_isLoading = false;
  } else {
    newValue = valueItem->text();
  }

  // Aplicar o novo valor ao widget via Command
  if (!propName.isEmpty()) {
    if (m_controller) {
      m_controller->undoStack()->push(
          new PropertyChangeCommand(m_target, propName, oldValue, newValue));
    } else {
      m_target->setProperty(propName.toUtf8().constData(), newValue);
    }
  }
}

void PropertyEditor::addDynamicPropertyRow(QWidget *widget, const QString &name,
                                           const QString &label) {
  int row = rowCount();
  insertRow(row);
  setItem(row, 0, new QTableWidgetItem(label));
  item(row, 0)->setFlags(item(row, 0)->flags() ^ Qt::ItemIsEditable);

  QTableWidgetItem *valueItem =
      new QTableWidgetItem(widget->property(name.toUtf8().constData()).toString());
  valueItem->setData(Qt::UserRole, name);
  setItem(row, 1, valueItem);
}

#ifndef PROPERTYEDITOR_H
#define PROPERTYEDITOR_H

#include <QMetaProperty>
#include <QObject>
#include <QTableWidget>

class StudioController;

class PropertyEditor : public QTableWidget {
  Q_OBJECT
public:
  explicit PropertyEditor(QWidget *parent = nullptr);

  void setController(StudioController *controller) {
    m_controller = controller;
  }

  // Carrega as propriedades do widget alvo na tabela
  void setTargetWidget(QWidget *widget);

private slots:
  // Chamado quando o usuário edita uma célula
  void onCellValueChanged(int row, int col);

private:
  QWidget *m_target = nullptr;
  StudioController *m_controller = nullptr;
  bool m_isLoading =
      false; // Flag para evitar loop de sinal durante o carregamento

  void addPropertyRow(const QMetaProperty &prop, const QVariant &value);
  void addLayoutPropertyRow(QWidget *widget);
  void addStylePropertyRows(QWidget *widget);
  void addDynamicPropertyRow(QWidget *widget, const QString &name,
                             const QString &label);
  void addColorButtonForRow(int row, const QString &name,
                            const QVariant &value);
};

#endif // PROPERTYEDITOR_H

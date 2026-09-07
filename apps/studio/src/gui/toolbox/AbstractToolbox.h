#ifndef ABSTRACTTOOLBOX_H
#define ABSTRACTTOOLBOX_H

#include <QStringList>
#include <QWidget>

/**
 * @brief Interface abstrata para implementações de Toolbox
 *
 * Permite alternar entre diferentes estilos de organização
 * da paleta de widgets (QToolBox, QTreeWidget, etc.)
 */
class AbstractToolbox : public QWidget {
  Q_OBJECT

public:
  explicit AbstractToolbox(QWidget *parent = nullptr) : QWidget(parent) {}
  virtual ~AbstractToolbox() = default;

  /**
   * @brief Adiciona uma categoria com lista de widgets
   * @param categoryName Nome da categoria (ex: "Básico", "Entrada")
   * @param items Lista de nomes de widgets
   */
  virtual void addCategory(const QString &categoryName,
                           const QStringList &items) = 0;

  /**
   * @brief Retorna o nome do estilo de toolbox
   */
  virtual QString styleName() const = 0;

signals:
  /**
   * @brief Emitido quando um widget é selecionado para arrastar
   */
  void widgetSelected(const QString &widgetType);
};

#endif // ABSTRACTTOOLBOX_H

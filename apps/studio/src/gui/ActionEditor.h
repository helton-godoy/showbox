#ifndef ACTIONEDITOR_H
#define ACTIONEDITOR_H

#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

class StudioController;

/**
 * @brief Widget para editar ações de um widget
 *
 * Permite definir comandos shell, scripts externos, ou modificações
 * de outros widgets para eventos como "clicked", "changed", etc.
 */
class ActionEditor : public QWidget {
  Q_OBJECT

public:
  explicit ActionEditor(QWidget *parent = nullptr);

  void setController(StudioController *controller) {
    m_controller = controller;
  }

  /**
   * @brief Carrega as ações do widget alvo
   */
  void setTargetWidget(QWidget *widget);

  /**
   * @brief Retorna o JSON das ações configuradas
   */
  QString getActionsJson() const;

signals:
  void actionsChanged();

private slots:
  void onAddAction();
  void onRemoveAction();
  void onMoveUp();
  void onMoveDown();
  void onActionSelected(int row);
  void onActionTypeChanged(int index);
  void onCommandChanged();
  void onTestAction();

private:
  void updateActionsJson();
  void loadActionsFromWidget();
  void saveActionsToWidget();
  void clearEditor();
  void updateActionsList();

  // Segurança
  enum SecurityLevel { Safe, Warning, Dangerous };
  SecurityLevel analyzeCommand(const QString &cmd);
  QString getSecurityWarning(const QString &cmd);
  void updateSecurityIndicator();

  QWidget *m_target = nullptr;
  StudioController *m_controller = nullptr;

  // UI Components
  QComboBox *m_eventCombo;    // clicked, changed, etc.
  QListWidget *m_actionsList; // Lista de ações configuradas
  QComboBox *m_actionType;    // shell, script, set, query, callback
  QLineEdit *m_commandLine;   // Comando ou caminho
  QLineEdit *m_targetWidget;  // Widget alvo (para set/query)
  QLineEdit *m_propertyName;  // Propriedade (para set)
  QLineEdit *m_valueLine;     // Valor (para set)
  QLineEdit *m_variableName;  // Variável (para query)
  QWidget *m_detailsPanel;    // Painel de detalhes do tipo
  QLabel *m_securityLabel;    // Indicador de segurança

  // Dados
  QJsonObject m_actionsData; // {"clicked": [...], "changed": [...]}
};

#endif // ACTIONEDITOR_H

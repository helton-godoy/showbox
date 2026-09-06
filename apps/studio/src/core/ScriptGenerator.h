#ifndef SCRIPTGENERATOR_H
#define SCRIPTGENERATOR_H

#include <QMap>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QWidget>

class ScriptGenerator {
public:
  explicit ScriptGenerator();

  /**
   * @brief Generates a Showbox CLI script from the given widget hierarchy.
   * @param root The root widget (usually the content of the Canvas, or the
   * Window).
   * @return A QString containing the full shell script with Here-Doc.
   */
  QString generate(QWidget *root);

private:
  /**
   * @brief Recursively processes a widget and its children.
   * @param widget The current widget to process.
   * @param indentLevel Indentation level for readability (optional in final
   * script but good for debug).
   */
  void processWidget(QWidget *widget, QString &script);

  /**
   * @brief Maps internal widget types/class names to Showbox CLI command types.
   * @param widget The widget to identify.
   * @return String representing the type (e.g., "label", "button").
   */
  QString getShowboxType(QWidget *widget);

  /**
   * @brief Extracts relevant properties for the 'add' command.
   */
  QString getPropertiesString(QWidget *widget);

  /**
   * @brief Extracts and stores widget actions for callback generation.
   * @param widget The widget to extract actions from.
   * @param widgetName The name of the widget.
   */
  void collectActions(QWidget *widget, const QString &widgetName);

  /**
   * @brief Generates callback functions for all collected actions.
   * @return String containing all callback function definitions.
   */
  QString generateCallbacks();

  // Armazena callbacks: widgetName_event -> lista de comandos
  QMap<QString, QStringList> m_callbacks;
};

#endif // SCRIPTGENERATOR_H

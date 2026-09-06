#ifndef PROJECTSERIALIZER_H
#define PROJECTSERIALIZER_H

#include <QList>
#include <QString>
#include <QStringList>
#include <QWidget>

class IStudioWidgetFactory;

// Serializador de projetos. Opera sobre o modelo versionado (source of truth)
// via ProjectWidgetMapper: na gravação, a árvore QWidget é convertida para o
// modelo e escrita como JSON v2; na leitura, o arquivo é migrado (v1) e
// validado antes de reconstruir os widgets.
class ProjectSerializer {
public:
  explicit ProjectSerializer();

  /**
   * @brief Saves the widget hierarchy to a JSON file (formato v2).
   * @param filename Full path to the output file.
   * @param root The root widget (usually the Canvas content container).
   * @param factory Factory used to create widgets on load.
   * @return true if successful.
   */
  bool save(const QString &filename, QWidget *root,
            IStudioWidgetFactory *factory);

  /**
   * @brief Loads a project from a JSON file and reconstructs the widgets.
   * @param filename Full path to the input file.
   * @param factory Factory to create widgets.
   * @param outWidgets List to populate with created root widgets (caller takes
   * ownership).
   * @return true if successful and the project passed validation.
   */
  bool load(const QString &filename, IStudioWidgetFactory *factory,
            QList<QWidget *> &outWidgets);

  // Problemas da última operação de leitura (versão desconhecida, arquivos
  // inválidos, erros de validação). Vazio quando a operação foi bem-sucedida.
  const QStringList &errors() const { return m_errors; }

private:
  QStringList m_errors;
};

#endif // PROJECTSERIALIZER_H
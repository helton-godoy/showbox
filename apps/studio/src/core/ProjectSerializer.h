#ifndef PROJECTSERIALIZER_H
#define PROJECTSERIALIZER_H

#include <QJsonObject>
#include <QString>
#include <QWidget>

class IStudioWidgetFactory;

class ProjectSerializer {
public:
  explicit ProjectSerializer();

  /**
   * @brief Saves the widget hierarchy to a JSON file.
   * @param filename Full path to the output file.
   * @param root The root widget (usually the Canvas content container).
   * @param factory Factory to handle specific serialization.
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
   * @return true if successful.
   */
  bool load(const QString &filename, IStudioWidgetFactory *factory,
            QList<QWidget *> &outWidgets);

private:
  QJsonObject serializeWidget(QWidget *widget, IStudioWidgetFactory *factory);
  QWidget *deserializeWidget(const QJsonObject &json,
                             IStudioWidgetFactory *factory);
};

#endif // PROJECTSERIALIZER_H

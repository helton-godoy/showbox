#ifndef PARSERMAIN_H
#define PARSERMAIN_H

#include "CommandParser.h"
#include <QMap>
#include <QObject>
#include <QPointer>
#include <QStack>
#include <QStringList>
#include <functional>

class IShowboxBuilder;
class QBoxLayout;
class QWidget;

class ParserMain : public QObject {
  Q_OBJECT
public:
  explicit ParserMain(IShowboxBuilder *builder, QObject *parent = nullptr);
  void run();
  void processLine(const QString &line);
  void setRootWidget(QWidget *widget);

signals:
  void showRequested();

private:
  void handleAdd(const QStringList &args);
  void handleEnd(const QStringList &args);
  void handleSet(const QStringList &args);
  void handleUnset(const QStringList &args);
  void handleQuery(const QStringList &args);
  void handleRemove(const QStringList &args);
  void handleClear(const QStringList &args);
  void handleStep(const QStringList &args);
  void handlePosition(const QStringList &args);

  // Helper to add created widget to current context and registry
  void addToContext(QWidget *widget);
  void connectOutputSignals(QWidget *widget);
  QString reportValue(QWidget *widget) const;
  void reportWidgets();
  QWidget *findWidget(const QString &name) const;

  void runInMainThread(std::function<void()> func);

  IShowboxBuilder *m_builder;
  CommandParser m_commandParser;
  QStack<QWidget *> m_contextStack;
  QMap<QString, QPointer<QWidget>> m_widgetRegistry;
  QPointer<QWidget> m_rootWidget;
  QPointer<QWidget> m_stepRow;
  QPointer<QWidget> m_stepColumn;
  QPointer<QBoxLayout> m_positionLayout;
  int m_positionIndex = -1;
  QPointer<QWidget> m_positionTabs;
  int m_positionTabIndex = -1;
  QPointer<QWidget> m_positionItemContainer;
  int m_positionItemIndex = -1;
};

#endif // PARSERMAIN_H

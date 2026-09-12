#include <QtTest>
#include <QListWidget>
#include <QTreeWidget>
#include "Catalog.h"
#include "gui/toolbox/ToolboxClassic.h"
#include "gui/toolbox/ToolboxTree.h"

static void populateLikeMainWindow(AbstractToolbox *toolbox) {
  for (const QString &group : showbox::catalog::toolboxGroups()) {
    QStringList items;
    for (const auto &info : showbox::catalog::widgetCatalog()) {
      if (info.toolboxGroup == group)
        items << info.displayName;
    }
    toolbox->addCategory(group, items);
  }
  for (const auto &info : showbox::catalog::widgetCatalog()) {
    if (!info.toolboxGroup.isEmpty() && !info.scriptable) {
      toolbox->markItemExperimental(
          info.displayName,
          QString("%1 ainda não é exportável").arg(info.displayName));
    }
  }
}

class tst_Toolbox : public QObject {
  Q_OBJECT

private slots:
  void classicDisablesNonExportable();
  void treeDisablesNonExportable();
  void exportableStaysEnabled();
};

void tst_Toolbox::classicDisablesNonExportable() {
  ToolboxClassic toolbox;
  populateLikeMainWindow(&toolbox);

  bool foundTable = false;
  bool foundGrid = false;
  for (QListWidget *list : toolbox.findChildren<QListWidget *>()) {
    for (QListWidgetItem *item :
         list->findItems("Table", Qt::MatchExactly)) {
      foundTable = true;
      QVERIFY(!(item->flags() & Qt::ItemIsEnabled));
      QVERIFY(!item->toolTip().isEmpty());
    }
    for (QListWidgetItem *item :
         list->findItems("GridLayout", Qt::MatchExactly)) {
      foundGrid = true;
      QVERIFY(!(item->flags() & Qt::ItemIsEnabled));
      QVERIFY(!item->toolTip().isEmpty());
    }
  }
  QVERIFY(foundTable);
  QVERIFY(foundGrid);
}

void tst_Toolbox::treeDisablesNonExportable() {
  ToolboxTree toolbox;
  populateLikeMainWindow(&toolbox);
  auto *tree = toolbox.findChild<QTreeWidget *>();
  QVERIFY(tree);

  bool foundCombo = false;
  for (QTreeWidgetItem *item : tree->findItems(
           "ComboBox", Qt::MatchExactly | Qt::MatchRecursive)) {
    if (item->childCount() > 0)
      continue;
    foundCombo = true;
    QVERIFY(!(item->flags() & Qt::ItemIsEnabled));
    QVERIFY(!(item->flags() & Qt::ItemIsDragEnabled));
    QVERIFY(!item->toolTip(0).isEmpty());
  }
  QVERIFY(foundCombo);
}

void tst_Toolbox::exportableStaysEnabled() {
  ToolboxClassic classic;
  populateLikeMainWindow(&classic);
  ToolboxTree tree;
  populateLikeMainWindow(&tree);

  bool classicButton = false;
  for (QListWidget *list : classic.findChildren<QListWidget *>()) {
    for (QListWidgetItem *item :
         list->findItems("Button", Qt::MatchExactly)) {
      classicButton = true;
      QVERIFY(item->flags() & Qt::ItemIsEnabled);
    }
  }
  QVERIFY(classicButton);

  auto *treeWidget = tree.findChild<QTreeWidget *>();
  QVERIFY(treeWidget);
  bool treeLabel = false;
  for (QTreeWidgetItem *item : treeWidget->findItems(
           "Label", Qt::MatchExactly | Qt::MatchRecursive)) {
    if (item->childCount() > 0)
      continue;
    treeLabel = true;
    QVERIFY(item->flags() & Qt::ItemIsEnabled);
  }
  QVERIFY(treeLabel);
}

QTEST_MAIN(tst_Toolbox)
#include "tst_Toolbox.moc"

#include <QtTest>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTemporaryDir>
#include <QTimer>

#include "gui/MainWindow.h"

namespace {
QWidget *findCanvasWidget(MainWindow &window, const QString &name) {
    return window.findChild<QWidget *>(name);
}

QTableWidget *innerTable(QWidget *wrapper) {
    if (!wrapper)
        return nullptr;
    return wrapper->findChild<QTableWidget *>();
}

QLineEdit *innerEdit(QWidget *wrapper) {
    if (!wrapper)
        return nullptr;
    if (auto *edit = qobject_cast<QLineEdit *>(wrapper->focusProxy()))
        return edit;
    return wrapper->findChild<QLineEdit *>();
}

void clickMessageBoxButton(QMessageBox::StandardButton which) {
    for (QWidget *top : QApplication::topLevelWidgets()) {
        auto *box = qobject_cast<QMessageBox *>(top);
        if (!box || !box->isVisible())
            continue;
        if (QAbstractButton *button = box->button(which)) {
            QTest::mouseClick(button, Qt::LeftButton);
            return;
        }
    }
}
} // namespace

class tst_DiscardProtection : public QObject {
    Q_OBJECT

private slots:
    void freshWindowIsClean();
    void pushMarksDirty();
    void saveClearsDirty();
    void directTableEditMarksDirty();
    void directTextEditMarksDirty();
    void sb019ScenarioStaysDirtyAfterUndo();
    void closeWithDiscardClosesWindow();
    void closeWithCancelKeepsWindow();
    void cleanCloseNeedsNoDialog();
};

void tst_DiscardProtection::freshWindowIsClean() {
    MainWindow window;
    QVERIFY(!window.hasUnsavedChanges());
}

void tst_DiscardProtection::pushMarksDirty() {
    MainWindow window;
    QString error;
    QVERIFY(window.automationAddWidget("label", "lbl_probe", {}, &error));
    QVERIFY2(error.isEmpty(), qPrintable(error));
    QVERIFY(window.hasUnsavedChanges());
}

void tst_DiscardProtection::saveClearsDirty() {
    MainWindow window;
    QString error;
    QVERIFY(window.automationAddWidget("label", "lbl_probe", {}, &error));
    QVERIFY(window.hasUnsavedChanges());
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString path = dir.filePath("probe.sbxproj");
    QVERIFY(window.automationSave(path, &error));
    QVERIFY2(error.isEmpty(), qPrintable(error));
    QVERIFY(!window.hasUnsavedChanges());
}

void tst_DiscardProtection::directTableEditMarksDirty() {
    MainWindow window;
    QString error;
    QVERIFY(window.automationAddWidget("table", "table_probe", {}, &error));
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QVERIFY(window.automationSave(dir.filePath("probe.sbxproj"), &error));
    QVERIFY(!window.hasUnsavedChanges());

    QWidget *wrapper = findCanvasWidget(window, "table_probe");
    QVERIFY2(wrapper, "widget table_probe nao encontrado no canvas");
    QTableWidget *table = innerTable(wrapper);
    QVERIFY2(table, "QTableWidget interno nao encontrado");
    QVERIFY(table->rowCount() > 0 && table->columnCount() > 0);
    QTableWidgetItem *item = table->item(0, 0);
    QVERIFY2(item, "celula (0,0) ausente");
    item->setText("EDITADO-SB020");

    // Edicao direta no canvas, sem undo: nao pode parecer limpa.
    QVERIFY(window.hasUnsavedChanges());
}

void tst_DiscardProtection::directTextEditMarksDirty() {
    MainWindow window;
    QString error;
    QVERIFY(window.automationAddWidget("textbox", "entry_probe", {}, &error));
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QVERIFY(window.automationSave(dir.filePath("probe.sbxproj"), &error));
    QVERIFY(!window.hasUnsavedChanges());

    QWidget *wrapper = findCanvasWidget(window, "entry_probe");
    QVERIFY2(wrapper, "widget entry_probe nao encontrado no canvas");
    QLineEdit *edit = innerEdit(wrapper);
    QVERIFY2(edit, "QLineEdit interno nao encontrado");
    edit->setText("EDITADO-SB020");

    QVERIFY(window.hasUnsavedChanges());
}

void tst_DiscardProtection::sb019ScenarioStaysDirtyAfterUndo() {
    // Replica o bloqueador SB-019: projeto salvo, edicao direta de tabela,
    // comando empilhado (agrupar/adicionar) e undo de volta. A pilha volta a
    // limpa, mas o modelo difere do salvo: fechar deve exigir confirmacao.
    MainWindow window;
    QString error;
    QVERIFY(window.automationAddWidget("table", "table1", {}, &error));
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString path = dir.filePath("cenario.sbxproj");
    QVERIFY(window.automationSave(path, &error));
    QVERIFY(!window.hasUnsavedChanges());

    QWidget *wrapper = findCanvasWidget(window, "table1");
    QVERIFY(wrapper);
    QTableWidget *table = innerTable(wrapper);
    QVERIFY(table);
    QVERIFY(table->rowCount() > 0 && table->columnCount() > 0);
    table->item(0, 1)->setText("42");

    // Comando posterior que sera desfeito (equivale ao Group/Undo do bloco F).
    QVERIFY(window.automationAddWidget("label", "lbl_extra", {}, &error));
    QVERIFY(window.hasUnsavedChanges());
    QVERIFY(window.automationUndo(&error));
    QVERIFY2(error.isEmpty(), qPrintable(error));

    // A pilha voltou ao indice limpo, mas a celula editada permanece.
    QVERIFY(window.hasUnsavedChanges());

    // Reanexar o widget do comando desfeito: sem o redo, ele ficaria orfao
    // (AddWidgetCommand guarda QPointer sem posse) e vazaria no teardown.
    QVERIFY(window.automationRedo(&error));
    QVERIFY(window.hasUnsavedChanges());

    // Salvar volta a limpar de verdade.
    QVERIFY(window.automationSave(path, &error));
    QVERIFY(!window.hasUnsavedChanges());
}

void tst_DiscardProtection::closeWithDiscardClosesWindow() {
    MainWindow window;
    window.show();
    QVERIFY(QTest::qWaitForWindowExposed(&window));
    QString error;
    QVERIFY(window.automationAddWidget("label", "lbl_probe", {}, &error));
    QVERIFY(window.hasUnsavedChanges());

    QTimer::singleShot(300, [] { clickMessageBoxButton(QMessageBox::Discard); });
    QVERIFY(window.close());
    QVERIFY(window.isHidden());
}

void tst_DiscardProtection::closeWithCancelKeepsWindow() {
    MainWindow window;
    window.show();
    QVERIFY(QTest::qWaitForWindowExposed(&window));
    QString error;
    QVERIFY(window.automationAddWidget("label", "lbl_probe", {}, &error));
    QVERIFY(window.hasUnsavedChanges());

    QTimer::singleShot(300, [] { clickMessageBoxButton(QMessageBox::Cancel); });
    QVERIFY(!window.close());
    QVERIFY(window.isVisible());
    QVERIFY(window.hasUnsavedChanges());
}

void tst_DiscardProtection::cleanCloseNeedsNoDialog() {
    MainWindow window;
    window.show();
    QVERIFY(QTest::qWaitForWindowExposed(&window));
    QVERIFY(!window.hasUnsavedChanges());

    // Sem sujeira, nenhum QMessageBox pode aparecer durante o close.
    QTimer::singleShot(300, [] {
        for (QWidget *top : QApplication::topLevelWidgets())
            QVERIFY2(!qobject_cast<QMessageBox *>(top), "dialogo inesperado em close limpo");
    });
    QVERIFY(window.close());
    QVERIFY(window.isHidden());
}

QTEST_MAIN(tst_DiscardProtection)
#include "tst_DiscardProtection.moc"

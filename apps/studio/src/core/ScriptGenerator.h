#ifndef SCRIPTGENERATOR_H
#define SCRIPTGENERATOR_H

#include <QSet>
#include <QMap>
#include <QStringList>
#include <QWidget>

class ScriptGenerator {
public:
    ScriptGenerator();
    QString generate(QWidget *root);
    QString generateUi(QWidget *root);
    QString errorString() const { return m_error; }

private:
    QString buildUi(QWidget *root, bool actions);
    void processWidget(QWidget *widget, QStringList &lines, bool actions);
    void collectActions(QWidget *widget, const QString &type);
    QString m_error;
    QSet<QString> m_names;
    QMap<QString, QString> m_types;
    QList<QPair<QString, QString>> m_references;
    QStringList m_functions;
    QStringList m_dispatch;
};
#endif

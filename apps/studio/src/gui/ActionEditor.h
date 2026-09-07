#ifndef ACTIONEDITOR_H
#define ACTIONEDITOR_H
#include <QJsonObject>
#include <QPointer>
#include <QWidget>
class QComboBox;
class QListWidget;
class QPlainTextEdit;
class QLineEdit;
class StudioController;

class ActionEditor : public QWidget {
    Q_OBJECT
public:
    explicit ActionEditor(QWidget *parent = nullptr);
    void setController(StudioController *) {}
    void setTargetWidget(QWidget *widget);
    QString getActionsJson() const;
signals:
    void actionsChanged();
    void executionRequested();
private:
    void reloadList(int row = -1);
    void loadAction(int row);
    void saveAction();
    void saveToWidget();
    void moveAction(int delta);
    QPointer<QWidget> m_target;
    QJsonObject m_actions;
    bool m_loading = false;
    QComboBox *m_event;
    QComboBox *m_type;
    QListWidget *m_list;
    QPlainTextEdit *m_command;
    QLineEdit *m_targetName;
    QLineEdit *m_property;
    QLineEdit *m_value;
    QLineEdit *m_variable;
};
#endif

#ifndef CLIBUILDER_H
#define CLIBUILDER_H

#include <IShowboxBuilder.h>
#include <QObject>
#include <QDialog>
#include <QVBoxLayout>

class CLIBuilder : public QObject, public IShowboxBuilder
{
    Q_OBJECT
public:
    explicit CLIBuilder(QObject *parent = nullptr);
    
    // Legacy
    PushButtonWidget* buildPushButton(const QString &title, const QString &name) override;

    // New Architecture
    QWidget* buildWindow(const Showbox::Models::WindowConfig& config) override;
    QWidget* buildButton(const Showbox::Models::ButtonConfig& config) override;
    QWidget* buildLabel(const Showbox::Models::LabelConfig& config) override;
    QLayout* buildLayout(const Showbox::Models::LayoutConfig& config) override;

private:
    QDialog *m_window;
    QVBoxLayout *m_layout;
};

#endif // CLIBUILDER_H

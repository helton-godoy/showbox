#ifndef SHOWBOXBUILDER_H
#define SHOWBOXBUILDER_H

#include "IShowboxBuilder.h"
#include <QObject>

class ShowboxBuilder : public QObject, public IShowboxBuilder
{
    Q_OBJECT
public:
    explicit ShowboxBuilder(QObject *parent = nullptr);

    QWidget* buildWindow(const Showbox::Models::WindowConfig& config) override;
    QWidget* buildButton(const Showbox::Models::ButtonConfig& config) override;
    QWidget* buildLabel(const Showbox::Models::LabelConfig& config) override;
    QWidget* buildLineEdit(const Showbox::Models::LineEditConfig& config) override;
    QLayout* buildLayout(const Showbox::Models::LayoutConfig& config) override;

    // Legacy support
    PushButtonWidget* buildPushButton(const QString &title, const QString &name) override;
};

#endif // SHOWBOXBUILDER_H

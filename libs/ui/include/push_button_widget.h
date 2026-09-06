#ifndef PUSH_BUTTON_WIDGET_H
#define PUSH_BUTTON_WIDGET_H

#include <QPushButton>

class PushButtonWidget : public QPushButton
{
    Q_OBJECT
public:
    explicit PushButtonWidget(const QString &text, QWidget *parent = nullptr);
    virtual ~PushButtonWidget() = default;

    // Passive View: No logic here, only visual properties if needed.
    // For now, it mirrors QPushButton.
};

#endif // PUSH_BUTTON_WIDGET_H

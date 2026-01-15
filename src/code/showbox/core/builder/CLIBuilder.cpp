#include "CLIBuilder.h"
#include <push_button_widget.h>
#include <widget_factory.h>

CLIBuilder::CLIBuilder(QObject *parent) : QObject(parent)
{
}

PushButtonWidget* CLIBuilder::buildPushButton(const QString &title, const QString &name)
{
    QWidget *w = WidgetFactory::create("pushbutton");
    auto *btn = qobject_cast<PushButtonWidget*>(w);
    if (btn) {
        btn->setText(title);
        btn->setObjectName(name);
    }
    return btn;
}
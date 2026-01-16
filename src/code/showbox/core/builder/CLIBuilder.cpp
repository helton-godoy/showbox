#include "CLIBuilder.h"
#include <push_button_widget.h>
#include <widget_factory.h>

CLIBuilder::CLIBuilder(QObject *parent) : QObject(parent)
{
    m_window = new QDialog();
    m_window->setWindowTitle("Showbox Target Architecture");
    m_layout = new QVBoxLayout(m_window);
}

PushButtonWidget* CLIBuilder::buildPushButton(const QString &title, const QString &name)
{
    QWidget *w = WidgetFactory::create("pushbutton");
    auto *btn = qobject_cast<PushButtonWidget*>(w);
    if (btn) {
        btn->setText(title);
        btn->setObjectName(name);
        m_layout->addWidget(btn);
        m_window->show();
    }
    return btn;
}

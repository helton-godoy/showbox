#include "ShowboxBuilder.h"
#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include "push_button_widget.h"

ShowboxBuilder::ShowboxBuilder(QObject *parent) : QObject(parent)
{
}

QWidget* ShowboxBuilder::buildWindow(const Showbox::Models::WindowConfig& config)
{
    auto *window = new QDialog();
    window->setWindowTitle(config.title);
    window->resize(config.width, config.height);
    
    // Default layout for new windows to allow immediate adding of widgets
    if (!window->layout()) {
        new QVBoxLayout(window);
    }
    
    return window;
}

QWidget* ShowboxBuilder::buildButton(const Showbox::Models::ButtonConfig& config)
{
    // For now using standard QPushButton, later can use PushButtonWidget if it adds value
    auto *btn = new QPushButton(config.text);
    btn->setObjectName(config.name);
    btn->setCheckable(config.checkable);
    btn->setChecked(config.checked);
    return btn;
}

QWidget* ShowboxBuilder::buildLabel(const Showbox::Models::LabelConfig& config)
{
    auto *lbl = new QLabel(config.text);
    lbl->setObjectName(config.name);
    lbl->setWordWrap(config.wordWrap);
    return lbl;
}

QLayout* ShowboxBuilder::buildLayout(const Showbox::Models::LayoutConfig& config)
{
    QLayout *layout = nullptr;
    switch (config.type) {
        case Showbox::Models::LayoutConfig::VBox:
            layout = new QVBoxLayout();
            break;
        case Showbox::Models::LayoutConfig::HBox:
            layout = new QHBoxLayout();
            break;
        default:
            layout = new QVBoxLayout();
            break;
    }
    
    if (layout) {
        layout->setSpacing(config.spacing);
        layout->setContentsMargins(config.margin, config.margin, config.margin, config.margin);
    }
    
    return layout;
}

PushButtonWidget* ShowboxBuilder::buildPushButton(const QString &title, const QString &name)
{
    // Implementation for legacy support using the new config-based methods
    Showbox::Models::ButtonConfig config;
    config.text = title;
    config.name = name;
    
    // Note: buildButton currently returns QPushButton, but legacy expects PushButtonWidget.
    // This highlights that we might need to migrate the implementation details later.
    // For now, this stub exists to satisfy the interface.
    return nullptr; 
}

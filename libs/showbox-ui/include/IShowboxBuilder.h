#ifndef ISHOWBOXBUILDER_H
#define ISHOWBOXBUILDER_H

#include <QWidget>
#include <QLayout>
#include "WidgetConfigs.h"

// Forward declaration if needed for legacy support
class PushButtonWidget;

class IShowboxBuilder
{
public:
    virtual ~IShowboxBuilder() = default;

    // --- New Architecture Methods (Phase 1 & 2) ---
    
    virtual QWidget* buildWindow(const Showbox::Models::WindowConfig& config) = 0;
    virtual QWidget* buildButton(const Showbox::Models::ButtonConfig& config) = 0;
    virtual QWidget* buildLabel(const Showbox::Models::LabelConfig& config) = 0;
    virtual QLayout* buildLayout(const Showbox::Models::LayoutConfig& config) = 0;

    // --- Legacy Methods (To be deprecated/removed) ---
    virtual PushButtonWidget* buildPushButton(const QString &title, const QString &name) = 0;
};

#endif // ISHOWBOXBUILDER_H
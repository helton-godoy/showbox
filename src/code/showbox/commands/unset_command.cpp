/*
 * Unset Command - Parser V2
 * ShowBox - GUI widgets for shell scripts
 */

#include "commands/unset_command.h"
#include "showbox.h"
#include "execution_context.h"
#include <QTimer>

void UnsetCommand::execute(ExecutionContext &context, const QList<QString> &args)
{
    QString name;
    unsigned int options = 0;
    unsigned int control = 0;
    
    for (const auto &arg : args) {
        if (arg == "enabled") { options |= DialogCommandTokens::OptionEnabled; }
        else if (arg == "visible") { options |= DialogCommandTokens::OptionVisible; }
        else if (arg == "focus") { options |= DialogCommandTokens::OptionFocus; }
        else if (arg == "stylesheet") { options |= DialogCommandTokens::OptionStyleSheet; }
        else if (arg == "checked") { control |= DialogCommandTokens::PropertyChecked; }
        else if (arg == "checkable") { control |= DialogCommandTokens::PropertyCheckable; }
        else if (name.isEmpty()) { name = arg; }
    }
    
    context.executeOnGui([&context, name, options, control]() {
        QWidget *w = name.isEmpty() ? context.dialogBox() 
                     : context.dialogBox()->findWidget(const_cast<char*>(name.toUtf8().data()));
        if (!w) return;
        
        // Handle enabled option
        if (options & DialogCommandTokens::OptionEnabled) {
            context.dialogBox()->setEnabled(w, false);
        }
        
        // Handle stylesheet option
        if (options & DialogCommandTokens::OptionStyleSheet) {
            w->setStyleSheet(QString());
            if (QWidget *proxyWidget = w->focusProxy())
                proxyWidget->setStyleSheet(QString());
        }
        
        // Handle visible option
        if (options & DialogCommandTokens::OptionVisible) {
            w->hide();
            if (QWidget *proxyWidget = w->focusProxy())
                proxyWidget->hide();
        }
        
        // Handle control properties (checked, etc.)
        if (control) {
            context.dialogBox()->setOptions(w, 0, control, nullptr);
        }
        
        // Process events to avoid races
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents
                                        | QEventLoop::ExcludeSocketNotifiers);
    });
}

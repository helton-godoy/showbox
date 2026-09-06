#include "commands/set_command.h"
#include "commands/command_utils.h"
#include "execution_context.h"
#include "logger.h"
#include "showbox.h"
#include <iostream>

void SetCommand::execute(ExecutionContext &context,
                         const QList<QString> &args) {
  ShowBox *dialog = context.dialogBox();
  if (!dialog)
    return;

  if (args.isEmpty())
    return;

  int argIndex = 0;
  QString name;
  CommandUtils::OptionDef def;

  // Check if first arg is option
  if (CommandUtils::getOption(args[0], def)) {
    // First arg is option, so name is omitted (apply to dialog)
  } else {
    name = args[0];
    argIndex++;
  }

  // We must execute everything that touches widgets on the GUI thread
  for (int i = argIndex; i < args.size(); ++i) {
    QString arg = args[i];
    if (CommandUtils::getOption(arg, def)) {
      QString text;

      bool needsText = (def.code & (DialogCommandTokens::PropertyTitle |
                                    DialogCommandTokens::PropertyText |
                                    DialogCommandTokens::PropertyIcon |
                                    DialogCommandTokens::PropertyIconSize |
                                    DialogCommandTokens::PropertyPlaceholder |
                                    DialogCommandTokens::PropertyAnimation |
                                    DialogCommandTokens::PropertyPicture |
                                    DialogCommandTokens::OptionStyleSheet |
                                    DialogCommandTokens::PropertyMinimum |
                                    DialogCommandTokens::PropertyMaximum |
                                    DialogCommandTokens::PropertyValue |
                                    DialogCommandTokens::PropertyFile |
                                    DialogCommandTokens::PropertyDate |
                                    DialogCommandTokens::PropertyFormat));

      if (needsText) {
        if (i + 1 < args.size()) {
          text = args[i + 1];
          i++; // Consume text
        }
      }

      unsigned int options = 0;
      if (!def.reset)
        options |= def.code;
      unsigned int mask = def.code;

      context.executeOnGui([dialog, name, options, mask, text]() {
        QWidget *widget = nullptr;
        if (!name.isEmpty()) {
          // Need a non-const char* for findWidget because it modifies the
          // string temporarily
          QByteArray ba = name.toUtf8();
          widget = dialog->findWidget(ba.data());
          if (!widget) {
            std::cerr << "Error: Widget '" << name.toStdString()
                      << "' not found." << std::endl;
            return;
          }
        } else {
          widget = dialog;
        }
        dialog->setOptions(widget, options, mask,
                           text.isEmpty() ? nullptr
                                          : text.toUtf8().constData());
      });
    } else {
      qCWarning(parserLog) << "Unknown option:" << arg;
    }
  }
}

std::unique_ptr<Command> SetCommandFactory::create() const {
  return std::make_unique<SetCommand>();
}
/*
 * Position Command - Parser V2
 * ShowBox - GUI widgets for shell scripts
 */

#include "commands/position_command.h"
#include "execution_context.h"
#include "showbox.h"

void PositionCommand::execute(ExecutionContext &context,
                              const QList<QString> &args) {
  if (args.isEmpty())
    return;

  bool behind = false;
  bool onto = false;
  QString name;

  for (const auto &arg : args) {
    if (arg == "behind") {
      behind = true;
    } else if (arg == "onto") {
      onto = true;
    } else if (name.isEmpty()) {
      name = arg;
    }
  }

  if (name.isEmpty())
    return;

  context.executeOnGui([&context, name, behind, onto]() {
    context.dialogBox()->position(const_cast<char *>(name.toUtf8().constData()),
                                  behind, onto);
  });
}

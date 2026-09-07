#include "commands/simple_commands.h"
#include "commands/command_utils.h"
#include "execution_context.h"
#include "showbox.h"

void ShowCommand::execute(ExecutionContext &context,
                          const QList<QString> &args) {
  QString name = args.isEmpty() ? "" : args[0];
  context.executeOnGui([&context, name]() {
    QWidget *w = name.isEmpty() ? context.dialogBox()
                                : context.dialogBox()->findWidget(
                                      const_cast<char *>(name.toUtf8().data()));
    if (w)
      context.dialogBox()->setOptions(w, DialogCommandTokens::OptionVisible,
                                      DialogCommandTokens::OptionVisible,
                                      nullptr);
  });
}

void HideCommand::execute(ExecutionContext &context,
                          const QList<QString> &args) {
  QString name = args.isEmpty() ? "" : args[0];
  context.executeOnGui([&context, name]() {
    QWidget *w = name.isEmpty() ? context.dialogBox()
                                : context.dialogBox()->findWidget(
                                      const_cast<char *>(name.toUtf8().data()));
    if (w)
      context.dialogBox()->setOptions(w, 0, DialogCommandTokens::OptionVisible,
                                      nullptr);
  });
}

void EnableCommand::execute(ExecutionContext &context,
                            const QList<QString> &args) {
  QString name = args.isEmpty() ? "" : args[0];
  context.executeOnGui([&context, name]() {
    QWidget *w = name.isEmpty() ? context.dialogBox()
                                : context.dialogBox()->findWidget(
                                      const_cast<char *>(name.toUtf8().data()));
    if (w)
      context.dialogBox()->setEnabled(w, true);
  });
}

void DisableCommand::execute(ExecutionContext &context,
                             const QList<QString> &args) {
  QString name = args.isEmpty() ? "" : args[0];
  context.executeOnGui([&context, name]() {
    QWidget *w = name.isEmpty() ? context.dialogBox()
                                : context.dialogBox()->findWidget(
                                      const_cast<char *>(name.toUtf8().data()));
    if (w)
      context.dialogBox()->setEnabled(w, false);
  });
}

void EndCommand::execute(ExecutionContext &context,
                         const QList<QString> &args) {
  QString type = args.isEmpty() ? "" : args[0];

  context.executeOnGui([&context, type]() {
    ShowBox *dialog = context.dialogBox();

    if (type == "groupbox" || type == "frame") {
      dialog->endGroup();
    } else if (type == "listbox" || type == "combobox" ||
               type == "dropdownlist" || type == "table") {
      dialog->endList();
    } else if (type == "tabs") {
      dialog->endTabs();
    } else if (type == "page") {
      dialog->endPage();
    } else {
      // Default behavior when no type specified:
      // Try to end in order of priority: list, group, page, tabs
      dialog->endGroup();
      dialog->endList();
    }
  });
}

void StepCommand::execute(ExecutionContext &context,
                          const QList<QString> &args) {
  bool vertical = !args.isEmpty() && args[0] == "vertical";
  context.executeOnGui([&context, vertical]() {
    if (vertical)
      context.dialogBox()->stepVertical();
    else
      context.dialogBox()->stepHorizontal();
  });
}

void ClearCommand::execute(ExecutionContext &context,
                           const QList<QString> &args) {
  QString name = args.isEmpty() ? "" : args[0];
  context.executeOnGui([&context, name]() {
    if (name.isEmpty())
      context.dialogBox()->clearDialog();
    else
      context.dialogBox()->clear(const_cast<char *>(name.toUtf8().data()));
  });
}

void RemoveCommand::execute(ExecutionContext &context,
                            const QList<QString> &args) {
  if (args.isEmpty())
    return;
  QString name = args[0];
  context.executeOnGui([&context, name]() {
    context.dialogBox()->removeWidget(const_cast<char *>(name.toUtf8().data()));
  });
}

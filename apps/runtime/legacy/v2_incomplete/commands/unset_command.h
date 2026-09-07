/*
 * Unset Command - Parser V2
 * ShowBox - GUI widgets for shell scripts
 */

#ifndef UNSET_COMMAND_H
#define UNSET_COMMAND_H

#include "command.h"

class UnsetCommand : public Command {
public:
  void execute(ExecutionContext &context, const QList<QString> &args) override;
};

class UnsetCommandFactory : public CommandFactory {
public:
  std::unique_ptr<Command> create() const override {
    return std::make_unique<UnsetCommand>();
  }
};

#endif // UNSET_COMMAND_H

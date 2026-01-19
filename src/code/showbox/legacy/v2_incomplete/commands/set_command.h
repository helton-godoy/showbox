#ifndef SET_COMMAND_H
#define SET_COMMAND_H

#include "command.h"

class SetCommand : public Command {
public:
  void execute(ExecutionContext &context, const QList<QString> &args) override;
};

class SetCommandFactory : public CommandFactory {
public:
  std::unique_ptr<Command> create() const override;
};

#endif // SET_COMMAND_H
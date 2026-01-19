#ifndef ADD_COMMAND_H
#define ADD_COMMAND_H

#include "command.h"

class AddCommand : public Command {
public:
  void execute(ExecutionContext &context, const QList<QString> &args) override;
};

class AddCommandFactory : public CommandFactory {
public:
  std::unique_ptr<Command> create() const override;
};

#endif // ADD_COMMAND_H
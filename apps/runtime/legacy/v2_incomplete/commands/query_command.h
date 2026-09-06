#ifndef QUERY_COMMAND_H
#define QUERY_COMMAND_H

#include "command.h"

class QueryCommand : public Command {
public:
  void execute(ExecutionContext &context, const QList<QString> &args) override;
};

class QueryCommandFactory : public CommandFactory {
public:
  std::unique_ptr<Command> create() const override;
};

#endif // QUERY_COMMAND_H

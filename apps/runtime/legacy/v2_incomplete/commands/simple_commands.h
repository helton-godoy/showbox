#ifndef SIMPLE_COMMANDS_H
#define SIMPLE_COMMANDS_H

#include "command.h"

#define DECLARE_SIMPLE_COMMAND(ClassName)                                      \
  class ClassName : public Command {                                           \
  public:                                                                      \
    void execute(ExecutionContext &context,                                    \
                 const QList<QString> &args) override;                         \
  };                                                                           \
  class ClassName##Factory : public CommandFactory {                           \
  public:                                                                      \
    std::unique_ptr<Command> create() const override {                         \
      return std::make_unique<ClassName>();                                    \
    }                                                                          \
  };

DECLARE_SIMPLE_COMMAND(ShowCommand)
DECLARE_SIMPLE_COMMAND(HideCommand)
DECLARE_SIMPLE_COMMAND(EnableCommand)
DECLARE_SIMPLE_COMMAND(DisableCommand)
DECLARE_SIMPLE_COMMAND(EndCommand)
DECLARE_SIMPLE_COMMAND(StepCommand)
DECLARE_SIMPLE_COMMAND(ClearCommand)
DECLARE_SIMPLE_COMMAND(RemoveCommand)

#endif // SIMPLE_COMMANDS_H

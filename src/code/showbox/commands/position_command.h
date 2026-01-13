/*
 * Position Command - Parser V2
 * ShowBox - GUI widgets for shell scripts
 */

#ifndef POSITION_COMMAND_H
#define POSITION_COMMAND_H

#include "command.h"

class PositionCommand : public Command {
public:
    void execute(ExecutionContext &context, const QList<QString> &args) override;
};

class PositionCommandFactory : public CommandFactory {
public:
    std::unique_ptr<Command> create() const override { return std::make_unique<PositionCommand>(); }
};

#endif // POSITION_COMMAND_H

#ifndef COMMAND_REGISTRY_H
#define COMMAND_REGISTRY_H

#include "command.h"
#include <QString>
#include <map>
#include <memory>
#include <string>

class CommandRegistry {
public:
  void registerCommand(const QString &name,
                       std::unique_ptr<CommandFactory> factory);
  std::unique_ptr<Command> createCommand(const QString &name) const;

private:
  std::map<QString, std::unique_ptr<CommandFactory>> m_registry;
};

#endif // COMMAND_REGISTRY_H

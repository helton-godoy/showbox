#include "command_registry.h"

void CommandRegistry::registerCommand(const QString &name,
                                      std::unique_ptr<CommandFactory> factory) {
  m_registry[name] = std::move(factory);
}

std::unique_ptr<Command>
CommandRegistry::createCommand(const QString &name) const {
  auto it = m_registry.find(name);
  if (it != m_registry.end()) {
    return it->second->create();
  }
  return nullptr;
}
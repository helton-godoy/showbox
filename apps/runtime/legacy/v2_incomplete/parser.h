#ifndef PARSER_H
#define PARSER_H

#include "command_registry.h"
#include "execution_context.h"
#include <QString>

class Parser {
public:
  Parser(const CommandRegistry &registry, ExecutionContext &context);
  bool parse(const QString &input);

private:
  const CommandRegistry &m_registry;
  ExecutionContext &m_context;
};

#endif // PARSER_H
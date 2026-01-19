#include "parser.h"
#include "logger.h"
#include "tokenizer.h"
#include <iostream>

Parser::Parser(const CommandRegistry &registry, ExecutionContext &context)
    : m_registry(registry), m_context(context) {}

bool Parser::parse(const QString &input) {
  Tokenizer tokenizer;
  QList<Token> allTokens = tokenizer.tokenize(input);

  if (allTokens.isEmpty()) {
    return true; // No-op
  }

  QList<QList<QString>> commandsToExecute;
  QList<QString> currentCommandArgs;

  for (const auto &token : allTokens) {
    if (token.text == ";") {
      if (!currentCommandArgs.isEmpty()) {
        commandsToExecute.append(currentCommandArgs);
        currentCommandArgs.clear();
      }
    } else {
      currentCommandArgs.append(token.text);
    }
  }
  if (!currentCommandArgs.isEmpty()) {
    commandsToExecute.append(currentCommandArgs);
  }

  bool allSuccess = true;
  for (const auto &args : commandsToExecute) {
    if (args.isEmpty())
      continue;

    QString commandName = args[0];
    auto command = m_registry.createCommand(commandName);

    if (!command) {
      qCCritical(parserLog) << "Unknown command:" << commandName;
      std::cerr << "Error: Unknown command '" << commandName.toStdString()
                << "'" << std::endl;
      allSuccess = false;
      continue;
    }

    QList<QString> commandArgs = args;
    commandArgs.removeFirst(); // Remove command name to leave only args

    command->execute(m_context, commandArgs);
  }

  return allSuccess;
}
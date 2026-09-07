#include "CommandParser.h"
#include "Tokenizer.h"

CommandParser::CommandParser() {}

void CommandParser::registerCommand(const QString &name,
                                    CommandHandler handler) {
  m_commands[name.toLower()] = handler;
}

void CommandParser::parseLine(const QString &line) {
  Tokenizer tokenizer(line);
  QStringList tokens = tokenizer.tokenize();

  if (tokens.isEmpty())
    return;

  QString verb = tokens[0].toLower();
  if (m_commands.contains(verb)) {
    m_commands[verb](tokens.mid(1));
  }
}

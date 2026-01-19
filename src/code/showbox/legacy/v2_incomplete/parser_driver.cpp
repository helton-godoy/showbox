#include "parser_driver.h"
#include <iostream>
#include <string>

ParserDriver::ParserDriver(const CommandRegistry &registry,
                           ExecutionContext &context, QObject *parent)
    : QThread(parent), m_parser(registry, context) {}

ParserDriver::~ParserDriver() {
  requestInterruption();
  wait();
}

void ParserDriver::run() {
  std::string line;
  std::string accumulatedLine;

  // Read from stdin
  while (std::getline(std::cin, line)) {
    if (isInterruptionRequested())
      break;

    // Handle line continuation with '\'
    if (!line.empty() && line.back() == '\\') {
      line.pop_back(); // Remove backslash
      accumulatedLine += line;
      continue;
    } else {
      accumulatedLine += line;
    }

    m_parser.parse(QString::fromStdString(accumulatedLine));
    accumulatedLine.clear();
  }
}
#ifndef PARSER_DRIVER_H
#define PARSER_DRIVER_H

#include "execution_context.h"
#include "parser.h"
#include <QThread>

class ParserDriver : public QThread {
  Q_OBJECT
public:
  ParserDriver(const CommandRegistry &registry, ExecutionContext &context,
               QObject *parent = nullptr);
  ~ParserDriver() override;
  void run() override;

private:
  Parser m_parser;
};

#endif // PARSER_DRIVER_H
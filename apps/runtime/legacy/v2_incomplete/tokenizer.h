#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <QList>
#include <QString>

struct Token {
  QString text;
  // We can add Type later if needed
};

class Tokenizer {
public:
  QList<Token> tokenize(const QString &input);
};

#endif // TOKENIZER_H

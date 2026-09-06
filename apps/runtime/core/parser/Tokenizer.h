#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <QString>
#include <QStringList>

class Tokenizer {
public:
  explicit Tokenizer(const QString &input);

  QStringList tokenize();

private:
  QString m_input;
};

#endif // TOKENIZER_H

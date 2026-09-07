#include "Tokenizer.h"

Tokenizer::Tokenizer(const QString &input) : m_input(input) {}

QStringList Tokenizer::tokenize() {
  QStringList tokens;
  QString currentToken;
  QChar quote;
  bool escaped = false;

  for (int i = 0; i < m_input.length(); ++i) {
    QChar c = m_input[i];

    if (escaped) {
      currentToken += (c == 'n' ? QChar('\n') : c);
      escaped = false;
      continue;
    }

    if (c == '\\') {
      escaped = true;
      continue;
    }

    if ((c == '"' || c == '\'') && quote.isNull()) {
      quote = c;
      continue;
    }

    if (c == quote) {
      quote = QChar();
      continue;
    }

    if (c.isSpace() && quote.isNull()) {
      if (!currentToken.isEmpty() ||
          (i > 0 && (m_input[i - 1] == '"' || m_input[i - 1] == '\''))) {
        tokens.append(currentToken);
        currentToken.clear();
      }
      continue;
    }

    currentToken += c;
  }

  // Handle last token
  if (!currentToken.isEmpty() ||
      (!m_input.isEmpty() &&
       (m_input.endsWith('"') || m_input.endsWith('\'')))) {
    tokens.append(currentToken);
  }

  return tokens;
}

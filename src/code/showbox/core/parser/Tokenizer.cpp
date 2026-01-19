#include "Tokenizer.h"

Tokenizer::Tokenizer(const QString &input) : m_input(input) {}

QStringList Tokenizer::tokenize() {
  QStringList tokens;
  QString currentToken;
  bool inQuotes = false;
  bool escaped = false;

  for (int i = 0; i < m_input.length(); ++i) {
    QChar c = m_input[i];

    if (escaped) {
      currentToken += c;
      escaped = false;
      continue;
    }

    if (c == '\\') {
      escaped = true;
      continue;
    }

    if (c == '"') {
      inQuotes = !inQuotes;
      // Note: We don't add the quote itself to the token
      continue;
    }

    if (c.isSpace() && !inQuotes) {
      if (!currentToken.isEmpty() || (i > 0 && m_input[i - 1] == '"')) {
        tokens.append(currentToken);
        currentToken.clear();
      }
      continue;
    }

    currentToken += c;
  }

  // Handle last token
  if (!currentToken.isEmpty() ||
      (!m_input.isEmpty() && m_input.endsWith('"'))) {
    tokens.append(currentToken);
  }

  return tokens;
}
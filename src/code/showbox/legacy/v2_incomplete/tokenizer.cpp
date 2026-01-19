#include "tokenizer.h"

QList<Token> Tokenizer::tokenize(const QString &input) {
  QList<Token> tokens;
  QString currentToken;
  bool inQuote = false;
  QChar quoteChar;

  for (int i = 0; i < input.length(); ++i) {
    QChar c = input[i];

    if (inQuote) {
      if (c == quoteChar) {
        // End of quote
        inQuote = false;
      } else {
        // Inside quote, append everything (including spaces)
        // Handle escaped quote? Standard showbox syntax says:
        // "If a token contains a white space character (including new line) it
        // must be quoted by double quote characters (`"`). To use double quote
        // character within a token (either quoted or not) the former must be
        // escaped by back slash character (`\`)."

        // So if we see backslash, check next char.
        if (c == '\\') {
          if (i + 1 < input.length()) {
            QChar nextC = input[i + 1];
            if (nextC == '"' || nextC == '\\') {
              currentToken.append(nextC);
              i++; // Skip next char
            } else {
              currentToken.append(c);
            }
          } else {
            currentToken.append(c);
          }
        } else {
          currentToken.append(c);
        }
      }
    } else {
      if (c.isSpace()) {
        if (!currentToken.isEmpty()) {
          tokens.append({currentToken});
          currentToken.clear();
        }
      } else if (c == ';') {
        if (!currentToken.isEmpty()) {
          tokens.append({currentToken});
          currentToken.clear();
        }
        tokens.append({";"}); // Command separator
      } else if (c == '"' || c == '\'') {
        inQuote = true;
        quoteChar = c;
      } else if (c == '\\') {
        // Escape outside quotes?
        // "To use double quote character within a token (either quoted or not)
        // the former must be escaped by back slash character (`\`)."
        if (i + 1 < input.length()) {
          QChar nextC = input[i + 1];
          if (nextC == '"' || nextC == '\\') {
            currentToken.append(nextC);
            i++;
          } else {
            currentToken.append(c);
          }
        } else {
          currentToken.append(c);
        }
      } else {
        currentToken.append(c);
      }
    }
  }

  if (!currentToken.isEmpty()) {
    tokens.append({currentToken});
  }

  return tokens;
}
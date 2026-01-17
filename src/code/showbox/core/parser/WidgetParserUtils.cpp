#include "WidgetParserUtils.h"

bool WidgetParserUtils::hasFlag(const QStringList &tokens, const QString &flag)
{
    for (const QString &token : tokens) {
        if (token.toLower() == flag.toLower()) {
            return true;
        }
    }
    return false;
}

QString WidgetParserUtils::findValue(const QStringList &tokens, const QString &key)
{
    for (int i = 0; i < tokens.size() - 1; ++i) {
        if (tokens[i].toLower() == key.toLower()) {
            return tokens[i + 1];
        }
    }
    return QString();
}

int WidgetParserUtils::findIntValue(const QStringList &tokens, const QString &key, int defaultValue)
{
    QString val = findValue(tokens, key);
    if (val.isEmpty()) return defaultValue;
    bool ok;
    int res = val.toInt(&ok);
    return ok ? res : defaultValue;
}

void WidgetParserUtils::populateBaseConfig(Showbox::Models::BaseConfig &config, const QStringList &tokens)
{
    // Common mappings if they appear as key-value pairs or flags
    // In V1, 'name' is usually positional but could be explicitly set if we want to be flexible.
    // However, the base config just has 'name'.
}

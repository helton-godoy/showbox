#ifndef WIDGETPARSERUTILS_H
#define WIDGETPARSERUTILS_H

#include <QString>
#include <QStringList>
#include <WidgetConfigs.h>

class WidgetParserUtils
{
public:
    static bool hasFlag(const QStringList &tokens, const QString &flag);
    static QString findValue(const QStringList &tokens, const QString &key);
    static int findIntValue(const QStringList &tokens, const QString &key, int defaultValue = 0);
    
    // Helper to populate BaseConfig properties
    static void populateBaseConfig(Showbox::Models::BaseConfig &config, const QStringList &tokens);
};

#endif // WIDGETPARSERUTILS_H

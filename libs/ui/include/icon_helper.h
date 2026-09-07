#ifndef ICON_HELPER_H
#define ICON_HELPER_H

#include <QIcon>
#include <QString>

class IconHelper
{
public:
    static QIcon loadIcon(const QString &iconSource);
};

#endif // ICON_HELPER_H

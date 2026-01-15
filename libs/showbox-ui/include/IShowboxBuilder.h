#ifndef ISHOWBOXBUILDER_H
#define ISHOWBOXBUILDER_H

#include <QString>

class PushButtonWidget;

class IShowboxBuilder
{
public:
    virtual ~IShowboxBuilder() = default;

    virtual PushButtonWidget* buildPushButton(const QString &title, const QString &name) = 0;
};

#endif // ISHOWBOXBUILDER_H

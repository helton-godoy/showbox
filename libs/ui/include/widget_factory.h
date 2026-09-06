#pragma once
#include <QWidget>
#include <QString>
#include <functional>
#include <map>

class WidgetFactory {
public:
    using CreatorFunc = std::function<QWidget*()>;

    static QWidget* create(const QString& type);
    static void registerWidget(const QString& type, CreatorFunc creator);

private:
    static std::map<QString, CreatorFunc>& registry();
};

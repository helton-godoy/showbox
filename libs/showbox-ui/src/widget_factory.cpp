#include "widget_factory.h"
#include <QPushButton>
#include <QLabel>

std::map<QString, WidgetFactory::CreatorFunc>& WidgetFactory::registry() {
    static std::map<QString, CreatorFunc> reg;
    // Bootstrap basic widgets
    if (reg.empty()) {
        reg["button"] = []() { return new QPushButton("Factory Button"); };
        reg["label"]  = []() { return new QLabel("Factory Label"); };
    }
    return reg;
}

QWidget* WidgetFactory::create(const QString& type) {
    auto& reg = registry();
    auto it = reg.find(type);
    if (it != reg.end()) {
        return it->second();
    }
    return nullptr;
}

void WidgetFactory::registerWidget(const QString& type, CreatorFunc creator) {
    registry()[type] = creator;
}

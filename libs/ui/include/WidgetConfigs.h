#ifndef WIDGET_CONFIGS_H
#define WIDGET_CONFIGS_H

#include <QString>
#include <QStringList>
#include <QList>
#include <QMap>

namespace Showbox {
namespace Models {

struct BaseConfig {
    QString name;
    
    virtual ~BaseConfig() = default;
    
    virtual bool isValid() const {
        return !name.isEmpty();
    }
};

struct WindowConfig : public BaseConfig {
    QString title;
    int width = 800;
    int height = 600;

    // Window might not strictly need a 'name' if it's the main window,
    // but usually in showbox it's implicitly 'window'.
    // Overriding isValid to be permissive for Window title.
    bool isValid() const override {
        return width > 0 && height > 0;
    }
};

struct ButtonConfig : public BaseConfig {
    QString text = "Button";
    bool checkable = false;
    bool checked = false;
    QString iconPath;
    bool apply = false;
    bool exit = false;
    bool isDefault = false;
};

struct LabelConfig : public BaseConfig {
    QString text = "Label";
    bool wordWrap = false;
    QString iconPath;
    bool animation = false;
};

struct LineEditConfig : public BaseConfig {
    QString title;
    QString text;
    QString placeholder;
    bool passwordMode = false;
};

struct ComboBoxConfig : public BaseConfig {
    QString title;
    QStringList items;
    int currentIndex = -1;
    bool editable = false;
    bool selection = false;
};

struct ListConfig : public BaseConfig {
    QString title;
    QStringList items;
    bool multipleSelection = false;
    bool activation = false;
    bool selection = false;
};

struct TableConfig : public BaseConfig {
    QStringList headers;
    QList<QStringList> rows;
    QString file;
    bool readOnly = false;
    bool selection = false;
    bool search = false;
};

struct ProgressBarConfig : public BaseConfig {
    int value = 0;
    int minimum = 0;
    int maximum = 100;
    QString format = "%p%";
    bool busy = false;
    int orientation = 1;
};

struct ChartConfig : public BaseConfig {
    enum Type { Line, Bar, Pie };
    Type type = Line;
    QString title;
    QMap<QString, double> data;
};

struct LayoutConfig : public BaseConfig {
    enum Type { VBox, HBox, Grid };
    Type type = VBox;
    int spacing = 5;
    int margin = 5;
};


struct CheckBoxConfig : public BaseConfig {
    QString text = "CheckBox";
    bool checked = false;
};

struct RadioButtonConfig : public BaseConfig {
    QString text = "RadioButton";
    bool checked = false;
};

struct SpinBoxConfig : public BaseConfig {
    int value = 0;
    int min = 0;
    int max = 100;
    int step = 1;
    QString prefix;
    QString suffix;
};

struct SliderConfig : public BaseConfig {
    int value = 0;
    int min = 0;
    int max = 100;
    int orientation = 1; // Qt::Horizontal = 0x1
};


struct CalendarConfig : public BaseConfig {
    QString date;
    QString minimum;
    QString maximum;
    bool navigation = true;
    bool selection = false;
    QString format = "yyyy-MM-dd";
};

struct TextEditConfig : public BaseConfig {
    QString text;
    QString file;
    bool readOnly = false;
    bool richText = false;
};

struct SeparatorConfig : public BaseConfig {
    int orientation = 1; // Qt::Horizontal
    int shadow = 48; // QFrame::Sunken
};


struct GroupBoxConfig : public BaseConfig {
    QString title = "Group";
    LayoutConfig layout;
    bool checkable = false;
    bool checked = false;
};

struct FrameConfig : public BaseConfig {
    LayoutConfig layout;
    int shape = 0;  // QFrame::NoFrame
    int shadow = 16; // QFrame::Plain
};

struct PageConfig : public BaseConfig {
    QString title = "Page";
    LayoutConfig layout;
};

struct TabWidgetConfig : public BaseConfig {
    QList<PageConfig> pages;
    int position = 0;
};

} // namespace Models
} // namespace Showbox

#endif // WIDGET_CONFIGS_H

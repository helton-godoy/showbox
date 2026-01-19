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

/**
 * @brief Configuração de uma ação individual
 * 
 * Tipos suportados:
 * - "shell": Executa comando shell inline
 * - "script": Executa arquivo .sh externo
 * - "set": Modifica propriedade de outro widget
 * - "query": Obtém valor de widget e armazena em variável
 * - "callback": Echo para stdout (capturado pelo script pai)
 */
struct ActionConfig {
    enum Type { Shell, Script, Set, Query, Callback };
    
    Type type = Shell;
    QString command;        // Para Shell/Script: o comando ou caminho
    QString targetWidget;   // Para Set/Query: nome do widget alvo
    QString property;       // Para Set: propriedade a modificar
    QString value;          // Para Set: novo valor
    QString variable;       // Para Query: nome da variável de destino
    
    bool isValid() const {
        switch (type) {
            case Shell:
            case Script:
            case Callback:
                return !command.isEmpty();
            case Set:
                return !targetWidget.isEmpty() && !property.isEmpty();
            case Query:
                return !targetWidget.isEmpty() && !variable.isEmpty();
        }
        return false;
    }
};

/**
 * @brief Configuração de eventos/ações para um widget
 * 
 * Mapeia eventos (clicked, changed, etc.) para listas de ações
 */
struct EventActionsConfig {
    QMap<QString, QList<ActionConfig>> events; // "clicked" -> [action1, action2, ...]
    
    bool hasActions() const { return !events.isEmpty(); }
    
    QList<ActionConfig> actionsFor(const QString &event) const {
        return events.value(event);
    }
    
    void addAction(const QString &event, const ActionConfig &action) {
        events[event].append(action);
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
    EventActionsConfig actions;  // Ações associadas ao botão
};

struct LabelConfig : public BaseConfig {
    QString text = "Label";
    bool wordWrap = false;
    QString iconPath;
};

struct LineEditConfig : public BaseConfig {
    QString text;
    QString placeholder;
    bool passwordMode = false;
};

struct ComboBoxConfig : public BaseConfig {
    QStringList items;
    int currentIndex = -1;
};

struct ListConfig : public BaseConfig {
    QStringList items;
    bool multipleSelection = false;
};

struct TableConfig : public BaseConfig {
    QStringList headers;
    QList<QStringList> rows;
};

struct ProgressBarConfig : public BaseConfig {
    int value = 0;
    int minimum = 0;
    int maximum = 100;
    QString format = "%p%";
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
    // Data atual ou selecionada pode ser adicionada aqui se necessário via QString ISO
};

struct TextEditConfig : public BaseConfig {
    QString text;
    bool readOnly = false;
    bool richText = false;
};

struct SeparatorConfig : public BaseConfig {
    int orientation = 1; // Qt::Horizontal
};


struct GroupBoxConfig : public BaseConfig {
    QString title = "Group";
    LayoutConfig layout;
};

struct FrameConfig : public BaseConfig {
    LayoutConfig layout;
};

struct PageConfig : public BaseConfig {
    QString title = "Page";
    LayoutConfig layout;
};

struct TabWidgetConfig : public BaseConfig {
    QList<PageConfig> pages;
};

} // namespace Models
} // namespace Showbox

#endif // WIDGET_CONFIGS_H

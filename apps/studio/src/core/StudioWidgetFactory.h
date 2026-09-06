#ifndef STUDIOWIDGETFACTORY_H
#define STUDIOWIDGETFACTORY_H

#include "IStudioWidgetFactory.h"
#include <QComboBox>
#include <QFormLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonObject>
#include <QListWidget>
#include <QScrollArea>
#include <QTableWidget>
#include <QVBoxLayout>
#include <ShowboxBuilder.h>
#include <WidgetConfigs.h>

class StudioWidgetFactory : public IStudioWidgetFactory {
public:
  QWidget *createWidget(const QString &type, const QString &name) override {
    ShowboxBuilder builder;
    QString t = type.toLower();
    QWidget *w = nullptr;

    if (t == "button" || t == "pushbutton") {
      Showbox::Models::ButtonConfig cfg;
      cfg.name = name;
      cfg.text = "Button";
      w = builder.buildButton(cfg);
    } else if (t == "label") {
      Showbox::Models::LabelConfig cfg;
      cfg.name = name;
      cfg.text = "Label";
      w = builder.buildLabel(cfg);
    } else if (t == "checkbox") {
      Showbox::Models::CheckBoxConfig cfg;
      cfg.name = name;
      cfg.text = "CheckBox";
      w = builder.buildCheckBox(cfg);
    } else if (t == "radiobutton") {
      Showbox::Models::RadioButtonConfig cfg;
      cfg.name = name;
      cfg.text = "RadioButton";
      w = builder.buildRadioButton(cfg);
    } else if (t == "spinbox") {
      Showbox::Models::SpinBoxConfig cfg;
      cfg.name = name;
      w = builder.buildSpinBox(cfg);
    } else if (t == "slider") {
      Showbox::Models::SliderConfig cfg;
      cfg.name = name;
      w = builder.buildSlider(cfg);
    } else if (t == "textbox" || t == "lineedit") {
      Showbox::Models::LineEditConfig cfg;
      cfg.name = name;
      cfg.placeholder = "Type here...";
      w = builder.buildLineEdit(cfg);
    } else if (t == "textview" || t == "textedit") {
      Showbox::Models::TextEditConfig cfg;
      cfg.name = name;
      cfg.text = "Edit me";
      w = builder.buildTextEdit(cfg);
    } else if (t == "groupbox") {
      Showbox::Models::GroupBoxConfig cfg;
      cfg.name = name;
      cfg.title = "Group";
      w = builder.buildGroupBox(cfg);
    } else if (t == "frame") {
      Showbox::Models::FrameConfig cfg;
      cfg.name = name;
      w = builder.buildFrame(cfg);
    } else if (t == "tabs" || t == "tabwidget") {
      Showbox::Models::TabWidgetConfig cfg;
      cfg.name = name;
      w = builder.buildTabWidget(cfg);
      
      // Adicionar uma página inicial padrão para novos TabWidgets criados visualmente
      if (auto *tabs = qobject_cast<QTabWidget *>(w)) {
        if (tabs->count() == 0) {
          QWidget *firstPage = createWidget("page", name + "_page1");
          firstPage->setProperty("title", "Tab 1");
          tabs->addTab(firstPage, "Tab 1");
        }
      }
    } else if (t == "page") {
      // Uma página é um container simples com layout vertical para abas
      w = new QWidget();
      w->setObjectName(name);
      w->setLayout(new QVBoxLayout(w));
      w->layout()->setContentsMargins(5, 5, 5, 5);
      w->layout()->setSpacing(2);
      // Garantir que a página se identifique corretamente
      w->setProperty("showbox_type", "page");
      w->setProperty("title", "New Page"); // Propriedade para o nome da aba
    } else if (t == "combobox") {
      Showbox::Models::ComboBoxConfig cfg;
      cfg.name = name;
      w = builder.buildComboBox(cfg);
    } else if (t == "listbox") {
      Showbox::Models::ListConfig cfg;
      cfg.name = name;
      w = builder.buildList(cfg);
    } else if (t == "table") {
      Showbox::Models::TableConfig cfg;
      cfg.name = name;
      cfg.headers = {"Col 1", "Col 2"};
      cfg.rows = {{"Data 1", "Data 2"}};
      w = builder.buildTable(cfg);
    } else if (t == "calendar") {
      Showbox::Models::CalendarConfig cfg;
      cfg.name = name;
      w = builder.buildCalendar(cfg);
    } else if (t == "progressbar") {
      Showbox::Models::ProgressBarConfig cfg;
      cfg.name = name;
      w = builder.buildProgressBar(cfg);
    } else if (t == "chart") {
      Showbox::Models::ChartConfig cfg;
      cfg.name = name;
      cfg.title = "Chart";
      w = builder.buildChart(cfg);
    } else if (t == "separator") {
      Showbox::Models::SeparatorConfig cfg;
      cfg.name = name;
      w = builder.buildSeparator(cfg);
    } else if (t == "scrollarea") {
      // ScrollArea container
      auto *scroll = new QScrollArea();
      scroll->setObjectName(name);
      scroll->setWidgetResizable(true);
      auto *content = new QWidget();
      content->setLayout(new QVBoxLayout(content));
      scroll->setWidget(content);
      w = scroll;
    } else if (t == "hboxlayout" || t == "vboxlayout" || t == "gridlayout" ||
               t == "formlayout") {
      // Layout containers - create a widget with the specified layout
      auto *container = new QWidget();
      container->setObjectName(name);
      container->setMinimumSize(100, 50);

      QLayout *layout = nullptr;
      if (t == "hboxlayout") {
        layout = new QHBoxLayout(container);
      } else if (t == "vboxlayout") {
        layout = new QVBoxLayout(container);
      } else if (t == "gridlayout") {
        layout = new QGridLayout(container);
      } else if (t == "formlayout") {
        layout = new QFormLayout(container);
      }

      if (layout) {
        layout->setContentsMargins(5, 5, 5, 5);
        layout->setSpacing(5);
      }

      // Visual indicator for layout type
      container->setStyleSheet("QWidget { border: 1px dashed #666; }");
      w = container;
    } else if (t == "horizontalspacer" || t == "verticalspacer") {
      // Spacers - create a widget that represents a spacer
      auto *spacer = new QWidget();
      spacer->setObjectName(name);

      if (t == "horizontalspacer") {
        spacer->setMinimumSize(40, 20);
        spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        spacer->setStyleSheet(
            "background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #333, "
            "stop:0.5 #555, stop:1 #333); border-radius: 2px;");
      } else {
        spacer->setMinimumSize(20, 40);
        spacer->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        spacer->setStyleSheet(
            "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #333, "
            "stop:0.5 #555, stop:1 #333); border-radius: 2px;");
      }
      w = spacer;
    }

    if (w) {
      setupStudioWidget(w, t);
    }
    return w;
  }

  void serializeWidget(QWidget *widget, QJsonObject &json) override {
    if (!widget)
      return;
    QString t = widget->property("showbox_type").toString();

    if (t == "table") {
      if (auto *table = qobject_cast<QTableWidget *>(widget)) {
        QJsonArray headers;
        for (int i = 0; i < table->columnCount(); ++i) {
          headers.append(table->horizontalHeaderItem(i)->text());
        }
        json["headers"] = headers;

        // TODO: Rows serialization if needed
      }
    } else if (t == "combobox") {
      if (auto *combo = qobject_cast<QComboBox *>(widget)) {
        QJsonArray items;
        for (int i = 0; i < combo->count(); ++i)
          items.append(combo->itemText(i));
        json["items"] = items;
      }
    } else if (t == "listbox") {
      if (auto *list = qobject_cast<QListWidget *>(widget)) {
        QJsonArray items;
        for (int i = 0; i < list->count(); ++i)
          items.append(list->item(i)->text());
        json["items"] = items;
      }
    }
  }

  void deserializeWidget(QWidget *widget, const QJsonObject &json) override {
    if (!widget)
      return;
    QString t = widget->property("showbox_type").toString();

    if (t == "table") {
      if (auto *table = qobject_cast<QTableWidget *>(widget)) {
        if (json.contains("headers")) {
          QJsonArray arr = json["headers"].toArray();
          table->setColumnCount(arr.size());
          QStringList headers;
          for (const auto &v : arr)
            headers << v.toString();
          table->setHorizontalHeaderLabels(headers);
        }
      }
    } else if (t == "combobox") {
      if (auto *combo = qobject_cast<QComboBox *>(widget)) {
        if (json.contains("items")) {
          combo->clear();
          QJsonArray arr = json["items"].toArray();
          for (const auto &v : arr)
            combo->addItem(v.toString());
        }
      }
    } else if (t == "listbox") {
      if (auto *list = qobject_cast<QListWidget *>(widget)) {
        if (json.contains("items")) {
          list->clear();
          QJsonArray arr = json["items"].toArray();
          for (const auto &v : arr)
            list->addItem(v.toString());
        }
      }
    }
  }

private:
  void setupStudioWidget(QWidget *widget, const QString &type) {
    if (!widget)
      return;
    // Atributo para garantir que o widget aceite eventos mas possa ser
    // "gerenciado" pelo Studio
    widget->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    
    // Anotar o tipo para o ScriptGenerator usando nomes CLI
    QString cliName = type.toLower();
    // Normalizar para CLI names
    if (cliName == "pushbutton") cliName = "button";
    else if (cliName == "lineedit") cliName = "textbox";
    else if (cliName == "textedit") cliName = "textview";
    else if (cliName == "tabwidget") cliName = "tabs";
    else if (cliName == "dropdownlist") cliName = "combobox";
    else if (cliName == "line") cliName = "separator";
    
    widget->setProperty("showbox_type", cliName);
  }
};

#endif // STUDIOWIDGETFACTORY_H
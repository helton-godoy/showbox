#include "commands/add_command.h"
#include "logger.h"
#include "execution_context.h"
#include "showbox.h"
#include <iostream>

void AddCommand::execute(ExecutionContext &context, const QList<QString> &args)
{
    if (args.isEmpty()) {
        std::cerr << "Error: 'add' command requires a widget type." << std::endl;
        return;
    }

    QString type = args[0];
    ShowBox *dialog = context.dialogBox();
    if (!dialog) return;

    // Helper for parsing options
    auto parseGeneric = [&](int &posIndex, bool &inOptions, QString &title, QString &name, const QString &arg) {
        if (!inOptions) {
            if (posIndex == 0) title = arg;
            else if (posIndex == 1) name = arg;
            posIndex++;
        }
    };

    if (type == "pushbutton") {
        QString title, name;
        bool apply = false, exit = false, def = false;
        
        int posIndex = 0;
        bool inOptions = false;

        for (int i = 1; i < args.size(); ++i) {
            QString arg = args[i];
            if (arg == "apply") { apply = true; inOptions = true; }
            else if (arg == "exit") { exit = true; inOptions = true; }
            else if (arg == "default") { def = true; inOptions = true; }
            else if (arg == "checkable" || arg == "checked") { inOptions = true; } 
            else parseGeneric(posIndex, inOptions, title, name, arg);
        }
        context.executeOnGui([=]() {
            dialog->addPushButton(title.toUtf8().constData(), name.toUtf8().constData(), apply, exit, def);
        });
    } 
    else if (type == "label") {
        QString title, name;
        ShowBox::ContentType content = ShowBox::TextContent;
        
        int posIndex = 0;
        bool inOptions = false;

        for (int i = 1; i < args.size(); ++i) {
            QString arg = args[i];
            if (arg == "picture") { content = ShowBox::PixmapContent; inOptions = true; }
            else if (arg == "animation") { content = ShowBox::MovieContent; inOptions = true; }
            else parseGeneric(posIndex, inOptions, title, name, arg);
        }
        context.executeOnGui([=]() {
            dialog->addLabel(title.toUtf8().constData(), name.toUtf8().constData(), content);
        });
    }
    else if (type == "checkbox") {
        QString title, name;
        bool checked = false;
        
        int posIndex = 0;
        bool inOptions = false;

        for (int i = 1; i < args.size(); ++i) {
            QString arg = args[i];
            if (arg == "checked") { checked = true; inOptions = true; }
            else parseGeneric(posIndex, inOptions, title, name, arg);
        }
        context.executeOnGui([=]() {
            dialog->addCheckBox(title.toUtf8().constData(), name.toUtf8().constData(), checked);
        });
    }
    else if (type == "radiobutton") {
        QString title, name;
        bool checked = false;
        
        int posIndex = 0;
        bool inOptions = false;

        for (int i = 1; i < args.size(); ++i) {
            QString arg = args[i];
            if (arg == "checked") { checked = true; inOptions = true; }
            else parseGeneric(posIndex, inOptions, title, name, arg);
        }
        context.executeOnGui([=]() {
            dialog->addRadioButton(title.toUtf8().constData(), name.toUtf8().constData(), checked);
        });
    }
    else if (type == "frame") {
        QString name;
        bool vertical = false; 
        unsigned int style = 0;
        
        bool inOptions = false;
        for (int i = 1; i < args.size(); ++i) {
            QString arg = args[i];
            if (arg == "vertical") { vertical = true; inOptions = true; }
            else if (arg == "horizontal") { vertical = false; inOptions = true; }
            else if (arg == "plain") { style |= DialogCommandTokens::PropertyPlain; inOptions = true; }
            else if (arg == "raised") { style |= DialogCommandTokens::PropertyRaised; inOptions = true; }
            else if (arg == "sunken") { style |= DialogCommandTokens::PropertySunken; inOptions = true; }
            else if (arg == "noframe") { style |= DialogCommandTokens::PropertyNoframe; inOptions = true; }
            else if (arg == "box") { style |= DialogCommandTokens::PropertyBox; inOptions = true; }
            else if (arg == "panel") { style |= DialogCommandTokens::PropertyPanel; inOptions = true; }
            else if (arg == "styled") { style |= DialogCommandTokens::PropertyStyled; inOptions = true; }
            else {
                if (!inOptions) name = arg;
            }
        }
        context.executeOnGui([=]() {
            dialog->addFrame(name.toUtf8().constData(), vertical, style);
        });
    }
    else if (type == "groupbox") {
        QString title, name;
        bool vertical = false; 
        bool checkable = false;
        bool checked = false;
        
        int posIndex = 0;
        bool inOptions = false;
        for (int i = 1; i < args.size(); ++i) {
            QString arg = args[i];
            if (arg == "vertical") { vertical = true; inOptions = true; }
            else if (arg == "horizontal") { vertical = false; inOptions = true; }
            else if (arg == "checkable") { checkable = true; inOptions = true; }
            else if (arg == "checked") { checked = true; inOptions = true; }
            else parseGeneric(posIndex, inOptions, title, name, arg);
        }
        context.executeOnGui([=]() {
            dialog->addGroupBox(title.toUtf8().constData(), name.toUtf8().constData(), vertical, checkable, checked);
        });
    }
    else if (type == "tabs") {
        QString name;
        unsigned int position = 0; 
        
        bool inOptions = false;
        for (int i = 1; i < args.size(); ++i) {
            QString arg = args[i];
            if (arg == "top") { position = DialogCommandTokens::PropertyPositionTop; inOptions = true; }
            else if (arg == "bottom") { position = DialogCommandTokens::PropertyPositionBottom; inOptions = true; }
            else if (arg == "left") { position = DialogCommandTokens::PropertyPositionLeft; inOptions = true; }
            else if (arg == "right") { position = DialogCommandTokens::PropertyPositionRight; inOptions = true; }
            else {
                if (!inOptions) name = arg;
            }
        }
        context.executeOnGui([=]() {
            dialog->addTabs(name.toUtf8().constData(), position);
        });
    }
    else if (type == "page") {
        QString title, name, icon;
        bool current = false;
        
        int posIndex = 0; 
        bool inOptions = false;
        for (int i = 1; i < args.size(); ++i) {
            QString arg = args[i];
            if (arg == "current") { current = true; inOptions = true; }
            else {
                if (!inOptions) {
                    if (posIndex == 0) title = arg;
                    else if (posIndex == 1) name = arg;
                    else if (posIndex == 2) icon = arg;
                    posIndex++;
                }
            }
        }
        context.executeOnGui([=]() {
            dialog->addPage(title.toUtf8().constData(), name.toUtf8().constData(), icon.isEmpty() ? nullptr : icon.toUtf8().constData(), current);
        });
    }
    else if (type == "listbox") {
        QString title, name;
        bool activation = false, selection = false;
        
        int posIndex = 0;
        bool inOptions = false;
        for (int i = 1; i < args.size(); ++i) {
            QString arg = args[i];
            if (arg == "activation") { activation = true; inOptions = true; }
            else if (arg == "selection") { selection = true; inOptions = true; }
            else parseGeneric(posIndex, inOptions, title, name, arg);
        }
        context.executeOnGui([=]() {
            dialog->addListBox(title.toUtf8().constData(), name.toUtf8().constData(), activation, selection);
        });
    }
    else if (type == "combobox") {
        QString title, name;
        bool selection = false;
        
        int posIndex = 0;
        bool inOptions = false;
        for (int i = 1; i < args.size(); ++i) {
            QString arg = args[i];
            if (arg == "selection") { selection = true; inOptions = true; }
            else parseGeneric(posIndex, inOptions, title, name, arg);
        }
        context.executeOnGui([=]() {
            dialog->addComboBox(title.toUtf8().constData(), name.toUtf8().constData(), false, selection); 
        });
    }
    else if (type == "dropdownlist") {
        QString title, name;
        bool selection = false;
        
        int posIndex = 0;
        bool inOptions = false;
        for (int i = 1; i < args.size(); ++i) {
            QString arg = args[i];
            if (arg == "selection") { selection = true; inOptions = true; }
            else parseGeneric(posIndex, inOptions, title, name, arg);
        }
        context.executeOnGui([=]() {
            dialog->addComboBox(title.toUtf8().constData(), name.toUtf8().constData(), false, selection); 
        });
    }
    else if (type == "item") {
        QString title, icon;
        bool current = false;
        
        int posIndex = 0; 
        bool inOptions = false;
        for (int i = 1; i < args.size(); ++i) {
            QString arg = args[i];
            if (arg == "current") { current = true; inOptions = true; }
            else {
                if (!inOptions) {
                    if (posIndex == 0) title = arg;
                    else if (posIndex == 1) icon = arg;
                    posIndex++;
                }
            }
        }
        context.executeOnGui([=]() {
            dialog->addItem(title.toUtf8().constData(), icon.isEmpty() ? nullptr : icon.toUtf8().constData(), current);
        });
    }
    else if (type == "progressbar") {
        QString name;
        bool vertical = false; 
        bool busy = false;
        
        bool inOptions = false;
        for (int i = 1; i < args.size(); ++i) {
            QString arg = args[i];
            if (arg == "vertical") { vertical = true; inOptions = true; }
            else if (arg == "horizontal") { vertical = false; inOptions = true; }
            else if (arg == "busy") { busy = true; inOptions = true; }
            else {
                if (!inOptions) name = arg;
            }
        }
        context.executeOnGui([=]() {
            dialog->addProgressBar(name.toUtf8().constData(), vertical, busy);
        });
    }
    else if (type == "slider") {
        QString name;
        bool vertical = false;
        int min = 0;
        int max = 100;
        
        int posIndex = 0; 
        bool inOptions = false;
        
        for (int i = 1; i < args.size(); ++i) {
            QString arg = args[i];
            if (arg == "vertical") { vertical = true; inOptions = true; }
            else if (arg == "horizontal") { vertical = false; inOptions = true; }
            else {
                if (!inOptions) {
                    if (posIndex == 0) name = arg;
                    else if (posIndex == 1) min = arg.toInt();
                    else if (posIndex == 2) max = arg.toInt();
                    posIndex++;
                }
            }
        }
        context.executeOnGui([=]() {
            dialog->addSlider(name.toUtf8().constData(), vertical, min, max);
        });
    }
    else if (type == "textview") {
        QString name, file;
        
        int posIndex = 0;
        bool inOptions = false;
        for (int i = 1; i < args.size(); ++i) {
            QString arg = args[i];
            if (!inOptions) {
                if (posIndex == 0) name = arg;
                else if (posIndex == 1) file = arg;
                posIndex++;
            }
        }
        context.executeOnGui([=]() {
            dialog->addTextView(name.toUtf8().constData(), file.isEmpty() ? nullptr : file.toUtf8().constData());
        });
    }
    else if (type == "separator") {
        QString name;
        bool vertical = false; 
        unsigned int style = DialogCommandTokens::PropertySunken; 
        
        bool inOptions = false;
        for (int i = 1; i < args.size(); ++i) {
            QString arg = args[i];
            if (arg == "vertical") { vertical = true; inOptions = true; }
            else if (arg == "horizontal") { vertical = false; inOptions = true; }
            else if (arg == "plain") { style = DialogCommandTokens::PropertyPlain; inOptions = true; }
            else if (arg == "raised") { style = DialogCommandTokens::PropertyRaised; inOptions = true; }
            else if (arg == "sunken") { style = DialogCommandTokens::PropertySunken; inOptions = true; }
            else {
                if (!inOptions) name = arg;
            }
        }
        context.executeOnGui([=]() {
            dialog->addSeparator(name.isEmpty() ? nullptr : name.toUtf8().constData(), vertical, style);
        });
    }
    else if (type == "space") {
        int size = 1;
        if (args.size() > 1) size = args[1].toInt();
        if (size <= 0) size = 1;
        context.executeOnGui([=]() {
            dialog->addSpace(size);
        });
    }
    else if (type == "stretch") {
        int stretch = 1;
        if (args.size() > 1) stretch = args[1].toInt();
        context.executeOnGui([=]() {
            dialog->addStretch(stretch);
        });
    }
    else if (type == "calendar") {
        QString title, name, date, min, max, format;
        bool selection = false;
        
        int posIndex = 0;
        bool inOptions = false;
        for (int i = 1; i < args.size(); ++i) {
            QString arg = args[i];
            if (arg == "selection") { selection = true; inOptions = true; }
            else if (arg == "date" && i + 1 < args.size()) { date = args[++i]; inOptions = true; }
            else if (arg == "minimum" && i + 1 < args.size()) { min = args[++i]; inOptions = true; }
            else if (arg == "maximum" && i + 1 < args.size()) { max = args[++i]; inOptions = true; }
            else if (arg == "format" && i + 1 < args.size()) { format = args[++i]; inOptions = true; }
            else parseGeneric(posIndex, inOptions, title, name, arg);
        }
        context.executeOnGui([=]() {
            dialog->addCalendar(title.toUtf8().constData(), name.toUtf8().constData(), 
                               date.isEmpty() ? nullptr : date.toUtf8().constData(),
                               min.isEmpty() ? nullptr : min.toUtf8().constData(),
                               max.isEmpty() ? nullptr : max.toUtf8().constData(),
                               format.isEmpty() ? nullptr : format.toUtf8().constData(),
                               selection);
        });
    }
    else if (type == "table") {
        QString headers, name, file;
        bool readonly = false;
        bool selection = false;
        bool search = false;

        int posIndex = 0;
        bool inOptions = false;
        for (int i = 1; i < args.size(); ++i) {
            QString arg = args[i];
            if (arg == "readonly") { readonly = true; inOptions = true; }
            else if (arg == "selection") { selection = true; inOptions = true; }
            else if (arg == "search") { search = true; inOptions = true; }
            else if (arg == "file" && i + 1 < args.size()) { file = args[++i]; inOptions = true; }
            else {
                if (!inOptions) {
                    if (posIndex == 0) headers = arg;
                    else if (posIndex == 1) name = arg;
                    posIndex++;
                }
            }
        }
        context.executeOnGui([=]() {
            dialog->addTable(headers.toUtf8().constData(), name.toUtf8().constData(),
                            file.isEmpty() ? nullptr : file.toUtf8().constData(),
                            readonly, selection, search);
        });
    }
    else if (type == "textbox") {
        QString title, name, text, placeholder;
        bool password = false;
        
        int posIndex = 0;
        bool inOptions = false;
        for (int i = 1; i < args.size(); ++i) {
            QString arg = args[i];
            if (arg == "password") { password = true; inOptions = true; }
            else if (arg == "text" && i + 1 < args.size()) { text = args[++i]; inOptions = true; }
            else if (arg == "placeholder" && i + 1 < args.size()) { placeholder = args[++i]; inOptions = true; }
            else parseGeneric(posIndex, inOptions, title, name, arg);
        }
        context.executeOnGui([=]() {
            dialog->addTextBox(title.toUtf8().constData(), name.toUtf8().constData(),
                              text.isEmpty() ? nullptr : text.toUtf8().constData(),
                              placeholder.isEmpty() ? nullptr : placeholder.toUtf8().constData(),
                              password);
        });
    }
    else if (type == "chart") {
        QString title, name;
        
        int posIndex = 0;
        bool inOptions = false;
        for (int i = 1; i < args.size(); ++i) {
            QString arg = args[i];
            parseGeneric(posIndex, inOptions, title, name, arg);
        }
        context.executeOnGui([=]() {
            dialog->addChart(title.toUtf8().constData(), name.toUtf8().constData());
        });
    }
    else {
        qCWarning(parserLog) << "Unsupported widget type in AddCommand:" << type;
    }
}

std::unique_ptr<Command> AddCommandFactory::create() const
{
    return std::make_unique<AddCommand>();
}
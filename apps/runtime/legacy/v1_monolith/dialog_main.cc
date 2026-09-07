/*
 * GUI widgets for shell scripts - showbox version 1.0
 *
 * Copyright (C) 2015-2016, 2020 Andriy Martynets <andy.martynets@gmail.com>
 *------------------------------------------------------------------------------
 * This file is part of showbox.
 *
 * Showbox is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Showbox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with showbox. If not, see http://www.gnu.org/licenses/.
 *------------------------------------------------------------------------------
 */

#include "showbox.h"
#include "theme_manager.h"
#include "command_registry.h"
#include "parser_driver.h"
#include "execution_context.h"
#include "commands/add_command.h"
#include "commands/set_command.h"
#include "commands/query_command.h"
#include "commands/simple_commands.h"
#include "commands/position_command.h"
#include "commands/unset_command.h"

/*  EXIT CODES */
#define E_SUCCESS 0
#define E_ARG 1

#define PROGRAM_NAME "showbox"
#define VERSION "1.0"

static const char *about_message =
    PROGRAM_NAME " v" VERSION "\n"
                 "Copyright (C) 2015-2016, 2020 Andriy Martynets "
                 "<andy.martynets@gmail.com>\n"
                 "License GPLv3+: GNU GPL version 3 or later "
                 "<http://gnu.org/licenses/gpl.html>.\n"
                 "This program comes with ABSOLUTELY NO WARRANTY.\n"
                 "This is free software, and you are welcome to redistribute it\n"
                 "under certain conditions. See the GNU GPL for details.\n\n"
                 "More information on <https://github.com/martynets/showbox/>.\n";

static const char *about_html_message =
    "<h3>" PROGRAM_NAME " version " VERSION "</h3>"
    "<p><b>Copyright (C) 2015-2016, 2020 Andriy Martynets </b><a href=\""
    "mailto:andy.martynets@gmail.com\">andy.martynets@gmail.com</a></p>"
    "<p><b>License GPLv3+:</b> GNU GPL version 3 or later <a href="
    "\"http://gnu.org/licenses/gpl.html\">"
    "http://gnu.org/licenses/gpl.html</a>.</p>"
    "<p>This program comes with ABSOLUTELY NO WARRANTY. "
    "This is free software, and you are welcome to redistribute it "
    "under certain conditions. See the GNU GPL for details.</p>"
    "<p>More information on <a href="
    "\"https://github.com/martynets/showbox/\">"
    "https://github.com/martynets/showbox/</a>.</p>";

static const char *default_title = PROGRAM_NAME " v" VERSION;

static void help();
static void version();

int main(int argc, char *argv[])
{
    QApplication::setApplicationName(PROGRAM_NAME);
    QApplication::setApplicationVersion(VERSION);
    QApplication::setDesktopFileName("showbox");
    QApplication app(argc, argv);
    bool resizable = false;
    bool hidden = false;
    bool useNewParser = false;

    // Consider QApplication has removed everything it recognized...
    for (int i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help"))
        {
            help();
            return E_SUCCESS;
        }
        if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--version"))
        {
            version();
            return E_SUCCESS;
        }
        if (!strcmp(argv[i], "-r") || !strcmp(argv[i], "--resizable"))
        {
            resizable = true;
            continue;
        }
        if (!strcmp(argv[i], "-d") || !strcmp(argv[i], "--hidden"))
        {
            hidden = true;
            continue;
        }
        if (!strcmp(argv[i], "--new-parser"))
        {
            useNewParser = true;
            continue;
        }
        fprintf(stderr, "Error: Unrecognized option %s\n", argv[i]);
        return E_ARG;
    }

    ShowBox dialog(default_title, about_html_message, resizable);
    ThemeManager themeManager;
    dialog.setThemeManager(&themeManager);

    if (useNewParser)
    {
        CommandRegistry registry;
        registry.registerCommand("add", std::make_unique<AddCommandFactory>());
        registry.registerCommand("set", std::make_unique<SetCommandFactory>());
        registry.registerCommand("query", std::make_unique<QueryCommandFactory>());
        registry.registerCommand("show", std::make_unique<ShowCommandFactory>());
        registry.registerCommand("hide", std::make_unique<HideCommandFactory>());
        registry.registerCommand("enable", std::make_unique<EnableCommandFactory>());
        registry.registerCommand("disable", std::make_unique<DisableCommandFactory>());
        registry.registerCommand("end", std::make_unique<EndCommandFactory>());
        registry.registerCommand("step", std::make_unique<StepCommandFactory>());
        registry.registerCommand("clear", std::make_unique<ClearCommandFactory>());
        registry.registerCommand("remove", std::make_unique<RemoveCommandFactory>());
        registry.registerCommand("position", std::make_unique<PositionCommandFactory>());
        registry.registerCommand("unset", std::make_unique<UnsetCommandFactory>());

        ExecutionContext context(&dialog);
        ParserDriver driver(registry, context, &dialog);
        driver.start();

        dialog.setAttribute(Qt::WA_DeleteOnClose, false);
        if (!hidden)
            dialog.show();

        return QCoreApplication::exec();
    }
    else
    {
        DialogParser parser(&dialog);

        parser.start();

        dialog.setAttribute(Qt::WA_DeleteOnClose, false);
        if (!hidden)
            dialog.show();

        return QCoreApplication::exec();
    }
}

static void version()
{
    puts(about_message);
}

static void help()
{
    const char *usage =
        "Usage: " PROGRAM_NAME " [options]\n"
        "Translate commands on stdin into widgets of GUI showbox and output "
        "user\n"
        "actions to stdout.\n"
        "\n"
        "In addition to standard Qt options the following ones are supported:\n"
        "  -h, --help  display this help and exit\n"
        "  -v, --version output version information and exit\n"
        "  -r, --resizable make the dialog resizable\n"
        "  -d, --hidden  don't show the dialog until explicit 'show' command\n"
        "  --new-parser use the new V2 parser engine (recommended)\n"
        "\n"
        "Supported commands:\n"
        "  - add type [title] [name] [options] [text] [auxtext]\n"
        "  - clear [name]\n"
        "  - disable [name]\n"
        "  - enable [name]\n"
        "  - end [type]\n"
        "  - hide [name]\n"
        "  - position [options] name\n"
        "  - query\n"
        "  - remove name\n"
        "  - set [name] options [text]\n"
        "  - show [name]\n"
        "  - step [direction]\n"
        "  - unset [name] options\n"
        "\n"
        "Supported widgets (types):\n"
        "  Standard: checkbox, combobox, frame, groupbox, label, listbox, page,\n"
        "            progressbar, pushbutton, radiobutton, separator, slider,\n"
        "            space, stretch, tabs, textbox, textview\n"
        "  Extended: calendar, chart, table\n"
        "\n"
        "Output format:\n"
        "  - on a pushbutton click:\n"
        "    <pushbutton name>=clicked\n"
        "  - on a toggle pushbutton click:\n"
        "    <pushbutton name>=pressed | released\n"
        "  - on a slider move:\n"
        "    <slider name>=<value>\n"
        "  - on item selection in a list box, drop-down list or combobox with\n"
        "    'selection' option set:\n"
        "    <list widget name>=<value>\n"
        "  - on item activation in a list box with 'activation' option set:\n"
        "    <list widget name>=<value>\n"
        "  - on table cell edit (if enabled):\n"
        "    <table name>[<row>][<col>]=<value>\n"
        "  - on chart slice click:\n"
        "    <chart name>.slice[\"<label>\"]=<value>\n"
        "  - on calendar selection:\n"
        "    <calendar name>=<date>\n"
        "  - on the dialog acceptance or 'query' command list all reportable\n"
        "    enabled named widgets in format:\n"
        "    <name>=<value>\n"
        "\n"
        "Full documentation at: <https://github.com/martynets/showbox/>\n";

    puts(usage);
    puts("");
}

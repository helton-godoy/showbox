TEMPLATE = app
CONFIG += qt thread release c++17
QT += widgets svg charts

TARGET = showbox
DESTDIR = $$PWD/bin
OBJECTS_DIR = $$PWD/obj
MOC_DIR = $$OBJECTS_DIR

# Input
HEADERS += showbox.h \
           tokenizer.h \
           command.h \
           command_registry.h \
           parser.h \
           parser_driver.h \
           execution_context.h \
           icon_helper.h \
           theme_manager.h \
           logger.h \
           custom_table_widget.h \
           custom_chart_widget.h \
           commands/add_command.h \
           commands/set_command.h \
           commands/query_command.h \
           commands/simple_commands.h \
           commands/command_utils.h \
           commands/position_command.h \
           commands/unset_command.h

SOURCES += showbox.cc \
           dialog_parser.cc \
           dialog_main.cc \
           dialog_set_options.cc \
           dialog_slots.cc \
           dialog_private.cc \
           tokenizer.cpp \
           command_registry.cpp \
           parser.cpp \
           parser_driver.cpp \
           execution_context.cpp \
           icon_helper.cpp \
           theme_manager.cpp \
           logger.cpp \
           custom_chart_widget.cpp \
           commands/add_command.cpp \
           commands/set_command.cpp \
           commands/query_command.cpp \
           commands/simple_commands.cpp \
           commands/command_utils.cpp \
           commands/position_command.cpp \
           commands/unset_command.cpp

# install recipe options
target.path = /usr/bin
INSTALLS += target

# Added C/C++ compiler options
# Strict warnings
QMAKE_CXXFLAGS += -Werror -Wall -Wextra

# Man page generation - temporarily disabled due to path issues
# man_page.target = MAN/showbox.1
# man_page.commands = pandoc -s -f markdown -t man MAN/showbox.md -o MAN/showbox.1
# man_page.depends = MAN/showbox.md

# QMAKE_EXTRA_TARGETS += man_page
# PRE_TARGETDEPS += MAN/showbox.1

# man_install.path = /usr/share/man/man1
# man_install.files = MAN/showbox.1
# INSTALLS += man_install

# Windows specific: Ensure console is available for stdout/stdin
win32:CONFIG += console

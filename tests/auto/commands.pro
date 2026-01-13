QT += testlib core widgets
CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app
TARGET = tst_commands

# Path to showbox source code (from tests/auto/)
SHOWBOX_SRC = ../../src/code/showbox

SOURCES += tst_commands.cpp \
           $$SHOWBOX_SRC/commands/add_command.cpp \
           $$SHOWBOX_SRC/commands/set_command.cpp \
           $$SHOWBOX_SRC/commands/command_utils.cpp \
           $$SHOWBOX_SRC/execution_context.cpp \
           $$SHOWBOX_SRC/logger.cpp

HEADERS += $$SHOWBOX_SRC/commands/add_command.h \
           $$SHOWBOX_SRC/commands/set_command.h \
           $$SHOWBOX_SRC/commands/command_utils.h \
           $$SHOWBOX_SRC/execution_context.h \
           $$SHOWBOX_SRC/command.h \
           $$SHOWBOX_SRC/logger.h

INCLUDEPATH += $$SHOWBOX_SRC

CONFIG += c++17
QMAKE_CXXFLAGS += -Wall -Wextra

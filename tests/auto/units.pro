QT += testlib core widgets
CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app
TARGET = tst_units

# Path to showbox source code (from tests/auto/)
SHOWBOX_SRC = ../../src/code/showbox

SOURCES += tst_units.cpp \
           $$SHOWBOX_SRC/execution_context.cpp \
           $$SHOWBOX_SRC/logger.cpp

HEADERS += $$SHOWBOX_SRC/execution_context.h \
           $$SHOWBOX_SRC/logger.h

INCLUDEPATH += $$SHOWBOX_SRC

CONFIG += c++17
QMAKE_CXXFLAGS += -Wall -Wextra

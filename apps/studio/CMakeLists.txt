cmake_minimum_required(VERSION 3.16)

project(showbox-studio LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Localizar pacotes do Qt6
find_package(Qt6 REQUIRED COMPONENTS Widgets Charts Svg)

# Configuração Automática do Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# A biblioteca compartilhada é definida pela raiz do monorepo.
set(SHOWBOX_UI_INCLUDE "${CMAKE_SOURCE_DIR}/libs/ui/include")

# Fontes do Studio
set(STUDIO_SOURCES
    resources/shell.qrc
    src/gui/MainWindow.cpp
    src/gui/MainWindow.h
    src/gui/Canvas.cpp
    src/gui/Canvas.h
    src/gui/ObjectInspector.cpp
    src/gui/ObjectInspector.h
    src/gui/PropertyEditor.cpp
    src/gui/PropertyEditor.h
    src/gui/ActionEditor.cpp
    src/gui/ActionEditor.h
    src/gui/toolbox/AbstractToolbox.cpp
    src/gui/toolbox/AbstractToolbox.h
    src/gui/toolbox/ToolboxClassic.cpp
    src/gui/toolbox/ToolboxClassic.h
    src/gui/toolbox/ToolboxTree.cpp
    src/gui/toolbox/ToolboxTree.h
    src/core/StudioController.cpp
    src/core/StudioController.h
    src/core/ScriptGenerator.cpp
    src/core/ScriptGenerator.h
    src/core/ProjectSerializer.cpp
    src/core/ProjectSerializer.h
    src/core/ProjectWidgetMapper.cpp
    src/core/ProjectWidgetMapper.h
    src/core/StudioWidgetFactory.h
    src/core/IStudioWidgetFactory.h
    src/core/PreviewManager.cpp
    src/core/PreviewManager.h
)

add_library(showbox-studio-components STATIC
    ${STUDIO_SOURCES}
)

# Includes
target_include_directories(showbox-studio-components PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Linkagem
target_link_libraries(showbox-studio-components PUBLIC
    Qt6::Widgets
    Qt6::Charts
    Qt6::Svg
    showbox-ui
    showbox-project
    showbox-catalog
)

add_executable(showbox-studio src/main.cpp)
target_link_libraries(showbox-studio PRIVATE showbox-studio-components)

enable_testing()
add_subdirectory(tests)

# ============================================================================
# Instalação Profissional (GNUInstallDirs e AppImage)
# ============================================================================
include(GNUInstallDirs)

# Instala o binário em usr/bin
install(TARGETS showbox-studio
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    BUNDLE DESTINATION .
)

# Instala o ícone SVG (Scalable) para garantir alta definição
# O comando RENAME garante que o ícone tenha o nome esperado pelo .desktop
install(FILES packaging/icon/showbox-studio.svg
    DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps
    RENAME showbox-studio.svg
)

# Instala o arquivo .desktop
install(FILES packaging/appimage/showbox-studio.desktop
    DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
)

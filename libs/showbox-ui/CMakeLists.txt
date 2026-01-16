cmake_minimum_required(VERSION 3.16)

project(libshowbox-ui)

# Habilitar MOC para processar Q_OBJECT nos widgets da lib
set(CMAKE_AUTOMOC ON)

# Incluir Headers explicitamente para que o AUTOMOC os encontre
add_library(showbox-ui STATIC
    src/icon_helper.cpp
    include/icon_helper.h
    src/logger.cpp
    include/logger.h
    src/custom_chart_widget.cpp
    include/custom_chart_widget.h
    include/custom_table_widget.h
    src/theme_manager.cpp
    include/theme_manager.h
    src/widget_factory.cpp
    include/widget_factory.h
    src/push_button_widget.cpp
    include/push_button_widget.h
    src/ShowboxBuilder.cpp
    include/ShowboxBuilder.h
    include/WidgetConfigs.h
)

target_include_directories(showbox-ui PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(showbox-ui PUBLIC 
    Qt6::Widgets 
    Qt6::Charts
    Qt6::Svg
)
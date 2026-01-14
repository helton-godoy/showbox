cmake_minimum_required(VERSION 3.16)

project(libshowbox-ui)

add_library(showbox-ui STATIC
    src/icon_helper.cpp
    src/logger.cpp
)

target_include_directories(showbox-ui PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(showbox-ui PUBLIC 
    Qt6::Widgets 
    Qt6::Charts
    Qt6::Svg
)

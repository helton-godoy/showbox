cmake_minimum_required(VERSION 3.16)

project(libshowbox-project)

# Modelo de projeto representado em dados, independente da infraestrutura de
# widgets do Studio. Depende somente de Qt Core (+ Teste nos testes).
add_library(showbox-project STATIC
    src/ProjectNode.cpp
    include/ProjectNode.h
    src/ProjectModel.cpp
    include/ProjectModel.h
)

target_include_directories(showbox-project PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(showbox-project PUBLIC
    Qt6::Core
    showbox-catalog
)

target_compile_options(showbox-project PRIVATE -Wall -Wextra -Wpedantic)
if(SHOWBOX_WARNINGS_AS_ERRORS)
    target_compile_options(showbox-project PRIVATE -Werror)
endif()

add_subdirectory(tests)
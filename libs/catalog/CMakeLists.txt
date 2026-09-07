cmake_minimum_required(VERSION 3.16)

project(libshowbox-catalog)

# Catálogo declarativo de tipos, aliases, eventos e propriedades suportados.
# Depende somente de Qt Core; consumido por libs/project, apps/studio e, em
# testes, pelo motor.
add_library(showbox-catalog STATIC
    src/Catalog.cpp
    include/Catalog.h
)

target_include_directories(showbox-catalog PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(showbox-catalog PUBLIC
    Qt6::Core
)

target_compile_options(showbox-catalog PRIVATE -Wall -Wextra -Wpedantic)
if(SHOWBOX_WARNINGS_AS_ERRORS)
    target_compile_options(showbox-catalog PRIVATE -Werror)
endif()

add_subdirectory(tests)

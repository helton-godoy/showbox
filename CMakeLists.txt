cmake_minimum_required(VERSION 3.16)

project(showbox-suite LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(SHOWBOX_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" ON)
set(SHOWBOX_LEGACY_ORACLE "" CACHE FILEPATH
    "Optional showbox-legacy executable used by the golden compatibility test")

# Configuração Global do Qt
find_package(Qt6 6.4 REQUIRED COMPONENTS Widgets Charts Svg)

# Diretórios de Saída
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Subprojetos
add_subdirectory(libs/showbox-ui)
add_subdirectory(src/code/showbox)
enable_testing()
add_subdirectory(tests)

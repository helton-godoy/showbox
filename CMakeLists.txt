cmake_minimum_required(VERSION 3.16)

project(showbox-suite LANGUAGES CXX)

# Versão única da suíte: VERSION (SemVer) é a fonte de verdade; os scripts de
# packaging e o CI derivam dela a sintaxe de cada formato.
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" _showbox_version_raw)
string(STRIP "${_showbox_version_raw}" SHOWBOX_VERSION)
message(STATUS "ShowBox version: ${SHOWBOX_VERSION}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(SHOWBOX_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" ON)
set(SHOWBOX_LEGACY_ORACLE "" CACHE FILEPATH
    "Optional showbox-legacy executable used by the golden compatibility test")

# Configuração Global do Qt
find_package(Qt6 6.4 REQUIRED COMPONENTS Widgets Charts Svg Network)

# Diretórios de Saída
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Subprojetos
add_subdirectory(libs/ui)
add_subdirectory(apps/runtime)
enable_testing()
add_subdirectory(libs/catalog)
add_subdirectory(tests)
add_subdirectory(libs/project)

option(SHOWBOX_BUILD_STUDIO "Compilar o editor visual experimental" ON)
if(SHOWBOX_BUILD_STUDIO)
    add_subdirectory(apps/studio)
endif()

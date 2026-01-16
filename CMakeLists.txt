cmake_minimum_required(VERSION 3.16)

project(showbox-suite LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configuração Global do Qt
find_package(Qt6 REQUIRED COMPONENTS Widgets Charts PrintSupport Network Svg)

# Diretórios de Saída
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Subprojetos
add_subdirectory(libs/showbox-ui)
add_subdirectory(src/code/showbox)
# add_subdirectory(apps/studio) # Habilitaremos quando iniciarmos o Studio

enable_testing()
add_subdirectory(tests)

add_executable(poc_modern_cli
    src/poc_modern_cli.cpp
)

target_link_libraries(poc_modern_cli PRIVATE
    Qt6::Widgets
    showbox-ui
)

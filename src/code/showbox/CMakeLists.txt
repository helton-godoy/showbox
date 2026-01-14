cmake_minimum_required(VERSION 3.16)

project(showbox-cli)

# Configuração Automática do Qt (MOC, UIC, RCC)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Encontrar fontes
file(GLOB_RECURSE SRCS 
    "*.cpp" 
    "*.cc" 
    "*.h"
)

add_executable(showbox 
    ${SRCS}
)

# Dependências
target_link_libraries(showbox PRIVATE 
    Qt6::Widgets 
    Qt6::Charts
    Qt6::PrintSupport
    Qt6::Network
    showbox-ui # Linkando nossa nova lib (mesmo que vazia por enquanto)
)

# Includes
target_include_directories(showbox PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

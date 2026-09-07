set(CMAKE_AUTOMOC ON)

add_library(showbox-core STATIC
    core/builder/CLIBuilder.cpp
    core/builder/CLIBuilder.h
    core/parser/CommandParser.cpp
    core/parser/CommandParser.h
    core/parser/ParserMain.cpp
    core/parser/ParserMain.h
    core/parser/Tokenizer.cpp
    core/parser/Tokenizer.h
    core/parser/WidgetParserUtils.cpp
    core/parser/WidgetParserUtils.h
)

target_include_directories(showbox-core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/core/builder
    ${CMAKE_CURRENT_SOURCE_DIR}/core/parser
)

target_link_libraries(showbox-core PUBLIC Qt6::Widgets showbox-ui)
target_compile_options(showbox-core PRIVATE -Wall -Wextra -Wpedantic)
if(SHOWBOX_WARNINGS_AS_ERRORS)
    target_compile_options(showbox-core PRIVATE -Werror)
endif()

add_executable(showbox main.cpp)
target_link_libraries(showbox PRIVATE showbox-core)
target_compile_options(showbox PRIVATE -Wall -Wextra -Wpedantic)
if(SHOWBOX_WARNINGS_AS_ERRORS)
    target_compile_options(showbox PRIVATE -Werror)
endif()

include(GNUInstallDirs)

# Instalação por componente permite empacotar apenas o motor ou Studio.
install(TARGETS showbox RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT showbox-engine
)

# Entry de desktop do motor (consome o arquivo canônico em packaging/desktop).
install(FILES ${CMAKE_SOURCE_DIR}/packaging/desktop/showbox.desktop
    DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
    COMPONENT showbox-engine
)

find_package(Qt6 REQUIRED COMPONENTS Test)

set(CMAKE_AUTOMOC ON)

add_executable(tst_widgetfactory
    auto/tst_widgetfactory.cpp
)

target_link_libraries(tst_widgetfactory PRIVATE
    Qt6::Widgets
    Qt6::Test
    showbox-ui
)

add_test(NAME tst_widgetfactory COMMAND tst_widgetfactory)

add_executable(tst_pushbutton
    auto/tst_pushbutton.cpp
)

target_link_libraries(tst_pushbutton PRIVATE
    Qt6::Widgets
    Qt6::Test
    showbox-ui
)

add_test(NAME tst_pushbutton COMMAND tst_pushbutton)

add_executable(tst_clibuilder
    auto/tst_clibuilder.cpp
    ../src/code/showbox/core/builder/CLIBuilder.cpp
)

target_link_libraries(tst_clibuilder PRIVATE
    Qt6::Widgets
    Qt6::Test
    showbox-ui
)

target_include_directories(tst_clibuilder PRIVATE
    ../src/code/showbox/core/builder
)

add_test(NAME tst_clibuilder COMMAND tst_clibuilder)

add_executable(tst_parser
    auto/tst_parser.cpp
    ../src/code/showbox/core/parser/ParserMain.cpp
    ../src/code/showbox/core/parser/CommandParser.cpp
    ../src/code/showbox/core/parser/Tokenizer.cpp
    ../src/code/showbox/core/parser/WidgetParserUtils.cpp
)

target_link_libraries(tst_parser PRIVATE
    Qt6::Widgets
    Qt6::Test
    showbox-ui
)

target_include_directories(tst_parser PRIVATE
    ../src/code/showbox/core/parser
)

add_test(NAME tst_parser COMMAND tst_parser)

add_executable(tst_widget_configs
    auto/tst_widget_configs.cpp
)

target_link_libraries(tst_widget_configs PRIVATE
    Qt6::Widgets
    Qt6::Test
    showbox-ui
)

add_test(NAME tst_widget_configs COMMAND tst_widget_configs)

add_executable(tst_interface_builder
    auto/tst_interface_builder.cpp
)

target_link_libraries(tst_interface_builder PRIVATE
    Qt6::Widgets
    Qt6::Test
    showbox-ui
)

add_test(NAME tst_interface_builder COMMAND tst_interface_builder)

add_executable(tst_showbox_builder
    auto/tst_showbox_builder.cpp
)

target_link_libraries(tst_showbox_builder PRIVATE
    Qt6::Widgets
    Qt6::Test
    showbox-ui
)

add_test(NAME tst_showbox_builder COMMAND tst_showbox_builder)

add_executable(tst_tokenizer
    auto/tst_tokenizer.cpp
    ../src/code/showbox/core/parser/Tokenizer.cpp
)

target_link_libraries(tst_tokenizer PRIVATE
    Qt6::Widgets
    Qt6::Test
    showbox-ui
)

target_include_directories(tst_tokenizer PRIVATE
    ../src/code/showbox/core/parser
)

add_test(NAME tst_tokenizer COMMAND tst_tokenizer)

add_executable(tst_command_parser
    auto/tst_command_parser.cpp
    ../src/code/showbox/core/parser/CommandParser.cpp
    ../src/code/showbox/core/parser/Tokenizer.cpp
)

target_link_libraries(tst_command_parser PRIVATE
    Qt6::Widgets
    Qt6::Test
    showbox-ui
)

target_include_directories(tst_command_parser PRIVATE
    ../src/code/showbox/core/parser
)

add_test(NAME tst_command_parser COMMAND tst_command_parser)
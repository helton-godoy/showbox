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
)

target_link_libraries(tst_clibuilder PRIVATE
    Qt6::Widgets
    Qt6::Test
    showbox-core
)

target_include_directories(tst_clibuilder PRIVATE
    ../apps/runtime/core/builder
)

add_test(NAME tst_clibuilder COMMAND tst_clibuilder)

add_executable(tst_parser
    auto/tst_parser.cpp
)

target_link_libraries(tst_parser PRIVATE
    Qt6::Widgets
    Qt6::Test
    showbox-core
)

target_include_directories(tst_parser PRIVATE
    ../apps/runtime/core/parser
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
)

target_link_libraries(tst_tokenizer PRIVATE
    Qt6::Widgets
    Qt6::Test
    showbox-core
)

target_include_directories(tst_tokenizer PRIVATE
    ../apps/runtime/core/parser
)

add_test(NAME tst_tokenizer COMMAND tst_tokenizer)

add_executable(tst_command_parser
    auto/tst_command_parser.cpp
)

target_link_libraries(tst_command_parser PRIVATE
    Qt6::Widgets
    Qt6::Test
    showbox-core
)

target_include_directories(tst_command_parser PRIVATE
    ../apps/runtime/core/parser
)

add_test(NAME tst_command_parser COMMAND tst_command_parser)

add_test(
    NAME cli_contract
    COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/integration/cli_contract.sh
            $<TARGET_FILE:showbox>
)
set_tests_properties(cli_contract PROPERTIES
    TIMEOUT 5
)

add_test(
    NAME version_contract
    COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/integration/version_contract.sh
)
set_tests_properties(version_contract PROPERTIES
    TIMEOUT 30
)

if(SHOWBOX_LEGACY_ORACLE)
    if(NOT EXISTS "${SHOWBOX_LEGACY_ORACLE}")
        message(FATAL_ERROR
            "SHOWBOX_LEGACY_ORACLE does not exist: ${SHOWBOX_LEGACY_ORACLE}")
    endif()

    add_test(
        NAME legacy_golden_contract
        COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/compatibility/golden_contract.sh
                ${SHOWBOX_LEGACY_ORACLE}
                $<TARGET_FILE:showbox>
    )
    set_tests_properties(legacy_golden_contract PROPERTIES
        TIMEOUT 5
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
    )
endif()

# Every Qt test must be runnable on CI and inside package builders, where no
# display server is available.  Keeping this here also covers tests launched
# indirectly by debhelper's dh_auto_test.
set_tests_properties(
    tst_widgetfactory
    tst_pushbutton
    tst_clibuilder
    tst_parser
    tst_widget_configs
    tst_interface_builder
    tst_showbox_builder
    tst_tokenizer
    tst_command_parser
    cli_contract
    PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

find_package(Python3 3.9 REQUIRED COMPONENTS Interpreter)
find_package(Git REQUIRED)
add_test(NAME worktree_isolation
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/test_worktree.py)
set_tests_properties(worktree_isolation PROPERTIES TIMEOUT 30)

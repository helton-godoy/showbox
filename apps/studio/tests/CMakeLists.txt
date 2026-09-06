find_package(Qt6 REQUIRED COMPONENTS Test Widgets Charts Svg)

# --- Teste de Configurações (Existente) ---
add_executable(tst_WidgetConfigs tst_WidgetConfigs.cpp)
target_link_libraries(tst_WidgetConfigs PRIVATE Qt6::Test showbox-ui)
target_include_directories(tst_WidgetConfigs PRIVATE "${SHOWBOX_UI_INCLUDE}")
add_test(NAME tst_WidgetConfigs COMMAND tst_WidgetConfigs)

# --- Teste de Integração do Studio (Existente) ---
set(STUDIO_TEST_SOURCES
    tst_StudioIntegration.cpp
    ../src/gui/Canvas.cpp
    ../src/core/StudioController.cpp
    ../src/core/ScriptGenerator.cpp
    ../src/core/ProjectSerializer.cpp
    ../src/gui/PropertyEditor.cpp 
)

add_executable(tst_StudioIntegration ${STUDIO_TEST_SOURCES})

target_link_libraries(tst_StudioIntegration PRIVATE 
    Qt6::Test 
    Qt6::Widgets
    Qt6::Charts
    Qt6::Svg
    showbox-ui
)

target_include_directories(tst_StudioIntegration PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/../src"
    "${SHOWBOX_UI_INCLUDE}"
)

add_test(NAME tst_StudioIntegration COMMAND tst_StudioIntegration)

# --- Teste do Script Generator (Existente) ---
set(GENERATOR_TEST_SOURCES
    tst_ScriptGenerator.cpp
    ../src/core/ScriptGenerator.cpp
    ../src/core/StudioController.cpp
    ../src/gui/Canvas.cpp
    ../src/gui/PropertyEditor.cpp
)

add_executable(tst_ScriptGenerator ${GENERATOR_TEST_SOURCES})

target_link_libraries(tst_ScriptGenerator PRIVATE 
    Qt6::Test 
    Qt6::Widgets
    Qt6::Charts
    Qt6::Svg
    showbox-ui
)

target_include_directories(tst_ScriptGenerator PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/../src"
    "${SHOWBOX_UI_INCLUDE}"
)

add_test(NAME tst_ScriptGenerator COMMAND tst_ScriptGenerator)

# --- Teste do Project Serializer (Novo) ---
set(SERIALIZER_TEST_SOURCES
    tst_ProjectSerializer.cpp
    ../src/core/ProjectSerializer.cpp
    ../src/core/StudioController.cpp # Dependência indireta ou apenas para linkagem
)

add_executable(tst_ProjectSerializer ${SERIALIZER_TEST_SOURCES})

target_link_libraries(tst_ProjectSerializer PRIVATE 
    Qt6::Test 
    Qt6::Widgets
    Qt6::Charts
    Qt6::Svg
    showbox-ui
)

target_include_directories(tst_ProjectSerializer PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/../src"
    "${SHOWBOX_UI_INCLUDE}"
)

add_test(NAME tst_ProjectSerializer COMMAND tst_ProjectSerializer)

# --- Teste de Undo/Redo (Novo) ---
set(UNDOREDO_TEST_SOURCES
    tst_UndoRedo.cpp
    ../src/core/StudioController.cpp
    ../src/gui/Canvas.cpp
    ../src/core/StudioWidgetFactory.h # Interface/Inline se houver
    ../src/core/ScriptGenerator.cpp
)

add_executable(tst_UndoRedo ${UNDOREDO_TEST_SOURCES})

target_link_libraries(tst_UndoRedo PRIVATE 
    Qt6::Test 
    Qt6::Widgets
    Qt6::Charts
    Qt6::Svg
    showbox-ui
)

target_include_directories(tst_UndoRedo PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/../src"
    "${SHOWBOX_UI_INCLUDE}"
)

# --- Teste de Hierarquia (Novo) ---
set(HIERARCHY_TEST_SOURCES
    tst_Hierarchy.cpp
    ../src/gui/ObjectInspector.cpp
    ../src/core/StudioController.cpp
)

add_executable(tst_Hierarchy ${HIERARCHY_TEST_SOURCES})

target_link_libraries(tst_Hierarchy PRIVATE 
    Qt6::Test 
    Qt6::Widgets
    Qt6::Charts
    Qt6::Svg
    showbox-ui
)

target_include_directories(tst_Hierarchy PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/../src"
    "${SHOWBOX_UI_INCLUDE}"
)

add_test(NAME tst_Hierarchy COMMAND tst_Hierarchy)

# --- Teste de Performance de Estilo (Novo) ---
set(STYLE_PERF_SOURCES
    tst_StylePerformance.cpp
    ../src/core/StudioController.cpp
    ../src/gui/Canvas.cpp
)

add_executable(tst_StylePerformance ${STYLE_PERF_SOURCES})

target_link_libraries(tst_StylePerformance PRIVATE 
    Qt6::Test 
    Qt6::Widgets
    Qt6::Charts
    Qt6::Svg
    showbox-ui
)

target_include_directories(tst_StylePerformance PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/../src"
    "${SHOWBOX_UI_INCLUDE}"
)

add_test(NAME tst_StylePerformance COMMAND tst_StylePerformance)

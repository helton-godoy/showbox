# Implementation Plan: Studio Persistence & Code Generation

## Phase 1: Script Generator Core ✅

Implementar a lógica de conversão da árvore de objetos Qt para sintaxe Showbox CLI.

- [x] Task: Create `ScriptGenerator` Class
  - [x] Define interface for generator.
  - [x] Implement recursive traversal of `StudioController` managed widgets.
  - [x] Map Qt properties back to Showbox CLI keys (e.g., `text` -> `text`, `checked` -> `checked`).
  - [x] Use `showbox_type` dynamic property for reliable type mapping.
- [x] Task: Unit Test Generation
  - [x] Test generating script for a simple Window + Label + Button.
  - [x] Verify output format matches expected Here-Doc syntax.

## Phase 2: Live Preview (Run) ✅

Permitir que o usuário veja o resultado real do seu design rodando no engine do Showbox.

- [x] Task: Implement Preview Action in UI
  - [x] Add "Run" / "Preview" button in Toolbar.
  - [x] Implement `PreviewManager` logic in `MainWindow` to save temp file.
  - [x] Execute `poc_modern_cli` via `QProcess` pointing to build directory.
- [x] Task: Process Output Capture
  - [x] Capture Exit Code and stderr for error reporting.

## Phase 3: Project Persistence (JSON) ✅

Salvar e carregar o estado do editor.

- [x] Task: Implement JSON Serializer
  - [x] Serialize `StudioWidgetFactory` created widgets to JSON (recursive).
  - [x] Deserialize JSON back to Widget Tree, restoring properties.
  - [x] Unit Test Save/Load cycle with `QTemporaryFile`.
- [x] Task: UI Integration
  - [x] Add Open/Save actions to Toolbar.
  - [x] Implement Canvas clearing and re-population.

## Phase 4: Polish & Integration ✅

- [x] Task: Clean up and Validation
  - [x] Full regression test suite run (4/4 passed).

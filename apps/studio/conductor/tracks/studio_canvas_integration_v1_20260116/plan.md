# Implementation Plan: Studio Canvas & Palette Integration

Este plano detalha a transformação do Showbox Studio em um designer funcional.

## Phase 1: Canvas Hosting & Builder Integration ✅

Fazer o Canvas ser capaz de renderizar e gerenciar widgets do Showbox.

- [x] Task: Integrate `ShowboxBuilder` into `Canvas`
  - [x] Update `Canvas` to hold a reference to `IShowboxBuilder` (via `StudioWidgetFactory`)
  - [x] Implement a `renderPreview(const WindowConfig&)` method (implemented as `addWidget` in Canvas/Controller)
- [x] Task: Interactive Selection Logic
  - [x] Implement visual highlight for selected widgets in the Canvas (Blue Border via `StudioController`)
  - [x] Emit signals when a widget is clicked (Connected via `StudioController`)

## Phase 2: Widget Palette (Drag & Drop) ✅

Criação da barra de ferramentas com todos os widgets disponíveis e suporte a arrastar e soltar.

- [x] Task: Enable Drag in Toolbox
  - [x] Set `m_toolbox` to support dragging items.
  - [x] Ensure `QMimeData` carries the widget type name.
- [x] Task: Drop Handling in Canvas
  - [x] Override `dragEnterEvent` in `Canvas` to accept widget drops.
  - [x] Override `dropEvent` in `Canvas` to calculate drop position and add the widget.
- [x] Task: Factory Mapping
  - [x] Ensure `StudioWidgetFactory` can handle all dropped types correctly (Added Table support).

## Phase 3: Object Inspector & Hierarchy ✅

- [x] Task: Dynamic Tree Population
  - [x] Sync `ObjectInspector` tree view with the current Canvas hierarchy.
- [x] Task: Bidirectional Selection
  - [x] Handle selection changes bidirectionally (Inspector <-> Canvas) via `StudioController`.

## Phase 4: Reactive Property Editor ✅

- [x] Task: Property Mapping
  - [x] Implement logic to display fields in `PropertyEditor` based on selected widget config.
  - [x] Support for Boolean types (checkboxes) and basic string properties.
- [x] Task: Live Update
  - [x] Implement "Live Update": changing a value in the editor updates the Canvas widget immediately.
- [x] Task: Advanced Types
  - [x] Support for Enums (ComboBox) via `QMetaProperty` reflection.

## Phase 5: Studio Validation ✅

- [x] Task: Full Design Flow Demo
  - [x] Verified via Automated Integration Test (`tst_StudioIntegration`).
  - [x] Validated Widget Creation, Property Reflection, and Canvas Selection Logic.

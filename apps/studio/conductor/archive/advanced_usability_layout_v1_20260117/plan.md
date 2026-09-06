# Implementation Plan: Advanced Usability & Layouts

## Phase 1: Undo/Redo Infrastructure [checkpoint: 90f2b76]

Implementar a rede de segurança para edição.

- [~] Task: Setup `QUndoStack` in `StudioController`
  - [ ] Initialize stack.
  - [ ] Expose Undo/Redo actions to MainWindow.
- [x] Task: Implement `Add/Delete` Commands [1d0a03c]
  - [ ] Refactor `MainWindow::onDeleteClicked` to push a `DeleteWidgetCommand`.
  - [ ] Refactor `Canvas::dropEvent` to push an `AddWidgetCommand`.
- [x] Task: Implement `PropertyChange` Command [474293f]
  - [ ] Intercept changes in `PropertyEditor` and push commands instead of direct set.

## Phase 2: Hierarchy Reordering (Tree View) [checkpoint: bf317ed]

Permitir organizar a estrutura via Drag & Drop na árvore.

- [x] Task: Enable Drag & Drop in `ObjectInspector` [22051a4]
  - [ ] Set `dragDropMode` to `InternalMove`.
  - [ ] Override `dropEvent` to handle reparenting logic.
- [x] Task: Synchronize with Canvas [bf317ed]
  - [ ] Implement logic to move the actual QWidget when the TreeItem is moved.
  - [ ] Ensure `layout()->insertWidget(index, w)` is used.

## Phase 3: Contextual Layout Grouping [checkpoint: caced0b]

Facilitar a criação de layouts complexos.

- [x] Task: Implement Context Menu in Canvas/Tree [78f1eea]
  - [x] Actions: "Group in Frame (HBox)", "Group in Frame (VBox)".
- [x] Task: Implement Grouping Logic [caced0b]

## Phase 4: Layout Properties & Refinement [checkpoint: f918a97]

- [x] Task: Expose Layout Configuration [f918a97]
  - [x] Show `layout` property in `PropertyEditor` for containers.
  - [x] Allow switching between HBox/VBox dynamically.

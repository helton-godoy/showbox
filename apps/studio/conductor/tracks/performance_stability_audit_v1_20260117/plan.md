# Implementation Plan: Performance Stability Audit

**Goal:** Eliminar a latência na interação de Drag & Drop e garantir robustez contra regressões.

## Phase 1: Diagnosis & Profiling [CRITICAL]

Identificar e medir os gargalos exatos.

- [ ] Task: Instrument `ObjectInspector::updateHierarchy`
  - [ ] Add `QElapsedTimer` logs to measure rebuild time.
- [ ] Task: Instrument `ScriptGenerator::generate`
  - [ ] Measure time taken to generate QML script on drop.
- [ ] Task: Audit Event Flow
  - [ ] Trace signals from `Canvas::dropEvent` to `MainWindow`.
  - [ ] Identify redundant calls (e.g., multiple updates per action).

## Phase 2: Object Inspector Optimization (The likely bottleneck)

Refatorar a árvore de objetos para não reconstruir tudo a cada alteração.

- [ ] Task: Refactor `updateHierarchy` to be incremental
  - [ ] Handle `widgetAdded` (insert single item).
  - [ ] Handle `widgetRemoved` (remove single item).
  - [ ] **Architectural Goal:** Move towards `QAbstractItemModel` if feasible.

## Phase 3: Canvas Interaction Refinement

Melhorar a experiência de arrastar e soltar.

- [ ] Task: Optimize `dragMoveEvent`
  - [ ] Ensure minimal logic during mouse move.
- [ ] Task: Asynchronous Preview
  - [ ] Debounce `PreviewManager` updates (don't update on every pixel move or immediate drop if heavy).

## Phase 4: Regression Guards

- [ ] Task: Create "High Load" Test Case
  - [ ] Script that adds 100 widgets and measures lag.

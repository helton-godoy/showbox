# Implementation Plan: Visual Refinement & Asset Management

## Phase 1: Specialized Property Delegates [checkpoint: 0a0af86]

Implementar editores visuais básicos para o `PropertyEditor`.

- [x] Task: Implement Color Picker Delegate [0a0af86]
  - [x] Detect color properties.
  - [x] Show color preview in cell.
  - [x] Connect `QColorDialog`.
- [x] Task: Implement Font Delegate [0a0af86]
  - [x] Detect font properties.
  - [x] Connect `QFontDialog`.

## Phase 2: Icon Browser & Selection

Integrar o sistema de ícones do Showbox.

- [ ] Task: Create `IconBrowser` Dialog
  - [ ] Grid view listing available SVGs.
  - [ ] Search/Filter by name.
- [ ] Task: Connect `IconBrowser` to PropertyEditor
  - [ ] Add icon preview to cells.
  - [ ] Trigger browser on click.

## Phase 3: Theme Integration & Canvas Styling

- [ ] Task: Implement Theme Selector in Studio
  - [ ] Global dropdown for themes.
- [ ] Task: Apply Live Themes to Canvas
  - [ ] Map selection to `ThemeManager` styles.
  - [ ] Update Canvas stylesheet dynamically.

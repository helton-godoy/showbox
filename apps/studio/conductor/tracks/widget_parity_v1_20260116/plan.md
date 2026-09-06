# Implementation Plan: Remaining Widgets (V1 Parity)

Este plano detalha a expansão do Showbox Suite para atingir paridade total com os widgets da versão legada, utilizando o padrão Builder e uma abordagem declarativa.

## Phase 1: Expanded Data Contracts [checkpoint: 19941d1]

Expansão das estruturas de dados em `WidgetConfigs.h` para suportar todos os novos widgets.

- [x] Task: Define Selection and Input Configuration Structures [19941d1]
  - [x] Write unit tests for CheckBox, RadioButton, SpinBox, and Slider configs
  - [x] Implement structs: `CheckBoxConfig`, `RadioButtonConfig`, `SpinBoxConfig`, `SliderConfig`
  - [x] Verify default values and validation logic
- [x] Task: Define Utility and Text Configuration Structures [19941d1]
  - [x] Write unit tests for Calendar, TextEdit, and Separator configs
  - [x] Implement structs: `CalendarConfig`, `TextEditConfig`, `SeparatorConfig`
- [x] Task: Define Container Configuration Structures (Hierarchical) [19941d1]
  - [x] Write unit tests for GroupBox, Frame, and TabWidget configs (including nesting)
  - [x] Implement structs: `GroupBoxConfig`, `FrameConfig`, `PageConfig`, `TabWidgetConfig`
- [x] Task: Conductor - User Manual Verification 'Phase 1: Expanded Contracts' (Protocol in workflow.md) [19941d1]

## Phase 2: Selection and Utility Widget Implementation

Implementação dos controles de seleção e utilitários visuais simples.

- [x] Task: Implement `ShowboxBuilder` for Selection Widgets [9207547]
  - [x] Write failing tests for CheckBox and RadioButton instantiation
  - [x] Implement `buildCheckBox` and `buildRadioButton`
  - [x] Verify signal/slot compatibility for passive view
- [x] Task: Implement `ShowboxBuilder` for Utility Widgets [9207547]
  - [x] Write failing tests for Separator and Calendar
  - [x] Implement `buildSeparator` and `buildCalendar`
- [x] Task: Conductor - User Manual Verification 'Phase 2: Selection and Utilities' (Protocol in workflow.md) [checkpoint: 2a3587a]

## Phase 3: Numerical and Text Input Implementation

Implementação de controles de entrada de dados mais complexos.

- [x] Task: Implement `ShowboxBuilder` for Numerical Inputs [0fa06f2]
  - [x] Write failing tests for SpinBox and Slider (Min/Max/Step validation)
  - [x] Implement `buildSpinBox` and `buildSlider`
- [x] Task: Implement `ShowboxBuilder` for Multiline Text [0fa06f2]
  - [x] Write failing tests for TextEdit (Rich Text support)
  - [x] Implement `buildTextEdit`
- [x] Task: Conductor - User Manual Verification 'Phase 3: Inputs' (Protocol in workflow.md) [checkpoint: a524fcb]

## Phase 4: Container and Hierarchical Implementation

Implementação dos contêineres que permitem organizar outros widgets.

- [x] Task: Implement `ShowboxBuilder` for Basic Containers [01216da]
  - [x] Write failing tests for GroupBox and Frame (Layout nesting)
  - [x] Implement `buildGroupBox` and `buildFrame`
- [x] Task: Implement `ShowboxBuilder` for Tabbed Interfaces (Block Construction) [01216da]
  - [x] Write failing tests for TabWidget with multiple Pages
  - [x] Implement `buildTabWidget` using the "Block Construction" approach
  - [x] Verify child widget reparenting and layout management
- [x] Task: Conductor - User Manual Verification 'Phase 4: Containers' (Protocol in workflow.md) [checkpoint: b4ff016]

## Phase 5: Final Parity Validation

Validação final de que todos os recursos do V1 estão acessíveis via nova arquitetura.

- [x] Task: Comprehensive Integration Demo [225dfa8]
  - [x] Create a complex UI PoC using every widget implemented in this track
  - [x] Verify visual alignment and behavior in the Dev Container
- [x] Task: Conductor - User Manual Verification 'Phase 5: Parity Validation' (Protocol in workflow.md) [checkpoint: 225dfa8]

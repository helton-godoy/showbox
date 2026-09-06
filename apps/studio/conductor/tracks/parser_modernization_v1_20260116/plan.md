# Implementation Plan: V1 Parser Modernization

Este plano detalha a implementação do novo sistema de parsing para conectar a CLI legado à nova arquitetura do Builder.

## Phase 1: Core Infrastructure

Estabelecimento das fundações para processamento de texto e comandos.

- [x] Task: Implement `Tokenizer` Class [60f5559]
  - [x] Write tests for splitting strings with quotes, spaces, and escapes
  - [x] Implement `Tokenizer` class (replaces naive split)
- [x] Task: Implement `CommandParser` Skeleton [dc666cd]
  - [x] Define `ICommand` interface or Command structure
  - [x] Implement dispatch logic (Map<String, Handler>)
  - [x] Connect `ParserMain` to use `CommandParser` [checkpoint: e7f1b89]

## Phase 2: Widget Creation (The 'Add' Command) - Part 1

Suporte para widgets básicos e propriedades comuns.

- [x] Task: Implement Generic Property Parser [ad4146a]
  - [x] Logic to parse common flags: `text`, `title`, `name`, `width`, `height`, `checked`, `enabled`, `visible`
- [x] Task: Implement Parsers for Basic Widgets [ad4146a]
  - [x] `add label`, `add button` (pushbutton)
  - [x] `add window` (main window config)
- [x] Task: Implement Parsers for Selection Widgets [ad4146a]
  - [x] `add checkbox`, `add radiobutton`
  - [x] `add combobox`, `add listbox` (parsing items lists)

## Phase 3: Widget Creation (The 'Add' Command) - Part 2

Suporte para widgets complexos e contêineres.

- [x] Task: Implement Parsers for Numerical/Text Inputs [e9e21a0]
  - [x] `add spinbox`, `add slider` (min/max/step)
  - [x] `add lineedit` (textbox), `add textedit` (textview)
- [x] Task: Implement Parsers for Containers [dc666cd]
  - [x] `add groupbox`, `add frame`
  - [x] `add tabwidget`, `add page` (handling context/hierarchy)

## Phase 4: Runtime Interaction

Suporte para modificar o estado da UI após a criação.

- [x] Task: Implement `Set` and `Unset` Commands [88a7e57]
  - [x] Map property names to `IShowboxBuilder` or direct widget manipulation (TBD: Bridge pattern might be needed here or access via `StudioController`)
- [x] Task: Implement `Query` Command [88a7e57]
  - [x] Logic to retrieve values from widgets by name

## Phase 5: Integration & Validation

- [x] Task: End-to-End Integration [88a7e57]
  - [x] Replace `poc_modern_cli` input loop with full `ParserMain`
  - [x] Verify against legacy test scripts (e.g., `examples/showbox_demo.sh`)
- [x] Task: Conductor - User Manual Verification 'Phase 5: Parser Validation' [checkpoint: 524b2c8]

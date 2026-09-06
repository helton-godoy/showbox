# Specification: V1 Parser Modernization

## Context

The `ShowboxBuilder` (libs/showbox-ui) now supports creating all legacy widgets via structured `Config` objects. However, the core application (`ParserMain.cpp`) processes text commands naively and does not yet map the legacy syntax (e.g., `add checkbox "Title" name checked`) to these new structures.

## Goals

1.  **Robust Tokenization:** Handle quoted strings, escaped characters, and whitespace correctly, mirroring the behavior of `dialog_parser.cc` but with cleaner C++17 code.
2.  **Command Dispatching:** Efficiently route commands (`add`, `set`, `query`) to appropriate handlers.
3.  **Config Mapping:** specific logic to parse property tokens (e.g., `min 10`, `readonly`) and populate the corresponding `Showbox::Models::*Config` structs.
4.  **Parity:** Support all keywords defined in the legacy `dialog_parser.cc`.

## Architecture

### 1. Tokenizer

A standalone class responsible for splitting a raw command line into a list of typed tokens (String, Number, Keyword).

### 2. CommandParser

The main driver that identifies the command verb (`add`, `set`, etc.) and delegates to a specific handler.

### 3. WidgetParsers

A set of helper functions or classes specialized in parsing specific widget arguments.

- Example: `CheckBoxParser` reads "Title", "name", and optional flags like "checked".

### 4. Integration

The `ParserMain` will own the `Tokenizer` and `CommandParser`, holding a reference to `IShowboxBuilder` to execute the final build commands.

## Constraints

- Must compile with C++17.
- Must maintain backward compatibility with V1 syntax.
- Must use the `IShowboxBuilder` interface.

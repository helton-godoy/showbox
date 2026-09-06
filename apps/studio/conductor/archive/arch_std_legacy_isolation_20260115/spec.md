# Specification: Architecture Standardization & Legacy Isolation

## 1. Overview

The goal of this track is to eliminate architectural ambiguity within the Showbox project. Currently, the coexistence of "Parser V1" (Legacy Monolith) and an incomplete "Parser V2" confuses AI agents and developers.

We will establish a **"Golden Sample"** architecture that unifies the CLI and Studio needs. This involves isolating legacy code into dedicated directories (preserving functionality for comparison) and implementing a definitive "Target Architecture" using the `ShowboxBuilder` pattern for a single widget (`PushButton`) as the absolute reference for future development.

## 2. Objectives

1.  **Eliminate Confusion:** Physically separate legacy code (V1 and V2) from the active source tree to prevent AI agents from using them as patterns.
2.  **Establish the Pattern:** Implement the complete "Target Architecture" flow (Parser -> Builder -> Lib -> Widget) for one component (`PushButton`).
3.  **Preserve & Validate:** Ensure legacy code remains executable for regression testing and comparison.
4.  **Studio Readiness:** The new architecture must be directly consumable by Showbox Studio.

## 3. Scope

### 3.1. Codebase Reorganization (Sanitation)

- Move existing Parser V1 files (`dialog_parser.cc`, `dialog_slots.cc`, etc.) to `src/legacy/v1_monolith/`.
- Move incomplete Parser V2 files to `src/legacy/v2_incomplete/`.
- Update build scripts (CMake/QMake) to allow building the legacy targets optionally (e.g., via a flag or distinct target name) for comparison purposes.

### 3.2. Target Architecture Implementation ("The Golden Sample")

- **Core Library (`libs/showbox-ui`):**
  - Ensure `PushButton` is fully refactored as a "Passive View" (no shell logic inside the widget).
- **Builder Pattern (`src/core/builder/`):**
  - Create `ShowboxBuilder` class to abstract widget instantiation and property setting.
  - Implement `CLIBuilder` (concrete implementation for CLI usage).
- **New Parser Loop (`src/core/parser/`):**
  - Implement a minimal, clean parser loop that consumes `stdin` and orchestrates the `ShowboxBuilder`.
  - Implement the `add` command specifically for `pushbutton` using this new structure.

### 3.3. Validation

- **Comparative Script:** Create a test script that launches both the Legacy V1 `pushbutton` and the New Architecture `pushbutton` to visually verify parity.

## 4. Functional Requirements

- **FR1:** The new parser must handle the command `add pushbutton "Label" name` correctly using the new architecture.
- **FR2:** The Legacy V1 parser must still compile and run when explicitly requested (e.g., `./showbox-legacy`).
- **FR3:** The directory structure must clearly distinguish between `src/core` (The Truth) and `src/legacy` (The Reference).

## 5. Non-Functional Requirements

- **Development Environment:** All compilation and execution must occur within the Dockerized environment (`./src/start-dev.sh`).
- **Code Quality:** The new implementation must strictly adhere to C++17 and Qt6 best practices.
- **AI Clarity:** The structure must be self-evident so future AI sessions immediately recognize the correct pattern to follow.

## 6. Out of Scope

- Refactoring other widgets (Label, Inputs, etc.) - this will happen in subsequent tracks using the "Golden Sample" as a guide.
- Complete implementation of all Dialogbox commands in the new parser (only `add` and basic flow for now).

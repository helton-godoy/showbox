# General Code Style Principles

This document outlines general coding principles that apply across all languages and frameworks used in this project.

## Readability

- Code should be easy to read and understand by humans.
- Avoid overly clever or obscure constructs.

## Consistency

- Follow existing patterns in the codebase.
- Maintain consistent formatting, naming, and structure.

## Simplicity

- Prefer simple solutions over complex ones.
- Break down complex problems into smaller, manageable parts.

## Maintainability

- Write code that is easy to modify and extend.
- Minimize dependencies and coupling.

## Documentation

- Document _why_ something is done, not just _what_.
- Keep documentation up-to-date with code changes.

# C++ & Qt Specific Conventions

## Naming Conventions

- **Classes:** PascalCase (e.g., `MainWindow`, `StudioController`).
- **Methods/Functions:** camelCase (e.g., `setupUI`, `calculateTotal`). This aligns with Qt's API style.
- **Member Variables:** camelCase, often prefixed with `m_` (e.g., `m_layout`, `m_button`) to distinguish from local variables, a common practice in Qt development.
- **Signals/Slots:** camelCase. Signals should focus on the _event_ (e.g., `dataChanged`), Slots on the _action_ (e.g., `updateView`).

## File Organization

- **Headers (.h):** Define classes, signals, and slots. Use `#pragma once` or include guards.
- **Sources (.cpp):** Implement logic. Keep headers clean of implementation details unless templated.
- **Project Structure:**
  - `src/gui/`: UI-related classes (Widgets, Windows).
  - `src/core/`: Business logic and controllers.
  - `libs/`: Reusable libraries (Showbox Core).

## Qt Best Practices

- **Memory Management:** Use Qt's parent-child ownership system whenever possible. Prefer `std::unique_ptr` or `QScopedPointer` for non-QObject members.
- **Signals & Slots:** Use the modern compile-time checked syntax: `connect(sender, &Sender::signal, receiver, &Receiver::slot);`.
- **String Handling:** Prefer `QString` over `std::string` for UI and file handling due to better Unicode support.

## Modern C++ (C++17)

- Use `auto` for complex types where the type is obvious.
- Prefer `nullptr` over `NULL` or `0`.
- Use `enum class` for scoped enumerations.

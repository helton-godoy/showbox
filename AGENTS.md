# AGENTS.md

## Project Overview

ShowBox is a C++/Qt6 application that provides GUI widgets for shell scripts. It uses Qt6 (QtWidgets, QtCharts, QtSVG) with qmake build system.

## Development Environment

### Building

```bash
# Build entire project
make build

# Or manually
cd src/code/showbox && qmake6 && make -j$(nproc)

# Clean build artifacts
make clean
```

### Development Environment (Docker)

```bash
# Start Docker development environment
make dev

# Start dev and compile inside container
make start-dev-build
```

### Testing

```bash
# Run all tests
make test

# Run unit tests manually
cd tests/auto
qmake6 units.pro
make
./tst_units

# Run single test case (Qt Test)
cd tests/auto
./tst_units TestShowBoxUnits::testLoggerCategoriesExist
```

### Linting

```bash
# Run all linters (Trunk.io)
trunk check

# Auto-fix where possible
trunk check --fix

# Run specific linters
trunk check --filter shellcheck
trunk check --filter prettier
trunk check --filter markdownlint
```

## Code Style Guidelines

### File Structure

- Headers: `showbox.h`, `tokenizer.h`, `commands/*.h`
- Sources: `showbox.cc`, `tokenizer.cpp`, `commands/*.cpp`
- Use `.cc` for main implementation files, `.cpp` for others (established pattern)

### Headers

- Standard license header at top of every file (GPLv3+)
- Include guards: `#ifndef FILENAME_H_` / `#define FILENAME_H_`
- Qt includes first, then local includes
- Sort includes alphabetically within groups

```cpp
/*
 * GUI widgets for shell scripts - showbox version 1.0
 *
 * Copyright (C) 2015-2016, 2020 Andriy Martynets <andy.martynets@gmail.com>
 *------------------------------------------------------------------------------
 * This file is part of showbox.
 * [GPLv3+ license text]
 *------------------------------------------------------------------------------
 */

#ifndef SHOWBOX_H_
#define SHOWBOX_H_

#include <QtGlobal>
#include <QtWidgets>
#include "logger.h"

#endif  // SHOWBOX_H_
```

### Naming Conventions

- **Classes**: PascalCase (`ShowBox`, `CommandUtils`, `DialogParser`)
- **Methods**: camelCase (`executeCommand`, `setOptions`, `findWidget`)
- **Variables**: camelCase for class members, underscore_case for locals (mixed - follow existing pattern)
- **Constants**: UPPER_SNAKE_CASE (`BUFFER_SIZE`, `DEFAULT_ALIGNMENT`)
- **Files**: lowercase_with_underscores (`showbox.h`, `command_utils.cpp`)

### Qt Conventions

- Use `Q_OBJECT` macro for all QObject-derived classes
- Use signals/slots pattern for event handling
- Use Qt containers (`QList`, `QMap`, `QString`) over STL when possible
- Namespace: `DialogCommandTokens` for enums/constants

### Code Formatting

- Indentation: 4 spaces (tabs not used)
- Braces: K&R style (opening brace on same line)
- Line length: No strict limit, keep reasonable (~100 chars)
- Spacing: Spaces around operators, after commas
- Strict compiler warnings enabled: `-Werror -Wall -Wextra`

### Error Handling

- Use Qt logging categories: `qCDebug(parserLog)`, `qCDebug(guiLog)`
- Declare categories in `logger.h`: `Q_DECLARE_LOGGING_CATEGORY(categoryName)`
- Prefer Qt error handling over exceptions (Qt convention)
- Check return values, validate pointers before use

### C++ Features

- Use C++17 standard
- Use `override` keyword for overridden methods
- Use `final` keyword for classes/methods that shouldn't be derived
- Prefer `= default` and `= delete` for constructors
- Smart pointers: Use Qt's parent-child ownership model, `std::unique_ptr` where appropriate

### Testing

- Use Qt Test framework
- Test files: `tests/auto/tst_*.cpp`
- Test classes: `class TestClassName : public QObject` with `Q_OBJECT`
- Test methods: `void testFunctionality()` (no parameters)
- Test lifecycle: `initTestCase()`, `cleanupTestCase()`
- Use macros: `QVERIFY(condition)`, `QCOMPARE(actual, expected)`

## Repository Structure

- `src/code/showbox/` - Main source code (C++/Qt)
- `src/code/showbox/commands/` - Command implementations (factory pattern)
- `tests/auto/` - Unit tests
- `examples/` - Shell script examples demonstrating ShowBox usage
- `docs/` - Project documentation
- `packaging/` - Distribution packages (DEB, RPM, Flatpak, etc.)

## PR Guidelines

- Follow conventional commits for PR titles (e.g., `feat(commands): add option support`)
- Ensure all tests pass: `make build && cd tests/auto && qmake6 && make && ./tst_units`
- Run linters: `trunk check`
- Build with Docker container to ensure reproducibility
- Test with examples in `examples/` directory
- Update documentation when adding new widgets or features

## AI Context References

- Documentation index: `.context/docs/README.md`
- Agent playbooks: `.context/agents/README.md`
- Commands knowledge base: `src/code/showbox/commands/AGENTS.md`
- Contributor guide: `docs/README.md`

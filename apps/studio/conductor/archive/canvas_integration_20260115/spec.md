# Specification: Canvas Integration and Core Refactoring

## Overview

This track focuses on the critical integration between the Showbox-Studio's Canvas and the modularized Showbox Core library. It aims to validate the CMake-based modular architecture by ensuring that the Studio can dynamically render widgets defined in the Core.

## Functional Requirements

- **Core Linkage:** Showbox-Studio must successfully link against the `showbox-ui` library using CMake.
- **Widget Factory Access:** The Studio must be able to instantiate widgets from the Core via a standardized factory or interface.
- **Canvas Rendering:** The `Canvas` widget in the Studio must act as a container for Core widgets, maintaining correct layouts and properties.
- **Basic Property Reflection:** Initial support for reading properties from a Core widget and displaying them in the Studio's Property Editor.

## Technical Requirements

- C++17 and Qt6.
- CMake FetchContent or Subdirectory integration for the `showbox` repository.
- Adherence to the `cpp-qt.md` style guide.

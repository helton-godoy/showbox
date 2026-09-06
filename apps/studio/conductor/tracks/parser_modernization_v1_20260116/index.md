# V1 Parser Modernization

**ID:** `parser_modernization_v1_20260116`
**Status:** In Progress
**Plan:** [plan.md](./plan.md)
**Spec:** [spec.md](./spec.md)

## Overview

This track modernizes the command-line parsing logic to bridge the gap between legacy Showbox V1 scripts and the new C++ Builder architecture. It involves replacing the naive string splitting with a robust tokenizer and implementing a command dispatcher that populates `WidgetConfig` structures.

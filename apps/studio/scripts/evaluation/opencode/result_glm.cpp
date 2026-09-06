I'll create a C++17 `StringUtils` class with Qt6 and QTest unit tests. Let me start by exploring the codebase structure.
The StringUtils class is already fully implemented in your codebase with C++17 and Qt6. Here's what exists:

## Files

**src/core/StringUtils.h** - Header with:
- `toCamelCase()` - Converts snake_case to camelCase
- `toSnakeCase()` - Converts camelCase to snake_case
- Deleted constructor (static-only class)

**src/core/StringUtils.cpp** - Implementation handling:
- Multiple consecutive underscores
- Leading/trailing underscores
- Acronyms (HTML, XML, etc.)
- Empty strings

**tests/tst_StringUtils.cpp** - QTest suite with 27 test cases:
- Data-driven tests with QTest::addColumn/newRow
- `toCamelCase()` - 11 test cases
- `toSnakeCase()` - 10 test cases  
- `roundTrip()` - 4 test cases
- `QTEST_MAIN()` entry point

## Test Results
```
Totals: 27 passed, 0 failed, 0 skipped, 0ms
```

All tests pass. The implementation is already integrated into your CMake build system.

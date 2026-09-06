[auto-update-checker] Package removed: /home/helton/.cache/opencode/node_modules/opencode-antigravity-auth
[auto-update-checker] Dependency removed from package.json: opencode-antigravity-auth
[auto-update-checker] Removed from bun.lock: opencode-antigravity-auth
Done. Created `StringUtils` class with Qt6:

**Files created:**
- `src/core/StringUtils.h` - Header with `toCamelCase()` and `toSnakeCase()` static methods
- `src/core/StringUtils.cpp` - Implementation handling edge cases (consecutive underscores, acronyms, all-caps)
- `tests/tst_StringUtils.cpp` - QTest unit suite with 27 test cases

**Features:**
- `toCamelCase()`: snake_case → camelCase (`hello_world` → `helloWorld`)
- `toSnakeCase()`: camelCase → snake_case (`myHTMLParser` → `my_html_parser`)
- Handles consecutive underscores, leading/trailing underscores, empty strings, acronyms

**Test coverage:**
- Basic conversions, edge cases, round-trip consistency

Run with: `./build/tests/tst_StringUtils`

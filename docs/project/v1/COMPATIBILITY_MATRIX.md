# Dialogbox compatibility matrix

The stable CLI accepts one command per line. Quoted strings preserve spaces and
backslash escapes the next character. Unknown commands are ignored for legacy
compatibility; malformed known commands must not crash or partially mutate UI.

## Commands

| Command              | Required behavior                                                   | Automated |
| -------------------- | ------------------------------------------------------------------- | --------- |
| `add`                | Create the documented widget or container                           | Partial   |
| `end`                | Close the current list/container/page context                       | Yes       |
| `set` / `unset`      | Use legacy `name property value` ordering                           | Yes       |
| `enable` / `disable` | Toggle named widget                                                 | Yes       |
| `show` / `hide`      | Empty name targets dialog; name targets widget                      | Yes       |
| `query`              | Print every enabled named reportable widget as `name=value`         | Yes       |
| `remove` / `clear`   | Remove widget or container contents safely                          | Yes       |
| `step`               | Create horizontal columns and vertical rows                         | Yes       |
| `position`           | Move insertion before/behind widgets/items/pages or onto containers | Yes       |

`position` is automated for ordinary widgets, `frame`/`groupbox`, tab pages,
and indexed or text-addressed list/table items.

Documented slider ranges, calendar date formats, separator orientation/shadow,
and frame orientation/shape/shadow have parser-level regression coverage.
Chart `data`, `append`, clearing, and slice-event output are also covered.
Table CSV loading, search visibility, readonly state, headers, cell/row events,
positioned row insertion, and safe content clearing are covered.
Animated labels and progress-bar busy mode transitions are covered.
Textbox, listbox, combobox, and dropdownlist now preserve their separate title
labels and route focus, values, mutation, events, queries, and clearing through
their internal input widgets.
Tab pages support runtime title/icon/current changes, and tab containers support
all four documented tab positions.

## Stable widgets

`label`, `pushbutton`, `checkbox`, `radiobutton`, `textbox`, `textview`,
`listbox`, `dropdownlist`, `combobox`, `item`, `progressbar`, `slider`,
`separator`, `space`, `stretch`, `frame`, `groupbox`, `tabs`, `page`, `table`,
`chart`, and `calendar`.

`spinbox`, `scrollarea`, named layout aliases, and experimental spacers are not
part of the 1.0 compatibility promise.

## Golden oracle

Build the historical executable outside the source tree and enable its CTest
comparison explicitly:

````sh
tests/compatibility/build_legacy_oracle.sh "$PWD" "$PWD/build-legacy-oracle"
cmake -S . -B build -DSHOWBOX_LEGACY_ORACLE="$PWD/build-legacy-oracle/bin/showbox-legacy"
cmake --build build --parallel
ctest --test-dir build -R legacy_golden_contract --output-on-failure
```text

The stable CMake targets never compile or link legacy sources.

## Output contract

- Button: `name=clicked`, or `pressed`/`released` for checkable buttons.
- Slider and selection: `name=value`.
- Table edit: `name[row][column]=value`.
- Calendar: ISO date in `name=YYYY-MM-DD` form.
- `query` and apply actions flush stdout before any dialog exit.
````

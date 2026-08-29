# Showbox 1.0 property audit

This audit maps every widget and property in `docs/widgets-reference.md` and
`docs/dialogbox-reference.md` to executable coverage. `Parser` means
`tst_parser::testDocumentedWidgetOptions`; `Builder` means the corresponding
case in `tst_showbox_builder`; `Golden` means `legacy_golden_contract`.

| Widget | Creation options and runtime properties | Evidence |
| --- | --- | --- |
| `pushbutton` | `apply`, `exit`, `default`, `checkable`, `checked`, `icon`, `iconsize`, `title` | Parser, Builder |
| `checkbox`, `radiobutton` | `checked`, `icon`, `iconsize` | Parser, Builder |
| `textbox` | `password`, `text`, `placeholder`, `title`, `readonly`, clear/reset behavior | Parser, Builder, Golden |
| `listbox` | `activation`, `selection`, item `icon`, `title`, `iconsize`, `current` | Parser, Builder |
| `combobox`, `dropdownlist` | `selection`, `editable`, item `icon`, `title`, `iconsize`, `current` | Parser, Builder |
| `slider` | positional and named ranges, `minimum`, `maximum`, `value`, orientation | Parser, Golden |
| `calendar` | `date`, `minimum`, `maximum`, `format`, `selection`, `navigation` | Parser, Builder |
| `label` | text/title, `picture`, `animation`, `icon` | Parser, Builder |
| `progressbar` | `minimum`, `maximum`, `value`, `busy`, `format`, orientation | Parser, Builder, Golden |
| `textview` | name-only syntax, `file`, `readonly`, clear/reset | Parser, Builder |
| `separator` | optional name, orientation, `plain`, `raised`, `sunken` | Parser, Builder |
| `table` | headers, CSV/JSON `file`, `readonly`, `selection`, `search`, `add_line`, `del_line` | Parser, Builder, Golden |
| `chart` | `data`, `append`, `file`, horizontal/vertical `axis`, `export` | Parser, Builder, Golden |
| `frame` | orientation, `noframe`, `box`, `panel`, `styled`, `plain`, `raised`, `sunken` | Parser, Builder |
| `groupbox` | orientation, `checkable`, `checked`, `title` | Parser, Builder |
| `tabs`, `page` | tab position, page `title`, `icon`, `current`, enabled state | Parser, Builder |
| `space`, `stretch` | size and stretch factor | Parser |

Global `enabled`, `visible`, `focus`, `stylesheet`, `icon`, and `iconsize`
mutations use the same runtime dispatcher. Their widget-specific Qt targets
(including titled-control focus proxies) are covered by the parser suite.

The golden test additionally proves layout-order query output, table cells,
chart slices, and exclusion of non-reportable progress bars against the legacy
executable. Table selection retains the legacy `name_selection=row` form.

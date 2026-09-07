# Showbox 1.0 status

Baseline: `origin/develop` commit `2848cb3`  
Integration branch: `integration/showbox-v1`

## Completed in the current integration pass

- Replaced recursive CMake source discovery with explicit modern targets.
- Centralized widget construction in `showbox-ui` and removed duplicated,
  blocking widget creation from `CLIBuilder`.
- Moved stdin handling to the Qt event loop; no detached parser thread remains.
- Restored legacy ordering for `set`, `unset`, `query`, `show`, and aliases.
- Added stdout event wiring for buttons, sliders, selection widgets, tables,
  and calendars without executing shell commands inside Showbox.
- Converted CI, DEB, RPM, and AppImage build paths to CMake.
- Removed tracked binaries, object files, MOC output, and qmake state.
- Added nine Qt unit suites plus a stdin/stdout CLI contract test.
- Produced, inspected, and tested the CMake-built Debian and Ubuntu 1.0.0 packages.
- Restored three-level `step` layout behavior and widget/container insertion
  cursors for `position`, with structural regression tests.
- Restored documented slider ranges, calendar output formats, separator styles,
  and frame orientation/frame styles.
- Added runtime chart data replacement/appending/clearing and documented slice
  events in `name.slice["label"]=value` form.
- Unified tables on `CustomTableWidget`, including CSV/JSON loading, search,
  readonly mode, header changes, row selection/edit events, and safe clearing.
- Reworked `clear` to clear widget contents or layout-owned user widgets without
  deleting Qt implementation children such as viewports and search fields.
- Added animated-label playback through `QMovie` and runtime progress-bar busy
  mode with legacy-compatible reset behavior.
- Restored the legacy titled-control composition for textbox, listbox,
  combobox, and dropdownlist using focus proxies while preserving public names.
- Added runtime page title/icon/current handling and tab-position mutations.
- Added an optional golden contract test that compares the modern executable
  byte-for-byte with a separately built, out-of-tree legacy oracle.
- Repaired and validated Fedora RPM and Ubuntu AppImage production, including
  a real SVG application icon and bundled Qt Wayland plugins.
- Added and passed X11/Xvfb and Wayland/Weston headless smoke tests for both the
  regular executable and the generated AppImage.
- Completed the documented property audit, restored layout-ordered complex
  query reports, and matched legacy table-selection output.

## Validation

- Environment: Debian 13 container, Qt 6.8.2, GCC 14.2.
- CMake configure: passing.
- Modern Debug build: passing.
- CTest: 11/11 suites passing when the optional legacy oracle is configured;
  this includes the executable CLI contract and golden legacy comparison.
- Debian package: `showbox_1.0.0-1_amd64.deb` built successfully; package
  contents and automatically resolved Qt 6 dependencies inspected.
- Fedora package: `showbox-1.0.0-1.fc44.x86_64.rpm` built successfully.
- AppImage: `ShowBox-1.0.0-x86_64.AppImage` built and exercised under X11 and
  Wayland headless compositors.

## Remaining release gates

None for the Showbox 1.0 implementation plan. Signing, publishing, and opening
a release pull request remain explicit maintainer actions.

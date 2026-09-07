# Showbox 1.0 roadmap

This roadmap is the source of truth for completing the Qt 6 CLI. Work starts
from `integration/showbox-v1`; feature branches use `agent/v1-wpNN-topic`.

| Package | Dependency               | Deliverable                                            | State |
| ------- | ------------------------ | ------------------------------------------------------ | ----- |
| WP-00   | `origin/develop@2848cb3` | Clean repository and CMake baseline                    | Done  |
| WP-01   | WP-00                    | Executable compatibility matrix                        | Done  |
| WP-02   | WP-01                    | Explicit `showbox-ui`, `showbox-core`, and CLI targets | Done  |
| WP-03   | WP-02                    | Legacy-compatible tokenizer and command parser         | Done  |
| WP-04   | WP-02                    | Basic and selection widgets                            | Done  |
| WP-05   | WP-02                    | Table, chart, and calendar                             | Done  |
| WP-06   | WP-03/04                 | Containers, layouts, and runtime interactions          | Done  |
| WP-07   | WP-06                    | Wayland/X11 and compatibility verification             | Done  |
| WP-08   | WP-07                    | DEB, RPM, AppImage, documentation, and release         | Done  |

## Merge rules

- An agent changes only the subsystem and tests named by its package.
- Shared interfaces are changed in a separate integration commit before
  parallel branches are created.
- Only the integrator updates `STATUS.md` and the table above.
- Every handoff reports base SHA, final SHA, tests, known gaps, and the next
  recommended command.

## Definition of done

The modern CMake binary must pass all unit and compatibility tests, preserve
the documented Dialogbox command/stdout/exit-code contract, run under Qt's
Wayland and XCB backends, and be the binary installed by all supported packages.
No stable target may compile sources under `legacy/`.

# COMMANDS KNOWLEDGE BASE

## OVERVIEW

Command implementations following factory pattern for parser integration.

## WHERE TO LOOK

| Command         | Location                | Notes                             |
| --------------- | ----------------------- | --------------------------------- |
| Add             | `add_command.cpp/h`     | Add widgets to dialog             |
| Set             | `set_command.cpp/h`     | Set widget properties             |
| Query           | `query_command.cpp/h`   | Query widget values               |
| Simple Commands | `simple_commands.cpp/h` | enable, disable, show, hide, etc. |
| Utils           | `command_utils.cpp/h`   | Shared command utilities          |

## CONVENTIONS

- Pattern: `*_command.h/cpp`
- Each command has factory class registered in CommandRegistry
- Use `command_utils.h` for shared functionality

#!/bin/bash

# Path to dialogbox binary
DIALOGBOX=./dist_qt6/dialogbox

if [[ ! -f ${DIALOGBOX} ]]; then
	echo "Error: Binary not found at ${DIALOGBOX}"
	exit 1
fi

echo "Testing basic add/set commands with --new-parser..."

# We pipe commands.
# We use 'add pushbutton ... exit' to ensure it closes (if 'exit' is supported in logic).
# Wait, AddCommand implementation for pushbutton supports 'exit'.
# But does it trigger close?
# DialogBox::addPushButton sets properties.
# Clicking the button triggers close.
# But we are running headless-ish (or detached).
# If we just pipe commands, the app will stay open waiting for interaction unless we kill it or use a command that exits immediately?
# There is no "quit" command in dialogbox grammar.
# We can run it with timeout or just check if it parses without crash.

# But `ParserDriver` reads until EOF. When EOF is reached, `run()` finishes.
# But `main` calls `app.exec()`.
# If `ParserDriver` finishes, does it quit app?
# Original `DialogParser` doesn't quit app on EOF.
# So `dialogbox` will hang waiting for GUI interaction.

# To test automatically, I can use `--hidden` and kill it after some time?
# Or just rely on "didn't crash immediately".

echo "add pushbutton 'OK' btn1 default" | timeout 2s "${DIALOGBOX}" --new-parser --hidden

if [[ $? -eq 124 ]]; then
	echo "Success: Application ran and timed out as expected (GUI loop active)."
	exit 0
else
	echo "Failure: Application exited with code $?"
	exit 1
fi

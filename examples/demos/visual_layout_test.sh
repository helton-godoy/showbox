#!/bin/bash
SHOWBOX_BIN="${SHOWBOX_BIN:-./src/code/showbox/bin/showbox}"

if [[ ! -f "${SHOWBOX_BIN}" ]]; then
	echo "Error: Binary not found at ${SHOWBOX_BIN}"
	exit 1
fi

echo "Launching Visual Layout Test..."
echo 'add frame "Outer Frame" Outer box
add label "Layout Refinement Test"
add separator flat
step horizontal
add groupbox "Group A" vertical checkable checked
add checkbox "Option 1"
add checkbox "Option 2"
end groupbox
add groupbox "Group B" vertical
add radiobutton "Choice X"
add radiobutton "Choice Y"
end groupbox
step vertical
add frame "Inner Frame" Inner
add label "Deeply nested label"
add pushbutton "Apply" apply
add pushbutton "Exit" exit default
end frame' | ${SHOWBOX_BIN} --new-parser --resizable

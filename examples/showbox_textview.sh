#!/bin/bash
#
# ShowBox Example: TextView Widget
# Demonstrates the textview widget for multi-line text
#

SHOWBOX_BIN="${SHOWBOX_BIN:-./src/code/showbox/bin/showbox}"

$SHOWBOX_BIN <<EODEMO
add label "<b>Text Editor</b>" title
set title stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"

add separator

add textview "Content" editor1 "Enter your text here...
This is a multi-line text area.
You can type multiple lines of text.

Features:
- Multi-line editing
- Scrollable content
- Rich text support"

add separator

add groupbox "Read-only Example" grp1
add textview "Log Output" log1 readonly "[2026-01-12 21:40:00] Application started
[2026-01-12 21:40:01] Loading configuration...
[2026-01-12 21:40:02] Ready"
end groupbox

add separator

add frame horizontal
add stretch
add pushbutton "&Save" save apply exit
add pushbutton "&Cancel" cancel exit
end frame

set save default
set title "ShowBox TextView"
show
EODEMO

echo "Exit code: $?"

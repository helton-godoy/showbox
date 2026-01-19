#!/bin/bash
#
# ShowBox Example: Themes Demo
# Demonstrates the theme manager (dark/light/system)
#

SHOWBOX_BIN="${SHOWBOX_BIN:-./src/code/showbox/bin/showbox}"

${SHOWBOX_BIN} <<EODEMO
add label "<h2>ShowBox Theme Demo</h2>" title
set title stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"

add separator

add label "The theme is automatically detected from your system." info
set info stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"

add frame horizontal
add checkbox "Dark Mode" darkmode
add checkbox "System Theme" systemtheme checked
end frame

add separator

add frame horizontal
add stretch
add pushbutton "&Close" close apply exit
end frame

set close default
set title "ShowBox Theme Test"
show
EODEMO

echo "Exit code: $?"

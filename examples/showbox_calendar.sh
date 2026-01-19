#!/bin/bash
#
# ShowBox Example: Calendar Widget
# Demonstrates the calendar widget with date selection
#

SHOWBOX_BIN="${SHOWBOX_BIN:-./src/code/showbox/bin/showbox}"

${SHOWBOX_BIN} <<EODEMO
add label "<b>Select a date:</b>" title
set title stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"

add calendar "Calendar" cal1 selection

add frame horizontal
add stretch
add pushbutton "&OK" ok apply exit
add pushbutton "&Cancel" cancel exit
end frame

set ok default
set title "ShowBox Calendar"
show
EODEMO

echo "Exit code: $?"

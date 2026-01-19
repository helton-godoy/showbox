#!/bin/bash
#
# ShowBox Example: ListBox Widget
# Demonstrates the listbox widget with items, selection and activation
#

SHOWBOX_BIN="${SHOWBOX_BIN:-./src/code/showbox/bin/showbox}"

${SHOWBOX_BIN} <<EODEMO
add label "<b>Select items from the list:</b>" title
set title stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"

add listbox "Options" listbox1 selection
add item "Option 1" opt1
add item "Option 2" opt2
add item "Option 3" opt3
add item "Option 4" opt4
add item "Option 5" opt5
end listbox

add separator

add frame horizontal
add stretch
add pushbutton "&OK" ok apply exit
add pushbutton "&Cancel" cancel exit
end frame

set ok default
set title "ShowBox ListBox"
show
EODEMO

echo "Exit code: $?"

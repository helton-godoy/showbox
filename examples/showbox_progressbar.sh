#!/bin/bash
#
# ShowBox Example: ProgressBar Widget
# Demonstrates the progressbar widget with normal and busy modes
#

SHOWBOX_BIN="${SHOWBOX_BIN:-./src/code/showbox/bin/showbox}"

# Demo with static progress bar
${SHOWBOX_BIN} <<EODEMO
add label "<b>Download Progress</b>" title
set title stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"

add separator

add label "Downloading file..." status
set status stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"

add progressbar "Progress" progress1
set progress1 value 65

add separator

add label "<small>Processing in background...</small>" busy_label
set busy_label stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"

add progressbar "Busy" progress2 busy

add separator

add frame horizontal
add stretch
add pushbutton "&Close" close exit
end frame

set close default
set title "ShowBox ProgressBar"
show
EODEMO

echo "Exit code: $?"

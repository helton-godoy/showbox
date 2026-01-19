#!/bin/bash
#
# ShowBox Example: Separator Widget
# Demonstrates horizontal and vertical separators
#

SHOWBOX_BIN="${SHOWBOX_BIN:-./src/code/showbox/bin/showbox}"

${SHOWBOX_BIN} <<EODEMO
add label "<b>Separator Demo</b>" title
set title stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"

add separator

add label "Content above separator" label1
set label1 stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"

add separator

add label "Content below separator" label2
set label2 stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"

add separator

add frame horizontal
add label "Left" left
set left stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"
add separator vertical
add label "Center" center
set center stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"
add separator vertical
add label "Right" right
set right stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"
end frame

add separator

add frame horizontal
add stretch
add pushbutton "&Close" close exit
end frame

set close default
set title "ShowBox Separator"
show
EODEMO

echo "Exit code: $?"

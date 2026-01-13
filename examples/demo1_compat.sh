#!/bin/bash
#
# Demo1 adaptado para ShowBox
# Teste de compatibilidade - exit codes
#

SHOWBOX_BIN="${SHOWBOX_BIN:-./src/code/showbox/bin/showbox}"

$SHOWBOX_BIN <<EODEMO
add label "<small>This script demonstrates the showbox application exit codes usage." note
set note stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"
add separator
add label "<big>Please confirm the operation" msg
set msg stylesheet "qproperty-wordWrap: false;
          qproperty-textInteractionFlags: NoTextInteraction;"

add frame horizontal
add stretch
add pushbutton C&ontinue apply exit
add pushbutton C&ancel exit
end frame
set title "ShowBox Demo 1"
show
EODEMO

if [ "$?" == "0" ]
then
  echo "User rejected dialog (exit code 0)"
else
  echo "User accepted dialog (exit code 1)"
fi

exit 0

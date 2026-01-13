#!/bin/bash
#
# ShowBox Example: Slider Widget
# Demonstrates the slider widget with min/max values
#

SHOWBOX_BIN="${SHOWBOX_BIN:-./src/code/showbox/bin/showbox}"

while IFS=$'=' read key value
do
    case $key in
        slider1)
            echo "Slider value changed: $value"
            ;;
        slider2)
            echo "Volume: $value%"
            ;;
    esac
done < <(

$SHOWBOX_BIN <<EODEMO
add label "<b>Slider Demo</b>" title
set title stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"

add separator

add groupbox "Basic Slider" grp1
add label "Adjust value (0-100):" lbl1
set lbl1 stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"
add slider "Value" slider1
set slider1 minimum 0
set slider1 maximum 100
set slider1 value 50
end groupbox

add separator

add groupbox "Volume Control" grp2
add label "Volume:" lbl2
set lbl2 stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"
add slider "Volume" slider2
set slider2 minimum 0
set slider2 maximum 100
set slider2 value 75
end groupbox

add separator

add frame horizontal
add stretch
add pushbutton "&OK" ok apply exit
add pushbutton "&Cancel" cancel exit
end frame

set ok default
set title "ShowBox Slider"
show
EODEMO

)

echo "Demo completed"

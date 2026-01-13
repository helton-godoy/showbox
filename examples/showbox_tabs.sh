#!/bin/bash
#
# ShowBox Example: Tabs Widget
# Demonstrates the tabs widget with multiple pages
#

SHOWBOX_BIN="${SHOWBOX_BIN:-./src/code/showbox/bin/showbox}"

$SHOWBOX_BIN <<EODEMO
add label "<b>Settings</b>" title
set title stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"

add separator

add tabs "Settings Tabs" tabs1
add page "General" general_page
add label "General settings go here" gen_label
set gen_label stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"
add checkbox "Enable notifications" notify checked
add checkbox "Start on boot" autostart
end page

add page "Appearance" appearance_page
add label "Appearance settings" app_label
set app_label stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"
add checkbox "Dark mode" darkmode
add checkbox "Use system theme" systemtheme checked
end page

add page "Advanced" advanced_page
add label "Advanced settings" adv_label
set adv_label stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"
add checkbox "Developer mode" devmode
add checkbox "Enable logging" logging
end page
end tabs

add separator

add frame horizontal
add stretch
add pushbutton "&Apply" apply_btn apply
add pushbutton "&OK" ok apply exit
add pushbutton "&Cancel" cancel exit
end frame

set ok default
set title "ShowBox Tabs"
show
EODEMO

echo "Exit code: $?"

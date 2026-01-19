#!/bin/bash
#
# ShowBox Example: Page Widget
# Demonstrates pages within tabs with icons and customization
#

SHOWBOX_BIN="${SHOWBOX_BIN:-./src/code/showbox/bin/showbox}"

${SHOWBOX_BIN} <<EODEMO
add label "<b>Wizard Example</b>" title
set title stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"

add separator

add tabs "Wizard Steps" wizard
add page "Step 1: Welcome" step1 current
add label "<h3>Welcome!</h3>" welcome
set welcome stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"
add label "This wizard will guide you through the setup process." desc1
set desc1 stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"
end page

add page "Step 2: Configuration" step2
add label "<h3>Configuration</h3>" config_title
set config_title stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"
add textbox "Your name" name_input
set name_input placeholder "Enter your name"
add textbox "Email" email_input
set email_input placeholder "user@example.com"
end page

add page "Step 3: Complete" step3
add label "<h3>Setup Complete!</h3>" complete
set complete stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"
add label "Your configuration has been saved." desc3
set desc3 stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"
end page
end tabs

add separator

add frame horizontal
add stretch
add pushbutton "&Previous" prev
add pushbutton "&Next" next
add pushbutton "&Finish" finish apply exit
end frame

set next default
set title "ShowBox Page"
show
EODEMO

echo "Exit code: $?"

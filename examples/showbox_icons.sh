#!/bin/bash
#
# ShowBox Example: Icons Demo
# Demonstrates buttons and labels with icons
#

SHOWBOX_BIN="${SHOWBOX_BIN:-./src/code/showbox/bin/showbox}"

${SHOWBOX_BIN} <<EODEMO
add label "<b>Icons Demo</b>" title
set title stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"

add separator

add groupbox "Toolbar Buttons" grp1 horizontal
add pushbutton "New" btn_new icon
set btn_new icon "document-new"
add pushbutton "Open" btn_open icon
set btn_open icon "document-open"
add pushbutton "Save" btn_save icon
set btn_save icon "document-save"
add separator vertical
add pushbutton "Cut" btn_cut icon
set btn_cut icon "edit-cut"
add pushbutton "Copy" btn_copy icon
set btn_copy icon "edit-copy"
add pushbutton "Paste" btn_paste icon
set btn_paste icon "edit-paste"
end groupbox

add separator

add groupbox "Icon Sizes" grp2
add frame horizontal
add pushbutton "Small" btn_small icon
set btn_small icon "face-smile"
set btn_small iconsize 16

add pushbutton "Medium" btn_medium icon
set btn_medium icon "face-smile"
set btn_medium iconsize 24

add pushbutton "Large" btn_large icon
set btn_large icon "face-smile"
set btn_large iconsize 32
end frame
end groupbox

add separator

add frame horizontal
add stretch
add pushbutton "&Close" close exit
end frame

set close default
set title "ShowBox Icons"
show
EODEMO

echo "Exit code: $?"

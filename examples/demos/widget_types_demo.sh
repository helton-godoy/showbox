#!/bin/bash
#
# ShowBox Demo: Widget Types Demo
# Reproduz a interface completa de demonstração de widgets
#

SHOWBOX_BIN="${SHOWBOX_BIN:-./src/code/showbox/bin/showbox}"

${SHOWBOX_BIN} -r <<EODEMO
# === Tabs principais (laterais à esquerda) ===
add tabs "Main Tabs" maintabs left

# --- Page 1 ---
add page "Page 1" page1 current

# GroupBox de checkboxes
add groupbox "Group of checkboxes" grp_check horizontal
add checkbox "Checkbox 1" cb1
add checkbox "Checkbox 2" cb2
end groupbox

# GroupBox de radiobuttons (checkable)
add groupbox "Group of radiobuttons" grp_radio checkable checked
add radiobutton "Radiobutton 1" rb1
add radiobutton "Radiobutton 2" rb2 checked
add radiobutton "Radiobutton 3" rb3
end groupbox

# Progressbar com porcentagem
add frame horizontal
add progressbar "Progress" prog1
set prog1 value 25
add label "25%" prog_label
end frame

# Slider horizontal
add slider "Slider" slider1
set slider1 value 30

# Imagem/Label com ícone grande
add label "" img_label picture
set img_label icon "system-search"
set img_label iconsize 128

end page

# --- Page 2 ---
add page "Page 2" page2

add label "Content for Page 2" p2_content
set p2_content stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"

# Widget de calendário
add calendar "Select Date" calendar1 selection
set calendar1 date "2026-01-12"

end page

# Fim das tabs laterais
end tabs

# === Lado direito: Tabs internas (Page 3, Page 4) ===
add tabs "Inner Tabs" innertabs top

# --- Page 3 ---
add page "Page 3" page3 current

# Textbox
add frame horizontal
add label "Text box:" lbl_txt
set lbl_txt stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"
add textbox "" txtbox1
set txtbox1 placeholder "enter your text here"
end frame

# Drop-down list (não editável)
add frame horizontal
add label "Drop-down list:" lbl_ddl
set lbl_ddl stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"
add dropdownlist "Options" ddl1
add item "Option 1" opt1
add item "Option 2" opt2
add item "Option 3" opt3
end dropdownlist
end frame

# Combobox (editável)
add frame horizontal
add label "Combobox:" lbl_combo
set lbl_combo stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"
add combobox "Auto" combo1
set combo1 text "autocomletion"
add item "autocomplete" auto1
add item "automatic" auto2
add item "automation" auto3
end combobox
end frame

# Listbox com ícones
add label "Listbox:" lbl_listbox
set lbl_listbox stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"
add listbox "Items" listbox1 selection
add item "Item 1" item1 icon
set item1 icon "folder"
add item "Item 2" item2 icon
set item2 icon "folder"
add item "Item 3" item3 icon current
set item3 icon "folder"
add item "Item 4" item4 icon
set item4 icon "folder"
end listbox

end page

# --- Page 4 ---
add page "Page 4" page4

# Label com texto formatado (rich text)
add label "This is text label widget. Text can<br>be either plain or reach one.<br><b>For example this text is big</b><br><small>when this one is small.</small>" rich_label
set rich_label stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"

end page

end tabs

# === TextView na parte inferior ===
add textview "License" license_view readonly "<center><b>GNU GENERAL PUBLIC LICENSE</b><br>Version 3, 29 June 2007</center>

Copyright (C) 2007 Free Software Foundation, Inc. <http://fsf.org/>
Everyone is permitted to copy and distribute verbatim copies
of this license document, but changing it is not allowed.

<center><b>Preamble</b></center>

The GNU General Public License is a free, copyleft license for
software and other kinds of works."

add separator

# === Botões OK e Cancel ===
add frame horizontal
add stretch
add pushbutton "&Ok" ok default apply exit icon
set ok icon "dialog-ok"
add pushbutton "&Cancel" cancel exit icon
set cancel icon "dialog-cancel"
end frame

set title "Widget types demo"
show
EODEMO

echo "Exit code: $?"

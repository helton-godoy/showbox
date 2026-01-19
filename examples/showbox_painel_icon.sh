#!/bin/bash
#
# ShowBox Example: Icon Panel / Dashboard
# Demonstrates a grid-like layout with icon buttons
#

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
SHOWBOX_BIN="${SHOWBOX_BIN:-./src/code/showbox/bin/showbox}"

# Use Adwaita icons (available on most Linux systems)
# Or replace with your own SVG/PNG files

${SHOWBOX_BIN} <<'EODEMO'
set title "ShowBox - Painel de Widgets"

add label "<h2>Selecione um Tipo de Campo</h2>" header
set header stylesheet "color: #2196F3; margin: 10px;"

add separator

add groupbox "" panel

# ========== Linha 1: Texto ==========
add frame horizontal row1
add stretch

add pushbutton "Single Line" btn_single
set btn_single icon "edit-paste"
set btn_single iconsize 48
set btn_single stylesheet "padding: 10px; min-width: 110px; max-width: 110px; min-height: 70px;"

add pushbutton "Paragraph" btn_paragraph
set btn_paragraph icon "document-new"
set btn_paragraph iconsize 48
set btn_paragraph stylesheet "padding: 10px; min-width: 110px; max-width: 110px; min-height: 70px;"

add pushbutton "Radio" btn_radio
set btn_radio icon "dialog-question"
set btn_radio iconsize 48
set btn_radio stylesheet "padding: 10px; min-width: 110px; max-width: 110px; min-height: 70px;"

add pushbutton "Name" btn_name
set btn_name icon "user-available"
set btn_name iconsize 48
set btn_name stylesheet "padding: 10px; min-width: 110px; max-width: 110px; min-height: 70px;"

add stretch
end frame

# ========== Linha 2: Contato ==========
add frame horizontal row2
add stretch

add pushbutton "Phone" btn_phone
set btn_phone icon "phone"
set btn_phone iconsize 48
set btn_phone stylesheet "padding: 10px; min-width: 110px; max-width: 110px; min-height: 70px;"

add pushbutton "Rating" btn_rating
set btn_rating icon "starred"
set btn_rating iconsize 48
set btn_rating stylesheet "padding: 10px; min-width: 110px; max-width: 110px; min-height: 70px;"

add pushbutton "Signature" btn_signature
set btn_signature icon "document-edit"
set btn_signature iconsize 48
set btn_signature stylesheet "padding: 10px; min-width: 110px; max-width: 110px; min-height: 70px;"

add pushbutton "Section" btn_section
set btn_section icon "view-list-symbolic"
set btn_section iconsize 48
set btn_section stylesheet "padding: 10px; min-width: 110px; max-width: 110px; min-height: 70px;"

add stretch
end frame

# ========== Linha 3: Outros ==========
add frame horizontal row3
add stretch

add pushbutton "Email" btn_email
set btn_email icon "mail-unread"
set btn_email iconsize 48
set btn_email stylesheet "padding: 10px; min-width: 110px; max-width: 110px; min-height: 70px;"

add pushbutton "Date/Time" btn_datetime
set btn_datetime icon "x-office-calendar"
set btn_datetime iconsize 48
set btn_datetime stylesheet "padding: 10px; min-width: 110px; max-width: 110px; min-height: 70px;"

add pushbutton "Calendar" btn_calendar
set btn_calendar icon "x-office-calendar"
set btn_calendar iconsize 48
set btn_calendar stylesheet "padding: 10px; min-width: 110px; max-width: 110px; min-height: 70px;"

add pushbutton "Payment" btn_payment
set btn_payment icon "emblem-money"
set btn_payment iconsize 48
set btn_payment stylesheet "padding: 10px; min-width: 110px; max-width: 110px; min-height: 70px;"

add stretch
end frame

end groupbox

add separator

add frame horizontal actions
add stretch
add pushbutton "&Cancel" btn_cancel exit
add pushbutton "&Select" btn_select apply exit default
set btn_select stylesheet "background-color: #2196F3; color: white; padding: 10px 30px;"
end frame

show
EODEMO

echo ""
echo "Widget selecionado ou diálogo fechado"
echo "Exit code: $?"

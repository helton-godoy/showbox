#!/bin/bash
#
# ShowBox Example: Sidebar + Tabs Layout
# Demonstrates a control panel layout with:
# - Left sidebar menu
# - Right panel with tabs
#

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
SHOWBOX_BIN="${SHOWBOX_BIN:-./src/code/showbox/bin/showbox}"

${SHOWBOX_BIN} --resizable <<'EODEMO'
set title "ShowBox - Painel de Controle"

# === SIDEBAR ESQUERDA ===
# Widgets adicionados na coluna 1 (padrão)
add frame sidebar vertical panel
add label "<b>Menu</b>" lbl_menu
set lbl_menu stylesheet "padding: 10px; font-size: 14px;"

add pushbutton "Dashboard" menu_dashboard
set menu_dashboard stylesheet "text-align: left; padding: 8px 15px;"

add pushbutton "Configurações" menu_config
set menu_config stylesheet "text-align: left; padding: 8px 15px;"

add pushbutton "Logs" menu_logs
set menu_logs stylesheet "text-align: left; padding: 8px 15px;"

add separator horizontal

add pushbutton "Sair" btn_exit exit
set btn_exit stylesheet "text-align: left; padding: 8px 15px; color: red;"

add stretch
end frame

# === PAINEL DIREITO COM ABAS ===
# step horizontal cria uma nova coluna à DIREITA
step horizontal

add tabs tabs_main

add page "Dashboard" pg_dashboard current
add label "<h3>Dashboard</h3>" lbl_dash_title
add progressbar pg_progress busy
add label "Sistema funcionando normalmente." lbl_status

add page "Configurações" pg_config
add label "<h3>Configurações</h3>" lbl_cfg_title
add checkbox "Habilitar recurso A" cfg_feature_a checked
add checkbox "Habilitar recurso B" cfg_feature_b
add checkbox "Modo avançado" cfg_advanced

add page "Logs" pg_logs
add label "<h3>Visualizador de Logs</h3>" lbl_logs_title
add label "Nenhum log recente." lbl_logs_content

end tabs

show
EODEMO

echo ""
echo "Diálogo fechado"
echo "Exit code: $?"

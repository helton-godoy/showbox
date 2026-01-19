#!/bin/bash
# Teste de Recursos V2 (Remove, Clear, Aliases, Icons)

# 1. Definição da Janela
add window "Teste Recursos V2" win width=500 height=400
add vboxlayout main_layout

# 2. Teste de Ícones
add label "Label com Icone (Placeholder)" lbl_icon icon="/usr/share/icons/hicolor/32x32/apps/firefox.png"
add button "Botao com Icone" btn_icon icon="/usr/share/icons/hicolor/32x32/apps/firefox.png"

# 3. Teste de Remove
add label "Este label sera removido em 3s" lbl_remove
add button "Clique para remover label" btn_rm action="remove_lbl"

# 4. Teste de Hide/Show
add button "Botao que some e aparece" btn_toggle
add button "Esconder Botao" btn_hider action="hide_btn"
add button "Mostrar Botao" btn_shower action="show_btn"

# 5. Container e Clear
add groupbox "Container Temporario" gb_temp
add label "Filho 1" l1
add label "Filho 2" l2
end # groupbox
add button "Limpar Container" btn_clear action="clear_gb"

show win

# Simulação de script interativo
sleep 3
remove lbl_remove

sleep 2
hide btn_toggle

sleep 2
show btn_toggle

sleep 2
clear gb_temp

# Fim

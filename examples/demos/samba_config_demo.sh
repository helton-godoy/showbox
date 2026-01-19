#!/bin/bash
#
# Demo do ShowBox: Painel de Configuração do Samba
# Interface com menu lateral (sidebar) à esquerda e três abas à direita
# Simula um painel de configuração avançada do Samba com opções realistas
#

SHOWBOX_BIN="${SHOWBOX_BIN:-./src/code/showbox/bin/showbox}"

${SHOWBOX_BIN} <<EODEMO
# Layout principal: frame horizontal
add frame horizontal main_frame

# Sidebar à esquerda: frame vertical com menu
add frame vertical sidebar
add label "<b>Menu de Configuração</b>" menu_title
set menu_title stylesheet "qproperty-textInteractionFlags: NoTextInteraction; font-size: 14px; margin: 10px;"
add separator

# Itens do menu (usando pushbuttons para simular menu)
add pushbutton "&Compartilhamentos" btn_shares
add pushbutton "&Permissões" btn_perms
add pushbutton "&Autenticação" btn_auth
add pushbutton "&Segurança" btn_security
add pushbutton "&Avançado" btn_advanced

add stretch
end frame

# Área à direita: três abas
add tabs "Configurações do Samba" samba_tabs top

# Aba 1: Compartilhamentos
add page "Compartilhamentos" tab_shares current

add label "<b>Gerenciar Compartilhamentos de Arquivos</b>" shares_title
set shares_title stylesheet "qproperty-textInteractionFlags: NoTextInteraction; margin: 10px;"

add frame horizontal
add label "Nome do compartilhamento:" lbl_share_name
set lbl_share_name stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"
add textbox "" txt_share_name
set txt_share_name placeholder "Ex: publico"
end frame

add frame horizontal
add label "Caminho:" lbl_share_path
set lbl_share_path stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"
add textbox "" txt_share_path
set txt_share_path placeholder "/srv/samba/publico"
end frame

add checkbox "&Compartilhamento público (sem senha)" chk_public
add checkbox "&Somente leitura" chk_readonly
add checkbox "&Permitir navegação" chk_browseable

add frame horizontal
add stretch
add pushbutton "&Adicionar Compartilhamento" btn_add_share
add pushbutton "&Remover Selecionado" btn_remove_share
end frame

add separator

add label "Compartilhamentos existentes:" lbl_existing
set lbl_existing stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"

add listbox "Compartilhamentos" list_shares selection
add item "publico (/srv/samba/publico)" share1
add item "privado (/home/user/privado)" share2
add item "backup (/mnt/backup)" share3
end listbox

end page

# Aba 2: Permissões
add page "Permissões" tab_perms

add label "<b>Configurações de Permissões</b>" perms_title
set perms_title stylesheet "qproperty-textInteractionFlags: NoTextInteraction; margin: 10px;"

add groupbox "Permissões Globais" grp_global_perms vertical
add checkbox "&Herdar permissões do sistema de arquivos" chk_inherit_perms
add checkbox "&Forçar permissões de grupo" chk_force_group
add checkbox "&Máscara de criação (create mask)" chk_create_mask
add frame horizontal
add label "Máscara:" lbl_mask
set lbl_mask stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"
add textbox "" txt_mask
set txt_mask text "0755"
set txt_mask maxwidth 100
end frame
end groupbox

add separator

add groupbox "Permissões por Usuário/Grupo" grp_user_perms vertical
add frame horizontal
add label "Usuário válido:" lbl_valid_users
set lbl_valid_users stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"
add textbox "" txt_valid_users
set txt_valid_users placeholder "user1,user2"
end frame

add frame horizontal
add label "Grupo válido:" lbl_valid_groups
set lbl_valid_groups stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"
add textbox "" txt_valid_groups
set txt_valid_groups placeholder "samba_users"
end frame

add checkbox "&Permitir usuários convidados" chk_guest_ok
end groupbox

end page

# Aba 3: Autenticação
add page "Autenticação" tab_auth

add label "<b>Configurações de Autenticação</b>" auth_title
set auth_title stylesheet "qproperty-textInteractionFlags: NoTextInteraction; margin: 10px;"

add groupbox "Método de Autenticação" grp_auth_method vertical
add radiobutton "&Autenticação local (smbpasswd)" rb_local_auth checked
add radiobutton "&Integração com Active Directory" rb_ad_auth
add radiobutton "&LDAP" rb_ldap_auth
add radiobutton "&Kerberos" rb_kerberos_auth
end groupbox

add separator

add groupbox "Configurações de Senha" grp_password vertical
add checkbox "&Criptografia de senha obrigatória" chk_encrypt_passwd checked
add checkbox "&Alteração de senha permitida" chk_change_passwd
add frame horizontal
add label "Tamanho mínimo da senha:" lbl_min_passwd_len
set lbl_min_passwd_len stylesheet "qproperty-textInteractionFlags: NoTextInteraction;"
add slider "" slider_min_passwd
set slider_min_passwd min 0
set slider_min_passwd max 20
set slider_min_passwd value 8
add label "8" lbl_passwd_len
end frame
end groupbox

add separator

add frame horizontal
add stretch
add pushbutton "&Testar Autenticação" btn_test_auth
add pushbutton "&Aplicar Configurações" btn_apply_auth
end frame

end page

end tabs

end frame

# Botões principais na parte inferior
add separator
add frame horizontal
add stretch
add pushbutton "&Aplicar Todas as Configurações" btn_apply_all default
add pushbutton "&Salvar Configuração" btn_save_config
add pushbutton "&Cancelar" btn_cancel exit
end frame

set title "Painel de Configuração do Samba - ShowBox Demo"
set btn_apply_all icon "dialog-ok"
set btn_save_config icon "document-save"
set btn_cancel icon "dialog-cancel"

show
EODEMO

echo "Demo do painel de configuração do Samba concluído. Código de saída: $?"

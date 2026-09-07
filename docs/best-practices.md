# Boas Práticas

Recomendações para escrever scripts Showbox eficientes e manuteníveis.

---

## Organização de Código

### Separar Interface da Lógica

```bash
#!/bin/bash

# === CONFIGURAÇÕES ===
TITULO="Minha Aplicação"
VERSION="1.0"

# === INTERFACE (heredoc separado) ===
read -r -d '' UI << 'EOUI'
set title "$TITULO v$VERSION"
add label "Bem-vindo!"
add pushbutton "Iniciar" btn_start apply exit default
show
EOUI

# === PROCESSAMENTO ===
process_output() {
    eval "$1"
    echo "Botão clicado: $btn_start"
}

# === EXECUÇÃO ===
OUTPUT=$(echo "$UI" | showbox)
[ $? -eq 1 ] && process_output "$OUTPUT"
```

---

### Usar Funções para Widgets Complexos

```bash
#!/bin/bash

# Função para criar botão estilizado
add_styled_button() {
    local title="$1"
    local name="$2"
    local color="${3:-#2196F3}"

    cat << EOF
add pushbutton "$title" $name
set $name stylesheet "background: $color; color: white; padding: 8px 16px; border-radius: 4px;"
EOF
}

# Uso
showbox << EOF
$(add_styled_button "Salvar" btn_save "#4CAF50")
$(add_styled_button "Cancelar" btn_cancel "#f44336")
show
EOF
```

---

## Nomeação de Widgets

### Convenções Recomendadas

| Tipo         | Prefixo  | Exemplo                     |
| ------------ | -------- | --------------------------- |
| Pushbutton   | `btn_`   | `btn_save`, `btn_cancel`    |
| Checkbox     | `chk_`   | `chk_remember`, `chk_agree` |
| Radiobutton  | `radio_` | `radio_light`, `radio_dark` |
| Textbox      | `txt_`   | `txt_name`, `txt_email`     |
| Label        | `lbl_`   | `lbl_status`, `lbl_title`   |
| Listbox      | `lst_`   | `lst_files`, `lst_options`  |
| Combobox     | `cmb_`   | `cmb_country`               |
| Dropdownlist | `ddl_`   | `ddl_category`              |
| Slider       | `sld_`   | `sld_volume`                |
| Progressbar  | `pb_`    | `pb_download`               |
| Frame        | `frm_`   | `frm_buttons`               |
| Groupbox     | `grp_`   | `grp_settings`              |
| Tabs         | `tabs_`  | `tabs_main`                 |
| Page         | `pg_`    | `pg_general`                |
| Table        | `tbl_`   | `tbl_data`                  |
| Chart        | `chart_` | `chart_sales`               |
| Calendar     | `cal_`   | `cal_date`                  |

### Nomes Descritivos

```bash
# ❌ Ruim
add textbox "Nome:" t1
add textbox "Email:" t2

# ✅ Bom
add textbox "Nome:" txt_user_name
add textbox "Email:" txt_user_email
```

---

## Performance

### Use --hidden para Interfaces Complexas

```bash
# Evita "piscadas" enquanto constrói a interface
showbox --hidden << 'EOF'
# ... muitos widgets ...
show  # Mostra tudo de uma vez
EOF
```

### Minimize Atualizações em Loop

```bash
# ❌ Ruim - Atualiza a cada iteração
for i in {1..100}; do
    echo "set pb_download value $i" >&$OUTPUTFD
    sleep 0.01
done

# ✅ Bom - Atualiza a cada 5%
for i in {5..100..5}; do
    echo "set pb_download value $i" >&$OUTPUTFD
    sleep 0.5
done
```

### Batch de Comandos

```bash
# ❌ Ruim - Comandos separados
echo "set lbl1 text 'A'" >&$OUTPUTFD
echo "set lbl2 text 'B'" >&$OUTPUTFD
echo "set lbl3 text 'C'" >&$OUTPUTFD

# ✅ Bom - Comandos em bloco
cat >&$OUTPUTFD << 'EOF'
set lbl1 text "A"
set lbl2 text "B"
set lbl3 text "C"
EOF
```

---

## Tratamento de Erros

### Verificar Código de Saída

```bash
OUTPUT=$(showbox << 'EOF'
add pushbutton "OK" btn_ok apply exit default
add pushbutton "Cancelar" btn_cancel exit
EOF
)

case $? in
    0)
        echo "Usuário cancelou ou fechou a janela"
        exit 0
        ;;
    1)
        echo "Usuário confirmou"
        eval "$OUTPUT"
        ;;
    *)
        echo "Erro desconhecido"
        exit 1
        ;;
esac
```

### Validar Entrada

```bash
OUTPUT=$(showbox << 'EOF'
add textbox "Email:" txt_email
add pushbutton "Enviar" btn_send apply exit default
EOF
)

if [ $? -eq 1 ]; then
    eval "$OUTPUT"

    # Validar email
    if [[ ! "$txt_email" =~ ^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Z|a-z]{2,}$ ]]; then
        showbox << 'EOF'
add label "Email inválido!"
add pushbutton "OK" btn_ok exit default
show
EOF
        exit 1
    fi
fi
```

---

## Templates Reutilizáveis

### Diálogo de Confirmação

```bash
confirm_dialog() {
    local message="$1"
    local title="${2:-Confirmação}"

    showbox << EOF
set title "$title"
add label "$message"
add frame frm horizontal
add stretch
add pushbutton "Não" btn_no exit
add pushbutton "Sim" btn_yes apply exit default
end frame
show
EOF
    return $?
}

# Uso
if confirm_dialog "Deseja continuar?" "Confirme"; then
    echo "Usuário confirmou"
fi
```

### Diálogo de Erro

```bash
error_dialog() {
    local message="$1"

    showbox << EOF
set title "Erro"
set icon "dialog-error"
add label "<span style='color:red'>⚠️ $message</span>"
add pushbutton "OK" btn_ok exit default
show
EOF
}

# Uso
error_dialog "Arquivo não encontrado!"
```

### Diálogo de Entrada

```bash
input_dialog() {
    local prompt="$1"
    local default_value="${2:-}"
    local result

    result=$(showbox << EOF
set title "Entrada"
add textbox "$prompt" txt_input text "$default_value"
add frame frm horizontal
add stretch
add pushbutton "Cancelar" btn_cancel exit
add pushbutton "OK" btn_ok apply exit default
end frame
show
EOF
)

    if [ $? -eq 1 ]; then
        eval "$result"
        echo "$txt_input"
        return 0
    fi
    return 1
}

# Uso
nome=$(input_dialog "Digite seu nome:" "João")
echo "Nome digitado: $nome"
```

---

## Manutenibilidade

### Comentários

```bash
showbox << 'EOF'
# === CABEÇALHO ===
set title "Minha App"

# === FORMULÁRIO ===
add groupbox "Dados Pessoais" grp_dados vertical
add textbox "Nome:" txt_nome
add textbox "Email:" txt_email
end groupbox

# === AÇÕES ===
add frame frm_acoes horizontal
add stretch
add pushbutton "Cancelar" btn_cancel exit
add pushbutton "Salvar" btn_save apply exit default
end frame

show
EOF
```

### Versionar Scripts

Mantenha versão e changelog:

```bash
#!/bin/bash
# Script: meu_app.sh
# Versão: 1.2.0
# Changelog:
#   1.2.0 - Adicionado suporte a temas
#   1.1.0 - Corrigido bug de layout
#   1.0.0 - Versão inicial
```

---

## Próximos Passos

- [Segurança](security.md)
- [Resolução de Problemas](troubleshooting.md)

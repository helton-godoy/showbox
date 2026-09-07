# Guia de Layouts

Aprenda a criar interfaces complexas com o sistema de layouts do Showbox.

---

## Sistema de 3 Níveis

O Showbox usa um sistema hierárquico de layouts:

```text
┌────────────────────────────────────────────────────────────┐
│ NÍVEL 1: Layout Vertical Principal (coluna)                │
│                                                            │
│  ┌─────────────────────────────────────────────────────┐   │
│  │ NÍVEL 2: Layout Horizontal (linha)                  │   │
│  │                                                     │   │
│  │  ┌──────────────┐  ┌──────────────┐                 │   │
│  │  │ NÍVEL 3      │  │ NÍVEL 3      │                 │   │
│  │  │ Vertical     │  │ Vertical     │                 │   │
│  │  │              │  │              │                 │   │
│  │  │ [Widget 1]   │  │ [Widget 3]   │                 │   │
│  │  │ [Widget 2]   │  │ [Widget 4]   │                 │   │
│  │  └──────────────┘  └──────────────┘                 │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                            │
│  ┌─────────────────────────────────────────────────────┐   │
│  │ NÍVEL 2: Layout Horizontal (nova linha)             │   │
│  │                                                     │   │
│  │  ┌──────────────┐                                   │   │
│  │  │ NÍVEL 3      │                                   │   │
│  │  │ [Widget 5]   │                                   │   │
│  │  └──────────────┘                                   │   │
│  └─────────────────────────────────────────────────────┘   │
└────────────────────────────────────────────────────────────┘
```

---

## Comandos de Layout

### step horizontal

Cria **nova coluna** à direita da posição atual.

```bash
add label "Coluna 1"
step horizontal
add label "Coluna 2"
step horizontal
add label "Coluna 3"
```

**Resultado:**

```text
┌──────────┐ ┌──────────┐ ┌──────────┐
│ Coluna 1 │ │ Coluna 2 │ │ Coluna 3 │
└──────────┘ └──────────┘ └──────────┘
```

---

### step vertical

Cria **nova linha** abaixo da posição atual.

```bash
add label "Linha 1"
step vertical
add label "Linha 2"
step vertical
add label "Linha 3"
```

**Resultado:**

```text
┌──────────┐
│ Linha 1  │
└──────────┘
┌──────────┐
│ Linha 2  │
└──────────┘
┌──────────┐
│ Linha 3  │
└──────────┘
```

---

## Containers: frame e groupbox

### frame

Container invisível para agrupar widgets.

```bash
add frame frm_nome [vertical|horizontal] [estilo...]
# ... widgets ...
end frame
```

### groupbox

Container com título visível.

```bash
add groupbox "Título" grp_nome [vertical|horizontal]
# ... widgets ...
end groupbox
```

> **Importante:** O layout padrão dentro de containers é **horizontal**.
> Use `vertical` se quiser empilhar widgets.

---

## Padrões de Layout

### Padrão 1: Formulário Simples

Labels à esquerda, campos à direita.

```bash
#!/bin/bash
showbox << 'EOF'
set title "Formulário"

add textbox "Nome:" txt_nome
add textbox "Email:" txt_email
add textbox "Telefone:" txt_tel

add separator

add frame frm_botoes horizontal
add stretch
add pushbutton "Cancelar" btn_cancel exit
add pushbutton "Salvar" btn_save apply exit default
end frame

show
EOF
```

---

### Padrão 2: Sidebar + Conteúdo

Menu à esquerda, conteúdo à direita.

```bash
#!/bin/bash
showbox --resizable << 'EOF'
set title "Painel de Controle"

# === SIDEBAR (coluna 1) ===
add frame sidebar vertical panel
add label "<b>Menu</b>"
set sidebar stylesheet "min-width: 150px; padding: 5px;"

add pushbutton "Dashboard" btn_dash
add pushbutton "Configurações" btn_config
add pushbutton "Relatórios" btn_report
add separator horizontal
add pushbutton "Sair" btn_exit exit
add stretch
end frame

# === CONTEÚDO (coluna 2) ===
step horizontal

add tabs tabs_content

add page "Dashboard" pg_dash current
add label "<h2>Dashboard</h2>"
add progressbar pb_status busy

add page "Configurações" pg_config
add checkbox "Opção 1" cfg_opt1 checked
add checkbox "Opção 2" cfg_opt2

add page "Relatórios" pg_report
add label "Nenhum relatório disponível"

end tabs

show
EOF
```

**Resultado Visual:**

```text
┌────────────────────────────────────────────────────────┐
│ Painel de Controle                                     │
├──────────────┬─────────────────────────────────────────┤
│  Menu        │ [Dashboard] [Configurações] [Relatórios]│
│              │─────────────────────────────────────────│
│ [Dashboard]  │                                         │
│ [Config]     │  Dashboard                              │
│ [Relatórios] │  ═══════════                            │
│ ────────     │                                         │
│ [Sair]       │                                         │
│              │                                         │
└──────────────┴─────────────────────────────────────────┘
```

---

### Padrão 3: Grade de Botões

Múltiplas linhas e colunas.

```bash
#!/bin/bash
showbox << 'EOF'
set title "Seletor de Ícones"

# Linha 1
add frame row1 horizontal
add stretch
add pushbutton "Copiar" btn_copy
set btn_copy icon "edit-copy"
add pushbutton "Colar" btn_paste
set btn_paste icon "edit-paste"
add pushbutton "Recortar" btn_cut
set btn_cut icon "edit-cut"
add stretch
end frame

# Linha 2
add frame row2 horizontal
add stretch
add pushbutton "Desfazer" btn_undo
set btn_undo icon "edit-undo"
add pushbutton "Refazer" btn_redo
set btn_redo icon "edit-redo"
add pushbutton "Salvar" btn_save
set btn_save icon "document-save"
add stretch
end frame

add separator

add pushbutton "Fechar" btn_close exit default

show
EOF
```

---

### Padrão 4: Abas com Conteúdo Complexo

Cada aba com seu próprio layout.

```bash
#!/bin/bash
showbox --resizable << 'EOF'
set title "Configurações do Sistema"

add tabs tabs_main

# === ABA 1: GERAL ===
add page "Geral" pg_geral current

add groupbox "Aparência" grp_aparencia vertical
add radiobutton "Tema Claro" radio_light checked
add radiobutton "Tema Escuro" radio_dark
add radiobutton "Automático" radio_auto
end groupbox

add groupbox "Sistema" grp_sistema vertical
add checkbox "Iniciar com o sistema" chk_autostart
add checkbox "Minimizar para bandeja" chk_tray checked
end groupbox

# === ABA 2: AVANÇADO ===
add page "Avançado" pg_avancado

add groupbox "Performance" grp_perf vertical
add slider sld_cache minimum 64 maximum 512
add label "Cache: 64-512 MB"
end groupbox

add groupbox "Logs" grp_logs vertical checkable
add checkbox "Ativar logs" chk_logs checked
add checkbox "Logs detalhados" chk_verbose
end groupbox

end tabs

# === RODAPÉ ===
add separator

add frame frm_footer horizontal
add stretch
add pushbutton "Cancelar" btn_cancel exit
add pushbutton "Aplicar" btn_apply apply exit default
end frame

show
EOF
```

---

## Dicas de Layout

### 1. Use stretch para Alinhamento

```bash
# Alinhar à direita
add frame frm horizontal
add stretch
add pushbutton "Botão"
end frame

# Centralizar
add frame frm horizontal
add stretch
add pushbutton "Botão"
add stretch
end frame

# Alinhar à esquerda
add frame frm horizontal
add pushbutton "Botão"
add stretch
end frame
```

### 2. Defina Larguras Mínimas

```bash
add frame sidebar vertical
set sidebar stylesheet "min-width: 200px;"
# ...
end frame
```

### 3. Use --hidden para Interfaces Complexas

```bash
showbox --hidden << 'EOF'
# Construir toda a interface aqui
# ...
# Só mostrar quando pronto
show
EOF
```

### 4. Evite Aninhamento Excessivo

Prefira:

```bash
add label "Widget 1"
step horizontal
add label "Widget 2"
```

Em vez de:

```bash
add frame outer horizontal
add frame inner1 vertical
add label "Widget 1"
end frame
add frame inner2 vertical
add label "Widget 2"
end frame
end frame
```

---

## Problema Comum: Elementos Empilhados

**Problema:** Widgets aparecem um sobre o outro em vez de lado a lado.

**Causa:** Usar `add frame horizontal` não cria colunas no nível raiz.

**Solução:** Use `step horizontal` no nível raiz:

```bash
# ❌ ERRADO
add frame col1 horizontal
add label "Coluna 1"
end frame
add frame col2 horizontal
add label "Coluna 2"
end frame

# ✅ CORRETO
add frame col1 vertical
add label "Coluna 1"
end frame
step horizontal
add frame col2 vertical
add label "Coluna 2"
end frame
```

---

## Próximos Passos

- [Funcionalidades Avançadas](advanced-features.md) - Stylesheets, comunicação bidirecional
- [Resolução de Problemas](troubleshooting.md) - Erros comuns e soluções

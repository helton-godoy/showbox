# Tutorial 03: Painel de Controle

Criando uma interface com sidebar e abas.

**Nível:** 🟡 Intermediário  
**Tempo estimado:** 15 minutos

---

## Objetivo

Criar um painel de controle profissional com:

- Menu lateral (sidebar) fixo
- Área principal com abas
- Layout responsivo

---

## Código Completo

```bash
#!/bin/bash
#
# Tutorial 03: Painel de Controle
# Demonstra layout sidebar + tabs

showbox --resizable << 'EOF'
set title "Painel de Controle"

# ============================================
# SIDEBAR (coluna esquerda)
# ============================================
add frame sidebar vertical panel
set sidebar stylesheet "
    min-width: 180px;
    max-width: 180px;
    background-color: #f5f5f5;
    padding: 10px;
"

add label "<b>📊 Menu</b>" lbl_menu
set lbl_menu stylesheet "font-size: 16px; margin-bottom: 10px;"

add pushbutton "Dashboard" btn_dash
set btn_dash stylesheet "text-align: left; padding: 8px;"
set btn_dash icon "go-home"

add pushbutton "Usuários" btn_users
set btn_users stylesheet "text-align: left; padding: 8px;"
set btn_users icon "system-users"

add pushbutton "Configurações" btn_config
set btn_config stylesheet "text-align: left; padding: 8px;"
set btn_config icon "preferences-system"

add pushbutton "Relatórios" btn_reports
set btn_reports stylesheet "text-align: left; padding: 8px;"
set btn_reports icon "x-office-spreadsheet"

add separator horizontal

add stretch

add pushbutton "Sair" btn_exit exit
set btn_exit stylesheet "text-align: left; padding: 8px; color: #c62828;"
set btn_exit icon "system-log-out"

end frame

# ============================================
# ÁREA PRINCIPAL (coluna direita)
# step horizontal cria nova coluna!
# ============================================
step horizontal

add tabs tabs_main

# --- Aba Dashboard ---
add page "Dashboard" pg_dash current
set pg_dash icon "go-home"

add label "<h2>Dashboard</h2>" lbl_dash_title

add frame stats_row horizontal
add groupbox "Usuários Ativos" grp_users vertical
add label "<h1 style='color:#1976D2'>1,234</h1>"
end groupbox

add groupbox "Vendas Hoje" grp_sales vertical
add label "<h1 style='color:#388E3C'>R$ 15.780</h1>"
end groupbox

add groupbox "Alertas" grp_alerts vertical
add label "<h1 style='color:#F57C00'>3</h1>"
end groupbox
end frame

add label "Status do Sistema:"
add progressbar pb_status busy

# --- Aba Usuários ---
add page "Usuários" pg_users
set pg_users icon "system-users"

add label "<h2>Gerenciar Usuários</h2>"
add table "Nome;Email;Perfil;Status" tbl_users search selection
set tbl_users stylesheet "min-height: 200px;"

# --- Aba Configurações ---
add page "Configurações" pg_config
set pg_config icon "preferences-system"

add label "<h2>Configurações</h2>"

add groupbox "Aparência" grp_aparencia vertical
add radiobutton "Tema Claro" radio_light checked
add radiobutton "Tema Escuro" radio_dark
end groupbox

add groupbox "Notificações" grp_notif vertical checkable checked
add checkbox "Email" chk_email checked
add checkbox "Push" chk_push
add checkbox "SMS" chk_sms
end groupbox

end tabs

show
EOF

echo "Painel fechado"
```

---

## Conceitos-Chave

### Layout Lado a Lado

O segredo é usar `step horizontal` no **nível raiz** do diálogo:

```bash
add frame sidebar vertical
# ... conteúdo da sidebar ...
end frame

step horizontal   # ← Cria nova coluna à direita

add tabs tabs_main
# ... conteúdo das abas ...
end tabs
```

### Largura Fixa da Sidebar

```bash
set sidebar stylesheet "
    min-width: 180px;
    max-width: 180px;
"
```

Força largura fixa enquanto o conteúdo principal é flexível.

### Abas com Ícones

```bash
add page "Dashboard" pg_dash current
set pg_dash icon "go-home"
```

---

## Diagrama do Layout

```
┌─────────────────────────────────────────────────────────────┐
│ Painel de Controle                                          │
├───────────────┬─────────────────────────────────────────────┤
│    SIDEBAR    │              ÁREA DE ABAS                   │
│               │  [Dashboard] [Usuários] [Configurações]     │
│  📊 Menu      │ ─────────────────────────────────────────── │
│               │                                             │
│  [Dashboard]  │  Dashboard                                  │
│  [Usuários]   │  ┌─────────┐ ┌─────────┐ ┌─────────┐       │
│  [Config]     │  │ 1,234   │ │R$15.780 │ │    3    │       │
│  [Relatórios] │  │ Ativos  │ │ Vendas  │ │ Alertas │       │
│               │  └─────────┘ └─────────┘ └─────────┘       │
│  ─────────    │                                             │
│               │  Status: ═══════════════                   │
│  [Sair]       │                                             │
└───────────────┴─────────────────────────────────────────────┘
```

---

## Próximo Tutorial

[04. Tabela de Dados →](04-tabela-dados.md)

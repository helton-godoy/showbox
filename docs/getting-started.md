# Início Rápido

Aprenda a criar interfaces gráficas com Showbox em minutos! Este guia apresenta os conceitos fundamentais através de exemplos progressivos.

---

## Conceitos Fundamentais

### Como o Showbox Funciona

1. **Seu script** envia comandos via **stdin**
2. **Showbox** interpreta os comandos e cria a interface
3. **Usuário** interage com a interface
4. **Showbox** reporta eventos via **stdout**

````text
Script ──stdin──▶ Showbox ──GUI──▶ Usuário
                     │
                     └──stdout──▶ Script (eventos)
```text

### Estrutura Básica de um Script

```bash
#!/bin/bash

showbox << 'EOF'
# Configurar a janela
set title "Meu Programa"

# Adicionar widgets
add label "Olá!"
add pushbutton "Fechar" btn_fechar exit

# Mostrar a janela
show
EOF

# Aqui você pode processar a saída
echo "Diálogo fechado com código: $?"
```text

---

## Exemplo 1: Olá Mundo

O diálogo mais simples possível.

```bash
#!/bin/bash

showbox << 'EOF'
set title "Olá Mundo"
add label "Bem-vindo ao Showbox!"
add pushbutton "OK" btn_ok exit default
show
EOF
```text

**O que acontece:**

- `set title` - Define o título da janela
- `add label` - Adiciona texto
- `add pushbutton` - Adiciona botão
  - `exit` - Fecha o diálogo ao clicar
  - `default` - Botão padrão (Enter ativa)
- `show` - Exibe a janela

---

## Exemplo 2: Entrada de Dados

Coletando informações do usuário.

```bash
#!/bin/bash

OUTPUT=$(showbox << 'EOF'
set title "Cadastro"

add textbox "Nome:" txt_nome
add textbox "Email:" txt_email

add frame btn_frame horizontal
add stretch
add pushbutton "Cancelar" btn_cancel exit
add pushbutton "Salvar" btn_ok apply exit default
end frame

show
EOF
)

# Verificar se o usuário clicou em Salvar
if [ $? -eq 1 ]; then
    # Processar os dados
    eval "$OUTPUT"
    echo "Nome: $txt_nome"
    echo "Email: $txt_email"
else
    echo "Usuário cancelou"
fi
```text

**Conceitos novos:**

- `textbox` - Campo de entrada de texto
- `apply` - Botão reporta valores dos widgets
- `$(...)` - Captura a saída do showbox
- `eval "$OUTPUT"` - Transforma em variáveis shell

---

## Exemplo 3: Opções com Checkbox

Permitindo múltiplas seleções.

```bash
#!/bin/bash

OUTPUT=$(showbox << 'EOF'
set title "Configurações"

add groupbox "Opções" grp_opcoes vertical
add checkbox "Habilitar logs" chk_logs checked
add checkbox "Modo debug" chk_debug
add checkbox "Notificações" chk_notify checked
end groupbox

add pushbutton "Aplicar" btn_apply apply exit default

show
EOF
)

if [ $? -eq 1 ]; then
    eval "$OUTPUT"

    [ "$chk_logs" = "1" ] && echo "Logs: Habilitado"
    [ "$chk_debug" = "1" ] && echo "Debug: Habilitado"
    [ "$chk_notify" = "1" ] && echo "Notificações: Habilitado"
fi
```text

**Conceitos novos:**

- `groupbox` - Agrupa widgets visualmente
- `checkbox` - Caixa de seleção
- `checked` - Marcado por padrão
- Valores: `1` = marcado, `0` = desmarcado

---

## Exemplo 4: Seleção Única (Radiobutton)

Quando apenas uma opção é válida.

```bash
#!/bin/bash

OUTPUT=$(showbox << 'EOF'
set title "Escolha o Tema"

add groupbox "Tema da Interface" grp_tema vertical
add radiobutton "Claro" radio_claro checked
add radiobutton "Escuro" radio_escuro
add radiobutton "Automático" radio_auto
end groupbox

add pushbutton "Aplicar" btn_apply apply exit default

show
EOF
)

if [ $? -eq 1 ]; then
    eval "$OUTPUT"

    if [ "$radio_claro" = "1" ]; then
        echo "Tema selecionado: Claro"
    elif [ "$radio_escuro" = "1" ]; then
        echo "Tema selecionado: Escuro"
    else
        echo "Tema selecionado: Automático"
    fi
fi
```text

**Conceitos novos:**

- `radiobutton` - Botão de opção
- Dentro de um `groupbox`, apenas um pode estar selecionado

---

## Exemplo 5: Lista de Seleção

Escolhendo de uma lista predefinida.

```bash
#!/bin/bash

OUTPUT=$(showbox << 'EOF'
set title "Selecione o Sistema"

add listbox "Sistemas Disponíveis:" lst_sistemas selection
add item "Ubuntu 24.04 LTS" current
add item "Debian 13 Trixie"
add item "Fedora 40"
add item "Arch Linux"
end listbox

add pushbutton "Selecionar" btn_select apply exit default

show
EOF
)

if [ $? -eq 1 ]; then
    eval "$OUTPUT"
    echo "Sistema selecionado: $lst_sistemas"
fi
```text

**Conceitos novos:**

- `listbox` - Lista de itens
- `item` - Adiciona item à lista
- `selection` - Reporta seleção imediatamente
- `current` - Item selecionado por padrão

---

## Dicas Rápidas

### Heredoc com Variáveis

Use `<< 'EOF'` (com aspas) para texto literal:

```bash
showbox << 'EOF'
add label "Texto literal $variavel não expande"
EOF
```text

Use `<< EOF` (sem aspas) para expandir variáveis:

```bash
NOME="João"
showbox << EOF
add label "Olá, $NOME!"
EOF
```text

### Estilização Rápida

```bash
add pushbutton "Botão Estilizado" btn_style
set btn_style stylesheet "background-color: #2196F3; color: white; padding: 10px;"
```text

### Modo Oculto

Para evitar "piscadas" ao construir interfaces complexas:

```bash
showbox --hidden << 'EOF'
# ... muitos widgets ...
show  # Mostra só quando tudo estiver pronto
EOF
```text

---

## Próximos Passos

Agora que você conhece o básico:

1. **[Referência de Widgets](widgets-reference.md)** - Todos os 20 widgets disponíveis
2. **[Guia de Layouts](layouts.md)** - Criar interfaces complexas
3. **[Funcionalidades Avançadas](advanced-features.md)** - Comunicação bidirecional, estilos
````

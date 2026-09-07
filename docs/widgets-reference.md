# Referência de Widgets

Documentação completa de todos os widgets disponíveis no Showbox.

---

## Visão Geral

| Categoria      | Widgets                                                                           |
| -------------- | --------------------------------------------------------------------------------- |
| **Ação**       | pushbutton                                                                        |
| **Entrada**    | checkbox, radiobutton, textbox, combobox, dropdownlist, listbox, slider, calendar |
| **Exibição**   | label, progressbar, textview, separator, chart, table                             |
| **Layout**     | frame, groupbox, tabs, page                                                       |
| **Auxiliares** | space, stretch, item                                                              |

---

## Widgets de Ação

### pushbutton

Botão de comando clicável.

**Sintaxe:**

```text
add pushbutton "título" nome [apply] [exit] [default] [checkable] [checked]
```

**Opções:**

| Opção       | Descrição                                       |
| ----------- | ----------------------------------------------- |
| `apply`     | Ao clicar, reporta valores de todos os widgets  |
| `exit`      | Ao clicar, fecha o diálogo                      |
| `default`   | Botão padrão (ativado por Enter)                |
| `checkable` | Botão toggle (mantém estado pressionado)        |
| `checked`   | Estado inicial pressionado (requer `checkable`) |

**Eventos (stdout):**

```bash
nome=clicked          # Botão normal
nome=pressed          # Botão toggle pressionado
nome=released         # Botão toggle liberado
```

**Exemplo:**

```bash
add pushbutton "Salvar" btn_save apply exit default
set btn_save icon "document-save"
set btn_save stylesheet "background: #4CAF50; color: white; padding: 8px 16px;"
```

---

## Widgets de Entrada

### checkbox

Caixa de seleção para opções independentes.

**Sintaxe:**

```text
add checkbox "título" nome [checked]
```

**Valor reportado:** `1` (marcado) ou `0` (desmarcado)

**Exemplo:**

```bash
add checkbox "Aceito os termos" chk_termos
add checkbox "Lembrar senha" chk_lembrar checked
```

---

### radiobutton

Botão de opção para seleção exclusiva em grupo.

**Sintaxe:**

```text
add radiobutton "título" nome [checked]
```

> **Importante:** Use dentro de um `groupbox` para agrupar opções mutuamente exclusivas.

**Valor reportado:** `1` (selecionado) ou `0`

**Exemplo:**

```bash
add groupbox "Tamanho" grp_tam vertical
add radiobutton "Pequeno" radio_p
add radiobutton "Médio" radio_m checked
add radiobutton "Grande" radio_g
end groupbox
```

---

### textbox

Campo de entrada de texto em linha única.

**Sintaxe:**

```text
add textbox "título" nome [password] [text "valor inicial"] [placeholder "dica"]
```

**Opções:**

| Opção         | Descrição                      |
| ------------- | ------------------------------ |
| `password`    | Oculta caracteres (asteriscos) |
| `text`        | Valor inicial do campo         |
| `placeholder` | Texto de dica quando vazio     |

**Valor reportado:** Texto digitado pelo usuário

**Exemplo:**

```bash
add textbox "Usuário:" txt_user
add textbox "Senha:" txt_pass password
set txt_user placeholder "Digite seu email"
```

---

### listbox

Lista de itens com seleção.

**Sintaxe:**

```text
add listbox "título" nome [activation] [selection]
add item "texto" [ícone] [current]
...
end listbox
```

**Opções:**

| Opção        | Descrição                                   |
| ------------ | ------------------------------------------- |
| `activation` | Reporta ao ativar item (duplo clique/Enter) |
| `selection`  | Reporta ao selecionar item                  |

**Valor reportado:** Texto do item selecionado

**Exemplo:**

```bash
add listbox "Arquivos:" lst_arquivos selection
add item "documento.pdf" "document-open" current
add item "imagem.png" "image-x-generic"
add item "planilha.csv" "x-office-spreadsheet"
end listbox
```

---

### dropdownlist

Lista suspensa de seleção única (não editável).

**Sintaxe:**

```text
add dropdownlist "título" nome [selection]
add item "texto" [current]
...
end dropdownlist
```

**Valor reportado:** Texto do item selecionado

**Exemplo:**

```bash
add dropdownlist "País:" ddl_pais selection
add item "Brasil" current
add item "Portugal"
add item "Angola"
end dropdownlist
```

---

### combobox

Lista suspensa editável.

**Sintaxe:**

```text
add combobox "título" nome [selection]
add item "texto" [current]
...
end combobox
```

**Valor reportado:** Texto digitado/selecionado

**Exemplo:**

```bash
add combobox "Cidade:" cmb_cidade
add item "São Paulo" current
add item "Rio de Janeiro"
add item "Belo Horizonte"
end combobox
```

---

### slider

Controle deslizante para valores numéricos.

**Sintaxe:**

```text
add slider nome [vertical] [minimum valor] [maximum valor]
```

**Padrões:** minimum=0, maximum=100

**Evento:** Reporta imediatamente ao mover: `nome=valor`

**Exemplo:**

```bash
add slider sld_volume minimum 0 maximum 100
set sld_volume value 75
```

---

### calendar

Seletor de data interativo.

**Sintaxe:**

```text
add calendar "título" nome [date "YYYY-MM-DD"] [minimum "YYYY-MM-DD"] [maximum "YYYY-MM-DD"] [format "formato"] [selection] [navigation]
```

**Opções:**

| Opção        | Descrição                           |
| ------------ | ----------------------------------- |
| `date`       | Data inicial selecionada            |
| `minimum`    | Data mínima permitida               |
| `maximum`    | Data máxima permitida               |
| `format`     | Formato de saída (ex: "dd/MM/yyyy") |
| `selection`  | Reporta ao selecionar data          |
| `navigation` | Mostra controles de navegação       |

**Valor reportado:** Data no formato especificado

**Exemplo:**

```bash
add calendar "Data de Nascimento:" cal_nascimento selection navigation
set cal_nascimento date "1990-01-15"
set cal_nascimento format "dd/MM/yyyy"
```

---

## Widgets de Exibição

### label

Exibe texto, imagem ou animação.

**Sintaxe:**

```text
add label "conteúdo" nome [picture] [animation]
```

**Modos:**

- **Texto (padrão):** Suporta HTML básico: `<b>`, `<i>`, `<h1>`, etc.
- **Imagem:** `add label "/caminho/imagem.png" lbl_img picture`
- **Animação:** `add label "/caminho/animacao.gif" lbl_anim animation`

**Exemplo:**

```bash
add label "<h2>Título</h2><p>Descrição em <b>negrito</b></p>" lbl_info
add label "/usr/share/icons/logo.svg" lbl_logo picture
```

---

### progressbar

Barra de progresso.

**Sintaxe:**

```text
add progressbar nome [vertical] [busy]
```

**Propriedades:**

```bash
set nome value 50        # Define progresso (0-100)
set nome minimum 0       # Valor mínimo
set nome maximum 100     # Valor máximo
set nome busy            # Modo indeterminado (animação)
```

**Exemplo:**

```bash
add progressbar pb_download
set pb_download value 35
# ou modo indeterminado:
add progressbar pb_loading busy
```

---

### textview

Área de texto multi-linha (somente leitura).

**Sintaxe:**

```text
add textview nome [file "caminho"]
```

**Formatos suportados:** Texto plano, HTML

**Exemplo:**

```bash
add textview tv_licenca file "/usr/share/doc/showbox/LICENSE"
set tv_licenca stylesheet "font-family: monospace; min-width: 40em; min-height: 20em;"
```

---

### separator

Linha divisória visual.

**Sintaxe:**

```text
add separator [nome] [vertical] [plain|raised|sunken]
```

**Estilos:** `plain` (sólida), `raised` (3D elevada), `sunken` (3D afundada, padrão)

**Exemplo:**

```bash
add separator sep1 horizontal sunken
```

---

### table

Tabela editável com busca.

**Sintaxe:**

```text
add table "col1;col2;col3" nome [file "arquivo.csv"] [readonly] [selection] [search]
```

**Opções:**

| Opção       | Descrição                    |
| ----------- | ---------------------------- |
| `file`      | Carrega dados de arquivo CSV |
| `readonly`  | Impede edição                |
| `selection` | Reporta seleção de linha     |
| `search`    | Mostra campo de busca        |

**Eventos:**

```bash
nome[linha][coluna]=valor    # Célula editada
nome_selection=numero        # Linha selecionada
```

**Exemplo:**

```bash
add table "Nome;Email;Telefone" tbl_contatos search selection
set tbl_contatos file "contatos.csv"
```

---

### chart

Gráfico interativo (pizza, barras, etc.).

**Sintaxe:**

```text
add chart "título" nome
```

**Propriedades:**

```bash
set nome data "Label1:Valor1;Label2:Valor2;..."   # Dados
set nome append "NovoLabel:Valor"                  # Adiciona dados
set nome axis "horizontal|vertical"                # Tipo de eixo
set nome export "/caminho/grafico.png"            # Exporta imagem
```

**Evento:** `nome.slice["Label"]=valor` ao clicar em fatia

**Exemplo:**

```bash
add chart "Vendas por Região" chart_vendas
set chart_vendas data "Norte:1500;Sul:2300;Leste:1800;Oeste:2100"
```

---

## Widgets de Layout

### frame

Container com borda para agrupar widgets.

**Sintaxe:**

```text
add frame nome [vertical|horizontal] [noframe|box|panel|styled] [plain|raised|sunken]
...
end frame
```

**Padrão:** horizontal, sem borda

**Exemplo:**

```bash
add frame frm_botoes horizontal
add stretch
add pushbutton "Cancelar" btn_cancel exit
add pushbutton "OK" btn_ok apply exit
end frame
```

---

### groupbox

Container com título e borda.

**Sintaxe:**

```text
add groupbox "título" nome [vertical|horizontal] [checkable] [checked]
...
end groupbox
```

**Opções:**

- `checkable` - Adiciona checkbox no título
- `checked` - Estado inicial marcado

Quando desmarcado, todos os widgets filhos são desabilitados.

**Exemplo:**

```bash
add groupbox "Opções Avançadas" grp_avancado vertical checkable
add checkbox "Modo verbose" chk_verbose
add slider sld_timeout minimum 1 maximum 60
end groupbox
```

---

### tabs

Container com abas para múltiplas páginas.

**Sintaxe:**

```text
add tabs nome [top|bottom|left|right]
add page "título" nome_pagina [ícone] [current]
...
end tabs
```

**Exemplo:**

```bash
add tabs tabs_main

add page "Geral" pg_geral current
add label "Configurações gerais aqui"

add page "Avançado" pg_avancado
add label "Configurações avançadas aqui"

end tabs
```

---

### page

Página dentro de um `tabs`.

**Sintaxe:**

```text
add page "título" nome [ícone] [current]
```

> Ver exemplo completo em `tabs` acima.

---

## Widgets Auxiliares

### space

Adiciona espaço fixo no layout.

**Sintaxe:**

```text
add space [tamanho]
```

**Padrão:** 1 pixel

**Exemplo:**

```bash
add space 20  # 20 pixels de espaço
```

---

### stretch

Adiciona espaço flexível que ocupa área disponível.

**Sintaxe:**

```text
add stretch [fator]
```

**Padrão:** fator 1

**Exemplo:**

```bash
add frame btn_frame horizontal
add stretch          # Empurra botões para a direita
add pushbutton "OK" btn_ok
end frame
```

---

### item

Item para widgets de lista (listbox, combobox, dropdownlist).

**Sintaxe:**

```text
add item "texto" [ícone] [current]
```

**Opções:**

- `ícone` - Caminho para arquivo de ícone ou nome do tema
- `current` - Define como item selecionado

**Exemplo:**

```bash
add listbox "Opções:" lst_opcoes
add item "Primeira" "dialog-information" current
add item "Segunda" "dialog-question"
end listbox
```

---

## Propriedades Globais

Propriedades aplicáveis a qualquer widget via `set`:

| Propriedade  | Descrição             | Exemplo                             |
| ------------ | --------------------- | ----------------------------------- |
| `enabled`    | Habilita/desabilita   | `enable nome` / `disable nome`      |
| `visible`    | Mostra/oculta         | `show nome` / `hide nome`           |
| `focus`      | Move foco para widget | `set nome focus`                    |
| `stylesheet` | Aplica CSS Qt         | `set nome stylesheet "color: red;"` |
| `icon`       | Define ícone          | `set nome icon "edit-copy"`         |
| `iconsize`   | Tamanho do ícone      | `set nome iconsize 32`              |

---

## Próximos Passos

- [Guia de Layouts](layouts.md) - Criar interfaces complexas
- [Funcionalidades Avançadas](advanced-features.md) - Stylesheets, comunicação bidirecional

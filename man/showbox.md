# showbox(1) - Manual do Usuário

## NOME

**showbox** — Uma ferramenta para criar interfaces gráficas de usuário (GUI) a partir de scripts de shell.

## SINOPSE

`showbox` [**opções_globais**] [**comandos**]

## DESCRIÇÃO

O **showbox** é uma aplicação poderosa que interpreta um script de comandos, lido da entrada padrão, para construir e exibir uma janela de diálogo gráfica. Ele permite que desenvolvedores e administradores de sistema criem interfaces de usuário interativas para seus scripts de shell sem a necessidade de compilar código em C++ ou usar toolkits de GUI complexos diretamente.

A interface é definida por uma sequência de comandos que adicionam, configuram e gerenciam widgets (elementos de interface como botões, rótulos, tabelas, etc.) dentro de uma janela.

## COMANDOS PRINCIPAIS

Os comandos são lidos da entrada padrão, um por linha.

### **add** `<tipo_do_widget>` [`argumentos...`] [`opções...`]

Adiciona um novo widget à interface. A posição do widget é determinada pelo layout atual (geralmente vertical ou horizontal).

- **`<tipo_do_widget>`**: O tipo de widget a ser criado (veja a seção **WIDGETS** abaixo para uma lista completa).
- **`[argumentos...]`**: Parâmetros posicionais específicos do widget, como `título` e `nome`. O `nome` é um identificador único para referenciar o widget posteriormente com `set` ou `query`.
- **`[opções...]`**: Flags que modificam o comportamento ou a aparência do widget (ex: `checked`, `vertical`, `readonly`).

---

### **set** [`nome_do_widget`] `<opção>` [`valor`]

Modifica uma propriedade de um widget existente ou do diálogo principal.

- **`[nome_do_widget]`**: O nome do widget a ser modificado. Se omitido, a alteração se aplica ao contêiner pai atual (como o diálogo principal).
- **`<opção>`**: A propriedade a ser alterada (ex: `title`, `text`, `visible`, `enabled`).
- **`[valor]`**: O novo valor para a propriedade. Algumas opções não requerem valor (ex: `checked`, `readonly`).

---

### **query**

Instrui o `showbox` a imprimir o estado atual de todos os widgets interativos para a saída padrão (stdout). A saída é formatada como `nome_do_widget="valor"`. É útil para capturar a entrada do usuário em um script de shell.

---

### **Comandos Simples**

Estes comandos realizam ações diretas em widgets ou no layout.

- **show** `<nome_do_widget>`: Torna um widget visível.
- **hide** `<nome_do_widget>`: Oculta um widget.
- **enable** `<nome_do_widget>`: Habilita um widget para interação.
- **disable** `<nome_do_widget>`: Desabilita um widget.
- **clear** [`nome_do_widget`]: Limpa o conteúdo de um widget (ex: uma lista) ou de todo o diálogo se o nome for omitido.
- **remove** `<nome_do_widget>`: Remove um widget da interface.
- **end**: Finaliza o contexto de um widget contêiner (como `groupbox` ou `tabs`), similar a fechar uma tag em HTML. Essencial para construir layouts aninhados.
- **step** [`vertical`|`horizontal`]: Avança para a próxima posição em um layout, criando uma quebra de linha ou coluna.
- **space** [`tamanho`]: Adiciona um espaço fixo no layout.
- **stretch** [`fator`]: Adiciona um espaço flexível que ocupa o espaço disponível.

## WIDGETS (Tipos para o comando `add`)

Abaixo estão os tipos de widgets que podem ser criados com o comando `add`.

#### **De Ação**

- **pushbutton** `título` `nome` [`apply`] [`exit`] [`default`] [`checkable`] [`checked`]: Um botão padrão.
  - `apply`/`exit`: Define papéis especiais para o botão.
  - `checkable`: Permite que o botão seja "pressionado" e permaneça nesse estado.

#### **De Exibição**

- **label** `título` `nome` [`picture`|`animation`]: Um rótulo para texto ou imagens.
  - `picture`/`animation`: Indica que o `título` é o caminho para uma imagem/animação.
- **progressbar** `nome` [`vertical`] [`busy`]: Uma barra de progresso.
  - `busy`: Modo indeterminado (animação contínua).
- **textview** `nome` [`arquivo`]: Uma área para exibir texto (possivelmente de um arquivo), não editável.
- **chart** `título` `nome`: Um gráfico interativo para visualização de dados.
- **separator** [`nome`] [`vertical`] [`plain`|`raised`|`sunken`]: Uma linha separadora visual.

#### **De Entrada**

- **checkbox** `título` `nome` [`checked`]: Uma caixa de seleção individual.
- **radiobutton** `título` `nome` [`checked`]: Um botão de opção, geralmente usado dentro de um `groupbox` para seleção única.
- **listbox** `título` `nome` [`activation`] [`selection`]: Uma lista de itens. Os itens são adicionados com o comando `add item`.
- **combobox** / **dropdownlist** `título` `nome` [`selection`]: Uma lista de opções suspensa. Itens são adicionados com `add item`.
- **slider** `nome` [`min`] [`max`] [`vertical`]: Um controle deslizante para selecionar um valor numérico.
- **calendar** `título` `nome` [`opções...`]: Um widget para seleção de data.
  - Opções: `date "YYYY-MM-DD"`, `minimum "YYYY-MM-DD"`, `format "..."`.
- **table** `cabeçalhos` `nome` [`opções...`]: Uma tabela para dados estruturados.
  - `cabeçalhos`: String com nomes das colunas, separados por `;`.
  - Opções: `file "caminho.csv"`, `readonly`, `search`.

#### **De Layout (Contêineres)**

- **frame** `nome` [`vertical`|`horizontal`] [`estilo...`]: Um contêiner com uma moldura para agrupar outros widgets.
- **groupbox** `título` `nome` [`vertical`|`horizontal`] [`checkable`]: Similar a um `frame`, mas com um título. Pode ser `checkable` para habilitar/desabilitar todos os widgets filhos de uma vez.
- **tabs** `nome` [`top`|`bottom`|`left`|`right`]: Um contêiner que organiza widgets em abas.
- **page** `título` `nome` [`ícone`] [`current`]: Adiciona uma nova aba (página) a um contêiner `tabs`.

## EXEMPLOS

### Exemplo 1: Diálogo Simples de "Olá, Mundo"

Este script cria uma janela com um rótulo e um botão de "Sair".

```bash
#!/bin/bash

showbox << EOF
set title "Exemplo Simples"
add label "Olá, Mundo! Bem-vindo ao dishowbox
add pushbutton "Sair" btn_sair exit
EOF
```

### Exemplo 2: Formulário de Login

Este exemplo demonstra um formulário com campos de entrada, uma caixa de seleção e botões de ação.

```bash
#!/bin/bash

# Define a interface do showbox
read -r -d '' UI_SCRIPT << EOM
set title "Acesso ao Sistema"
set icon "./images/logo.png"

# Agrupa os campos de login
add groupbox "Credenciais" creds_group vertical
  add label "Usuário:" user_label
  add textbox "" user_input
  add label "Senha:" pass_label
  add textbox "" pass_input password # A opção 'password' oculta o texto
end

add checkbox "Lembrar-me" remember_cb checked

# Adiciona um espaço flexível para empurrar os botões para baixo
add stretch 1

# Layout horizontal para os botões
add frame "" btn_frame horizontal
  add stretch 1 # Espaço para alinhar botões à direita
  add pushbutton "Login" btn_login default apply
  add pushbutton "Cancelar" btn_cancel exit
end
EOM

# Executa o dishowbox captura a saída
OUTPUT=$(echo "$UI_SCRIPT" | dialshowbox Verifica se o usuário pressionou "Login"
if [ $? -eq 0 ]; then
  # Extrai os valores usando o comando 'query'
  eval "$OUTPUT"
  echo "Usuário: $user_input"
  echo "Senha: $pass_input"
  echo "Lembrar: $remember_cb"
fi
```

_Neste exemplo, assumimos que `add textbox` é um widget válido para entrada de texto._

## VALORES DE SAÍDA

- **0**: Sucesso (geralmente quando um botão com a opção `apply` é pressionado).
- **1**: Cancelado pelo usuário (geralmente quando um botão com a opção `exit` é pressionado, a janela é fechada ou `ESC` é pressionado).
- **Outros valores**: Indicam erros de parsing ou outros problemas.

## ARQUIVOS

- `~/.config/showbox/dialogrc`: Arquivo de configuração do usuário (se aplicável).

## AMBIENTE

- `showbox_RC`: Pode apontar para um arquivo de configuração alternativo.

## BUGS

Bugs podem ser reportados no sistema de issues do projeto.

## VEJA TAMBÉM

**zenity(1)**, **kdialog(1)**

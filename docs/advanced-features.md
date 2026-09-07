# Funcionalidades Avançadas

Recursos avançados para usuários experientes do Showbox.

---

## Comunicação Bidirecional

O Showbox suporta modificação dinâmica da interface em resposta a eventos do usuário.

### Usando Pipes (coproc)

````bash
#!/bin/bash

# Iniciar showbox como co-processo
coproc showbox --hidden
INPUTFD=${COPROC[0]}    # Leitura (saída do showbox)
OUTPUTFD=${COPROC[1]}   # Escrita (entrada do showbox)

# Construir interface
cat >&$OUTPUTFD << 'EOF'
set title "Exemplo Dinâmico"

add slider sld_valor minimum 0 maximum 100
add label "Valor: 50" lbl_valor

add separator
add pushbutton "Fechar" btn_close exit default

show
EOF

# Loop de eventos
while IFS='=' read -r widget valor; do
    case $widget in
        sld_valor)
            # Atualiza o label quando o slider muda
            echo "set lbl_valor text \"Valor: $valor\"" >&$OUTPUTFD
            ;;
        btn_close)
            break
            ;;
    esac
done <&$INPUTFD

wait $COPROC_PID
echo "Diálogo fechado"
```text

---

### Usando FIFOs (Named Pipes)

Para scripts mais complexos ou quando coproc não está disponível.

```bash
#!/bin/bash

FIFO_IN="./showbox_in"
FIFO_OUT="./showbox_out"

# Criar FIFOs
rm -f "$FIFO_IN" "$FIFO_OUT"
mkfifo "$FIFO_IN" "$FIFO_OUT"

# Iniciar showbox em background
showbox < "$FIFO_OUT" > "$FIFO_IN" &
SHOWBOX_PID=$!

# Cleanup ao sair
trap "kill $SHOWBOX_PID 2>/dev/null; rm -f '$FIFO_IN' '$FIFO_OUT'" EXIT

# Construir interface
cat > "$FIFO_OUT" << 'EOF'
set title "FIFO Demo"
add label "Aguardando..." lbl_status
add pushbutton "Processar" btn_process
add pushbutton "Fechar" btn_close exit
show
EOF

# Loop de eventos
while IFS='=' read -r widget valor; do
    case $widget in
        btn_process)
            echo 'set lbl_status text "Processando..."' > "$FIFO_OUT"
            sleep 2
            echo 'set lbl_status text "Concluído!"' > "$FIFO_OUT"
            ;;
    esac
done < "$FIFO_IN"
```text

---

## Stylesheets Qt

O Showbox suporta stylesheets CSS-like do Qt para customização visual completa.

### Sintaxe Básica

```bash
set widget_name stylesheet "propriedade: valor; propriedade2: valor2;"
```text

### Propriedades Comuns

| Propriedade        | Descrição           | Exemplo                             |
| ------------------ | ------------------- | ----------------------------------- |
| `color`            | Cor do texto        | `color: #333333;`                   |
| `background-color` | Cor de fundo        | `background-color: #f0f0f0;`        |
| `background`       | Fundo (gradiente)   | `background: qlineargradient(...);` |
| `border`           | Borda               | `border: 1px solid #ccc;`           |
| `border-radius`    | Borda arredondada   | `border-radius: 5px;`               |
| `padding`          | Espaçamento interno | `padding: 10px;`                    |
| `margin`           | Espaçamento externo | `margin: 5px;`                      |
| `font-family`      | Fonte               | `font-family: 'Ubuntu';`            |
| `font-size`        | Tamanho             | `font-size: 14px;`                  |
| `font-weight`      | Peso                | `font-weight: bold;`                |
| `min-width`        | Largura mínima      | `min-width: 200px;`                 |
| `min-height`       | Altura mínima       | `min-height: 50px;`                 |

### Exemplos de Estilização

#### Botão Moderno

```bash
add pushbutton "Enviar" btn_send
set btn_send stylesheet "
    background-color: #2196F3;
    color: white;
    border: none;
    border-radius: 4px;
    padding: 10px 20px;
    font-size: 14px;
"
```text

#### Botão com Hover (via diálogo)

```bash
set btn_send stylesheet "
    QPushButton {
        background-color: #2196F3;
        color: white;
        border-radius: 4px;
        padding: 10px 20px;
    }
    QPushButton:hover {
        background-color: #1976D2;
    }
    QPushButton:pressed {
        background-color: #0D47A1;
    }
"
```text

#### Campo de Texto Elegante

```bash
add textbox "Email:" txt_email
set txt_email stylesheet "
    QLineEdit {
        border: 2px solid #e0e0e0;
        border-radius: 4px;
        padding: 8px;
        background-color: #fafafa;
    }
    QLineEdit:focus {
        border-color: #2196F3;
        background-color: white;
    }
"
```text

#### Caixa de Seleção Personalizada

```bash
add checkbox "Aceito os termos" chk_terms
set chk_terms stylesheet "
    QCheckBox {
        spacing: 8px;
    }
    QCheckBox::indicator {
        width: 20px;
        height: 20px;
        border: 2px solid #666;
        border-radius: 3px;
    }
    QCheckBox::indicator:checked {
        background-color: #4CAF50;
        border-color: #4CAF50;
    }
"
```text

---

## Temas

### Tema Global do Sistema

O Showbox respeita o tema Qt do sistema. Para forçar:

```bash
# Via variável de ambiente
export QT_STYLE_OVERRIDE=Fusion
showbox < comandos.txt

# Via linha de comando
showbox --style fusion < comandos.txt
```text

### Tema Escuro via Stylesheet

```bash
set stylesheet "
    QDialog {
        background-color: #2d2d2d;
        color: #e0e0e0;
    }
    QLabel {
        color: #e0e0e0;
    }
    QPushButton {
        background-color: #3d3d3d;
        color: #e0e0e0;
        border: 1px solid #555;
        padding: 8px 16px;
        border-radius: 4px;
    }
    QPushButton:hover {
        background-color: #4d4d4d;
    }
    QLineEdit, QTextEdit {
        background-color: #3d3d3d;
        color: #e0e0e0;
        border: 1px solid #555;
        border-radius: 4px;
        padding: 6px;
    }
    QGroupBox {
        color: #e0e0e0;
        border: 1px solid #555;
        border-radius: 4px;
        margin-top: 10px;
        padding-top: 10px;
    }
    QGroupBox::title {
        color: #e0e0e0;
    }
"
```text

---

## Gráficos Interativos

### Gráfico de Pizza

```bash
#!/bin/bash

showbox << 'EOF'
set title "Vendas por Região"

add chart "Distribuição Regional" chart_vendas
set chart_vendas data "Norte:2500;Sul:3800;Leste:2100;Oeste:2900"

add separator
add pushbutton "Exportar" btn_export
add pushbutton "Fechar" btn_close exit default

show
EOF
```text

### Atualização Dinâmica

```bash
# Em loop de comunicação bidirecional:
echo 'set chart_vendas append "Centro-Oeste:1500"' >&$OUTPUTFD
```text

### Exportar Gráfico

```bash
set chart_vendas export "/tmp/grafico_vendas.png"
```text

---

## Tabelas com Dados CSV

### Carregar de Arquivo

```bash
add table "Nome;Email;Departamento" tbl_funcionarios file "funcionarios.csv" readonly search
```text

### Dados Dinâmicos

```bash
# Adicionar linha
set tbl_dados add_line "João;joao@email.com;TI"

# Remover linha selecionada
set tbl_dados del_line

# Recarregar dados de um CSV
set tbl_dados file "/tmp/dados-atualizados.csv"
```text

---

## Calendário Avançado

### Com Restrições de Data

```bash
add calendar "Data de Entrega:" cal_entrega selection navigation
set cal_entrega date "2026-01-20"          # Data inicial
set cal_entrega minimum "2026-01-15"       # Mínimo
set cal_entrega maximum "2026-02-28"       # Máximo
set cal_entrega format "dd 'de' MMMM 'de' yyyy"
```text

### Capturando Seleção

A opção `selection` faz o calendário reportar imediatamente:

```text
cal_entrega=2026-01-20
```text

---

## Processamento Assíncrono

### Barra de Progresso com Processo em Background

```bash
#!/bin/bash

coproc showbox --hidden
INPUTFD=${COPROC[0]}
OUTPUTFD=${COPROC[1]}

cat >&$OUTPUTFD << 'EOF'
set title "Processando..."
add label "Baixando arquivo..." lbl_status
add progressbar pb_download
add pushbutton "Cancelar" btn_cancel exit
show
EOF

# Simular download em background
(
    for i in {0..100..5}; do
        echo "set pb_download value $i" >&$OUTPUTFD
        sleep 0.2
    done
    echo 'set lbl_status text "Download concluído!"' >&$OUTPUTFD
    echo 'set btn_cancel title "Fechar"' >&$OUTPUTFD
) &
DOWNLOAD_PID=$!

# Loop de eventos
while IFS='=' read -r widget valor; do
    case $widget in
        btn_cancel)
            kill $DOWNLOAD_PID 2>/dev/null
            break
            ;;
    esac
done <&$INPUTFD

wait $COPROC_PID
```text

---

## Ícones do Sistema

O Showbox suporta ícones do tema do sistema por nome:

```bash
add pushbutton "Salvar" btn_save
set btn_save icon "document-save"       # Ícone pelo nome do tema
set btn_save iconsize 24

add pushbutton "Abrir" btn_open
set btn_open icon "/path/to/custom.svg" # Ícone personalizado
```text

### Nomes de Ícones Comuns

| Nome                 | Descrição      |
| -------------------- | -------------- |
| `document-new`       | Novo documento |
| `document-open`      | Abrir          |
| `document-save`      | Salvar         |
| `edit-copy`          | Copiar         |
| `edit-paste`         | Colar          |
| `edit-cut`           | Recortar       |
| `edit-undo`          | Desfazer       |
| `edit-redo`          | Refazer        |
| `dialog-ok`          | OK             |
| `dialog-cancel`      | Cancelar       |
| `dialog-information` | Informação     |
| `dialog-warning`     | Aviso          |
| `dialog-error`       | Erro           |
| `user-available`     | Usuário        |
| `folder`             | Pasta          |

---

## Próximos Passos

- [Boas Práticas](best-practices.md) - Organização e performance
- [Segurança](security.md) - Validação e proteção
- [Resolução de Problemas](troubleshooting.md) - FAQ e erros comuns
````

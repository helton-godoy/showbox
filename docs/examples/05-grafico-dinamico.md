# Tutorial 05: Gráfico Dinâmico

Criando gráficos interativos com atualização em tempo real.

**Nível:** 🔴 Avançado  
**Tempo estimado:** 20 minutos

---

## Objetivo

Criar um dashboard com:

- Gráfico de pizza interativo
- Atualização dinâmica de dados
- Exportação de imagem
- Detecção de clique em fatias

---

## Código Completo

```bash
#!/bin/bash
#
# Tutorial 05: Gráfico Dinâmico
# Demonstra chart widget com interatividade

FIFO_IN="/tmp/showbox_$$_in"
FIFO_OUT="/tmp/showbox_$$_out"

# Cleanup
cleanup() {
    kill $SHOWBOX_PID 2>/dev/null
    rm -f "$FIFO_IN" "$FIFO_OUT"
}
trap cleanup EXIT

# Criar FIFOs
mkfifo "$FIFO_IN" "$FIFO_OUT"

# Iniciar showbox
showbox --resizable < "$FIFO_OUT" > "$FIFO_IN" &
SHOWBOX_PID=$!

# Construir interface
cat > "$FIFO_OUT" << 'EOF'
set title "Dashboard de Vendas"

add label "<h2>📊 Vendas por Região</h2>" lbl_title

add chart "Distribuição Regional" chart_vendas
set chart_vendas data "Norte:2500;Sul:3800;Leste:2100;Oeste:2900;Centro:1800"
set chart_vendas stylesheet "min-height: 300px; min-width: 400px;"

add separator

add frame frm_controls horizontal

add groupbox "Adicionar Região" grp_add vertical
add textbox "Nome:" txt_regiao
add textbox "Valor:" txt_valor
add pushbutton "Adicionar" btn_add
end groupbox

add stretch

add groupbox "Ações" grp_actions vertical
add pushbutton "Atualizar Dados" btn_refresh
set btn_refresh icon "view-refresh"
add pushbutton "Exportar PNG" btn_export
set btn_export icon "document-save-as"
end groupbox

end frame

add separator

# Área de status
add label "Clique em uma fatia do gráfico para ver detalhes" lbl_status
set lbl_status stylesheet "background: #f0f0f0; padding: 8px; border-radius: 4px;"

add separator

add frame frm_footer horizontal
add stretch
add pushbutton "Fechar" btn_close exit default
end frame

show
EOF

# Função para atualizar gráfico
update_chart() {
    echo "set chart_vendas data \"Norte:$((RANDOM % 5000 + 1000));Sul:$((RANDOM % 5000 + 1000));Leste:$((RANDOM % 5000 + 1000));Oeste:$((RANDOM % 5000 + 1000));Centro:$((RANDOM % 5000 + 1000))\"" > "$FIFO_OUT"
    echo "set lbl_status text \"Dados atualizados em $(date '+%H:%M:%S')\"" > "$FIFO_OUT"
}

# Loop de eventos
while IFS='=' read -r widget valor; do
    case $widget in
        btn_add)
            # Ler valores dos campos (simulado - em produção use query)
            echo "set chart_vendas append \"NovaRegiao:1500\"" > "$FIFO_OUT"
            echo "set lbl_status text \"Região adicionada!\"" > "$FIFO_OUT"
            ;;
        btn_refresh)
            update_chart
            ;;
        btn_export)
            EXPORT_FILE="/tmp/grafico_vendas_$(date +%Y%m%d_%H%M%S).png"
            echo "set chart_vendas export \"$EXPORT_FILE\"" > "$FIFO_OUT"
            echo "set lbl_status text \"Exportado: $EXPORT_FILE\"" > "$FIFO_OUT"
            ;;
        chart_vendas.slice*)
            # Clique em fatia: chart_vendas.slice["Norte"]=2500
            REGIAO=$(echo "$widget" | sed 's/.*\["\(.*\)"\].*/\1/')
            echo "set lbl_status text \"Região selecionada: $REGIAO - Valor: $valor\"" > "$FIFO_OUT"
            ;;
        btn_close)
            break
            ;;
    esac
done < "$FIFO_IN"

echo "Dashboard encerrado"
```

---

## Conceitos-Chave

### Criando o Gráfico

```bash
add chart "Título" chart_nome
set chart_nome data "Label1:Valor1;Label2:Valor2;..."
```

### Adicionando Dados Dinamicamente

```bash
set chart_nome append "NovaLabel:1500"
```

### Atualizando Todos os Dados

```bash
set chart_nome data "Norte:3000;Sul:4000;..."
```

### Exportando como Imagem

```bash
set chart_nome export "/caminho/grafico.png"
```

### Detectando Clique em Fatia

Evento emitido:

```text
chart_vendas.slice["Norte"]=2500
```

Processamento:

```bash
REGIAO=$(echo "$widget" | sed 's/.*\["\(.*\)"\].*/\1/')
```

---

## Tipos de Gráfico

O widget `chart` suporta automaticamente gráficos de pizza. Dados são interpretados como pares `label:valor`.

Para gráficos de barras ou linha, configure o eixo:

```bash
set chart_nome axis "horizontal"  # Barras
set chart_nome axis "vertical"    # Linha/Área
```

---

## Diagrama de Arquitetura

```text
┌─────────────┐     FIFO_OUT      ┌─────────────┐
│   Script    │ ─────────────────▶│   Showbox   │
│   (bash)    │                   │   (Qt GUI)  │
│             │◀───────────────── │             │
└─────────────┘     FIFO_IN       └─────────────┘
      │                                  │
      │ Comandos:                        │ Eventos:
      │ set chart data...                │ chart.slice["X"]=Y
      │ set chart append...              │ btn_refresh=clicked
      │ set chart export...              │
      ▼                                  ▼
```

---

## Parabéns

Você completou todos os tutoriais básicos e intermediários!

### Próximos Passos

- Explore a [Referência de Widgets](../widgets-reference.md) completa
- Aprenda mais sobre [Stylesheets](../advanced-features.md#stylesheets-qt)
- Consulte [Boas Práticas](../best-practices.md) para projetos maiores

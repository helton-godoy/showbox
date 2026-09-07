# Tutorial 04: Tabela de Dados

Trabalhando com tabelas editáveis e busca.

**Nível:** 🟡 Intermediário  
**Tempo estimado:** 15 minutos

---

## Objetivo

Criar uma interface de gerenciamento de dados com:

- Tabela editável
- Campo de busca
- Importação/exportação CSV
- Ações de linha (adicionar/remover)

---

## Código Completo

```bash
#!/bin/bash
#
# Tutorial 04: Tabela de Dados
# Demonstra table widget com busca e edição

coproc showbox --resizable --hidden
INPUTFD=${COPROC[0]}
OUTPUTFD=${COPROC[1]}

# Criar interface
cat >&$OUTPUTFD << 'EOF'
set title "Gerenciador de Contatos"

add label "<h2>📇 Meus Contatos</h2>"

add table "Nome;Email;Telefone;Cidade" tbl_contatos search selection
set tbl_contatos stylesheet "min-height: 300px; min-width: 600px;"

# Adicionar dados iniciais
set tbl_contatos add_line "João Silva;joao@email.com;(11) 99999-1234;São Paulo"
set tbl_contatos add_line "Maria Santos;maria@email.com;(21) 98888-5678;Rio de Janeiro"
set tbl_contatos add_line "Pedro Costa;pedro@email.com;(31) 97777-9012;Belo Horizonte"

add separator

add frame frm_actions horizontal

add pushbutton "Adicionar" btn_add
set btn_add icon "list-add"

add pushbutton "Remover" btn_del
set btn_del icon "list-remove"

add stretch

add pushbutton "Importar CSV" btn_import
set btn_import icon "document-open"

add pushbutton "Exportar CSV" btn_export
set btn_export icon "document-save"

end frame

add separator

add frame frm_buttons horizontal
add stretch
add pushbutton "Fechar" btn_close exit default
end frame

show
EOF

# Loop de eventos
while IFS='=' read -r widget valor; do
    case $widget in
        btn_add)
            # Adicionar nova linha
            echo 'set tbl_contatos add_line "Novo Contato;email@exemplo.com;(00) 00000-0000;Cidade"' >&$OUTPUTFD
            ;;
        btn_del)
            # Remover linha selecionada
            echo 'set tbl_contatos del_line' >&$OUTPUTFD
            ;;
        btn_import)
            echo "Importar CSV - implementar diálogo de arquivo"
            ;;
        btn_export)
            echo 'set tbl_contatos file "/tmp/contatos_export.csv"' >&$OUTPUTFD
            echo "Exportado para /tmp/contatos_export.csv"
            ;;
        tbl_contatos*)
            # Célula editada: tbl_contatos[linha][coluna]=valor
            echo "Célula editada: $widget = $valor"
            ;;
        btn_close)
            break
            ;;
    esac
done <&$INPUTFD

wait $COPROC_PID
echo "Aplicação encerrada"
```

---

## Conceitos-Chave

### Criando a Tabela

```bash
add table "Nome;Email;Telefone;Cidade" tbl_contatos search selection
```

- Headers separados por `;`
- `search` - Adiciona campo de busca
- `selection` - Reporta linha selecionada

### Adicionando Dados

```bash
set tbl_contatos add_line "João;joao@email.com;(11) 1234;SP"
```

Valores das colunas separados por `;`.

### Removendo Linha Selecionada

```bash
set tbl_contatos del_line
```

Remove a linha atualmente selecionada.

### Exportando para CSV

```bash
set tbl_contatos file "/caminho/arquivo.csv"
```

Salva todo o conteúdo da tabela em formato CSV.

### Eventos de Edição

Quando o usuário edita uma célula:

```
tbl_contatos[2][1]=novo_email@email.com
```

Formato: `nome[linha][coluna]=valor`

---

## Importando CSV Existente

Para carregar dados de um arquivo na inicialização:

```bash
add table "Col1;Col2;Col3" tbl_dados file "/caminho/dados.csv" readonly
```

---

## Próximo Tutorial

[05. Gráfico Dinâmico →](05-grafico-dinamico.md)

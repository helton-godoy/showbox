# Resolução de Problemas

Soluções para problemas comuns ao usar o Showbox.

---

## Problemas de Instalação

### Erro: "showbox: command not found"

**Causa:** Binário não está no PATH.

**Solução:**

```bash
# Verificar onde está instalado
which showbox || find /usr -name "showbox" 2>/dev/null

# Adicionar ao PATH temporariamente
export PATH="$PATH:/caminho/para/showbox"

# Adicionar permanentemente
echo 'export PATH="$PATH:/caminho/para/showbox"' >> ~/.bashrc
source ~/.bashrc
```

---

### Erro: "error while loading shared libraries: libQt6..."

**Causa:** Dependências Qt6 não instaladas.

**Solução Debian/Ubuntu:**

```bash
sudo apt-get install qt6-base-dev qt6-charts-dev
```

**Solução com AppImage:** Use o AppImage que inclui todas as dependências.

---

## Problemas de Layout

### Widgets empilhados em vez de lado a lado

**Causa:** Uso incorreto de `frame horizontal` no nível raiz.

**Solução:** Use `step horizontal` para criar colunas:

```bash
# ❌ ERRADO - Widgets empilhados
add frame col1 horizontal
add label "Coluna 1"
end frame
add frame col2 horizontal
add label "Coluna 2"
end frame

# ✅ CORRETO - Widgets lado a lado
add frame col1 vertical
add label "Coluna 1"
end frame
step horizontal
add frame col2 vertical
add label "Coluna 2"
end frame
```

---

### Widgets não aparecem

**Causas possíveis:**

1. **Falta `show`:**

   ```bash
   # Adicione no final do script
   show
   ```

2. **Widget dentro de container fechado:**

   ```bash
   add groupbox "Grupo" grp
   add label "Dentro"
   end groupbox
   add label "Este está FORA do grupo"  # OK
   ```

3. **Widget oculto:**

   ```bash
   # Verificar se não há hide
   show nome_widget
   ```

---

### Espaçamento estranho

**Causa:** `stretch` consumindo todo o espaço.

**Solução:** Remova ou reposicione o `stretch`:

```bash
# Muito espaço entre widgets
add label "A"
add stretch    # Remove esta linha se indesejado
add label "B"
```

---

## Problemas de Sintaxe

### Erro: Aspas não fechadas

**Sintoma:** Comando não executa ou comportamento estranho.

**Causa:** Aspas não balanceadas.

```bash
# ❌ ERRADO
add label "Texto com "aspas" internas"

# ✅ CORRETO - Escape com backslash
add label "Texto com \"aspas\" internas"

# ✅ CORRETO - Aspas simples externas
add label 'Texto com "aspas" internas'
```

---

### Erro: Caracteres especiais

**Problema:** `&`, `$`, `!` interpretados pelo shell.

**Solução:** Use heredoc com aspas:

```bash
showbox << 'EOF'   # Note as ASPAS em EOF
add label "Preço: $100 & mais!"
EOF
```

---

### Erro: Espaços em nomes de arquivo

```bash
# ❌ ERRADO
set lbl_img picture /path/to/my image.png

# ✅ CORRETO
set lbl_img picture "/path/to/my image.png"
```

---

## Problemas de Comunicação

### Eventos não recebidos

**Causa:** Widget sem `activation` ou `selection`.

```bash
# Listbox não reporta seleção
add listbox "Lista:" lst
add item "Item 1"
end listbox

# Adicione 'selection' para reportar
add listbox "Lista:" lst selection
add item "Item 1"
end listbox
```

---

### Pipe fechado prematuramente

**Sintoma:** "Broken pipe" ou showbox fecha sozinho.

**Causa:** Script termina antes de ler toda a entrada.

**Solução:** Use loop de leitura adequado:

```bash
while IFS='=' read -r widget valor; do
    # processar
done < <(showbox << 'EOF'
...
EOF
)
```

---

### Comandos não executam via pipe

**Causa:** Buffer não enviado imediatamente.

**Solução:** Force flush:

```bash
# Após enviar comando
echo "set lbl text 'Novo texto'" >&$OUTPUTFD

# Em alguns casos, adicione sleep mínimo
sleep 0.01
```

---

## Problemas Visuais

### Ícones não aparecem

**Causas:**

1. Nome de ícone incorreto
2. Tema de ícones não instalado

**Solução:**

```bash
# Verificar ícones disponíveis
ls /usr/share/icons/*/

# Usar caminho absoluto como fallback
set btn_save icon "/usr/share/icons/Adwaita/48x48/actions/document-save.png"
```

---

### Fonte muito pequena/grande

**Solução via stylesheet:**

```bash
set stylesheet "* { font-size: 14px; }"
```

**Solução via variável de ambiente (HiDPI):**

```bash
export QT_SCALE_FACTOR=1.5
showbox < comandos.txt
```

---

### Cores estranhas

**Causa:** Conflito com tema do sistema.

**Solução:** Forçar estilo neutro:

```bash
export QT_STYLE_OVERRIDE=Fusion
showbox < comandos.txt
```

---

## FAQ

### Como fechar o diálogo programaticamente?

Não há comando direto. Alternativas:

1. Simule clique em botão exit
2. Termine o processo: `kill $SHOWBOX_PID`

---

### Como verificar qual botão foi clicado?

```bash
OUTPUT=$(showbox << 'EOF'
add pushbutton "OK" btn_ok apply exit
add pushbutton "Cancelar" btn_cancel exit
EOF
)

case $? in
    1) echo "OK clicado" ;;
    0) echo "Cancelado" ;;
esac
```

---

### Como limpar um campo de texto?

```bash
set txt_campo text ""
```

---

### Como desabilitar um botão?

```bash
disable btn_nome
# Ou
set btn_nome enabled 0
```

---

### Como obter os valores sem fechar?

Use `query`:

```bash
echo "query" >&$OUTPUTFD
# Leia os valores do INPUTFD
```

---

### Posso usar imagens SVG?

Sim, Qt6 suporta SVG nativamente:

```bash
add label "/path/to/image.svg" lbl_logo picture
```

---

## Debug

### Modo Verbose

Não há modo verbose nativo. Para debug:

1. **Teste comandos interativamente:**

   ```bash
   showbox
   # Digite comandos manualmente e veja o resultado
   add label "teste"
   show
   ```

2. **Log de comandos:**

   ```bash
   tee /tmp/showbox_debug.log | showbox
   ```

3. **Verifique saída:**

   ```bash
   showbox < comandos.txt 2>&1 | tee /tmp/output.log
   ```

---

## Próximos Passos

- [Boas Práticas](best-practices.md)
- [Segurança](security.md)

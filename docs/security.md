# Segurança

Práticas de segurança para scripts Showbox.

---

## Validação de Entrada

### Nunca Confie na Entrada do Usuário

Todo dado vindo do usuário deve ser validado antes de usar.

```bash
# ❌ PERIGOSO - Executa entrada diretamente
OUTPUT=$(showbox << 'EOF'
add textbox "Comando:" txt_cmd
add pushbutton "Executar" btn_run apply exit
EOF
)
eval "$OUTPUT"
$txt_cmd   # NUNCA faça isso!

# ✅ SEGURO - Valide e sanitize
eval "$OUTPUT"
case "$txt_cmd" in
    "listar") ls -la ;;
    "status") systemctl status ;;
    *) echo "Comando não permitido" ;;
esac
```

---

### Sanitização de Strings

```bash
sanitize_input() {
    local input="$1"
    # Remove caracteres perigosos
    echo "$input" | tr -d '`$(){}[]|;&<>\\' | head -c 100
}

OUTPUT=$(showbox << 'EOF'
add textbox "Nome do arquivo:" txt_filename
add pushbutton "Criar" btn_create apply exit
EOF
)

if [ $? -eq 1 ]; then
    eval "$OUTPUT"
    SAFE_NAME=$(sanitize_input "$txt_filename")

    # Validação adicional
    if [[ ! "$SAFE_NAME" =~ ^[a-zA-Z0-9._-]+$ ]]; then
        echo "Nome de arquivo inválido"
        exit 1
    fi

    touch "/tmp/$SAFE_NAME"
fi
```

---

### Validação de Tipos

```bash
# Validar número
is_number() {
    [[ "$1" =~ ^[0-9]+$ ]]
}

# Validar email
is_email() {
    [[ "$1" =~ ^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Z|a-z]{2,}$ ]]
}

# Validar data
is_date() {
    date -d "$1" >/dev/null 2>&1
}

# Uso
eval "$OUTPUT"

if ! is_number "$txt_idade"; then
    echo "Idade deve ser um número"
    exit 1
fi

if ! is_email "$txt_email"; then
    echo "Email inválido"
    exit 1
fi
```

---

## Proteção Contra Injection

### Command Injection

```bash
# ❌ VULNERÁVEL
echo "Pesquisando: $txt_busca"
grep "$txt_busca" arquivo.txt  # Usuário pode injetar comandos!

# ✅ SEGURO - Use -- para separar opções
grep -- "$txt_busca" arquivo.txt

# ✅ MAIS SEGURO - Use regex literals
grep -F -- "$txt_busca" arquivo.txt
```

---

### Path Traversal

```bash
# ❌ VULNERÁVEL
cat "/dados/$txt_arquivo"  # Usuário pode usar ../../../etc/passwd

# ✅ SEGURO
ARQUIVO=$(basename "$txt_arquivo")  # Remove diretórios
if [[ -f "/dados/$ARQUIVO" ]]; then
    cat "/dados/$ARQUIVO"
fi
```

---

### SQL Injection (se aplicável)

```bash
# ❌ VULNERÁVEL
sqlite3 db.sqlite "SELECT * FROM users WHERE name='$txt_nome'"

# ✅ SEGURO - Use parâmetros ou escape
SAFE_NOME=$(echo "$txt_nome" | sed "s/'/''/g")
sqlite3 db.sqlite "SELECT * FROM users WHERE name='$SAFE_NOME'"
```

---

## Gerenciamento de Senhas

### Campo de Senha

```bash
add textbox "Senha:" txt_senha password
```

### Nunca Armazene Senhas em Texto Plano

```bash
# ❌ ERRADO
echo "$txt_senha" > /tmp/senha.txt

# ✅ Armazene hash
HASH=$(echo -n "$txt_senha" | sha256sum | cut -d' ' -f1)
echo "$HASH" > ~/.config/myapp/credentials.hash
```

### Limpe Variáveis Sensíveis

```bash
eval "$OUTPUT"

# Processar senha
verificar_senha "$txt_senha"
resultado=$?

# Limpar da memória
unset txt_senha
```

---

## Permissões de Arquivos

### Scripts Executáveis

```bash
# Defina permissões corretas
chmod 755 meu_script.sh    # rwxr-xr-x

# Arquivos de configuração
chmod 600 ~/.config/myapp/config  # rw-------
```

### Arquivos Temporários

```bash
# ❌ INSEGURO
TMPFILE="/tmp/myapp_data.txt"

# ✅ SEGURO - Use mktemp
TMPFILE=$(mktemp)
trap "rm -f '$TMPFILE'" EXIT

# Ou diretório privado
TMPDIR=$(mktemp -d)
trap "rm -rf '$TMPDIR'" EXIT
```

---

## Execução Segura

### Ambientes Restritos

Para scripts de instalação ou administrativos:

```bash
# Verificar se está executando como root
if [ "$EUID" -eq 0 ]; then
    echo "Não execute como root!"
    exit 1
fi

# Ou exigir root
if [ "$EUID" -ne 0 ]; then
    echo "Este script requer privilégios de root"
    exit 1
fi
```

### Timeout para Processos

```bash
# Evite travamentos infinitos
timeout 60 processo_demorado || {
    echo "Processo excedeu tempo limite"
    exit 1
}
```

---

## Logs e Auditoria

### Log de Ações

```bash
LOG_FILE="/var/log/myapp.log"

log_action() {
    echo "$(date '+%Y-%m-%d %H:%M:%S') - $USER - $1" >> "$LOG_FILE"
}

# Uso
log_action "Usuário iniciou o aplicativo"
log_action "Configuração alterada: tema=$radio_tema"
```

### Não Logue Dados Sensíveis

```bash
# ❌ ERRADO
log_action "Login attempt: user=$txt_user, pass=$txt_pass"

# ✅ CORRETO
log_action "Login attempt: user=$txt_user"
```

---

## Considerações para Scripts de Instalação

Se usar Showbox para instaladores:

### 1. Verificar Integridade de Downloads

```bash
# Verificar checksum
wget https://example.com/file.tar.gz
wget https://example.com/file.tar.gz.sha256

if ! sha256sum -c file.tar.gz.sha256; then
    error_dialog "Arquivo corrompido ou adulterado!"
    exit 1
fi
```

### 2. Usar HTTPS

```bash
# ❌ INSEGURO
wget http://example.com/installer.sh

# ✅ SEGURO
wget https://example.com/installer.sh
```

### 3. Pedir Confirmação para Ações Críticas

```bash
# Antes de operações destrutivas
showbox << 'EOF'
set title "⚠️ ATENÇÃO"
add label "<b>Esta ação irá formatar o disco!</b>"
add label "Todos os dados serão perdidos."
add frame frm horizontal
add stretch
add pushbutton "Cancelar" btn_cancel exit default
add pushbutton "Continuar" btn_continue apply exit
end frame
show
EOF

if [ $? -ne 1 ]; then
    echo "Operação cancelada"
    exit 0
fi
```

---

## Checklist de Segurança

Antes de distribuir seu script:

- [ ] Valido todas as entradas do usuário
- [ ] Sanitizo strings antes de usar em comandos
- [ ] Uso `password` para campos sensíveis
- [ ] Limpo variáveis sensíveis após uso
- [ ] Uso arquivos temporários seguros (mktemp)
- [ ] Defino permissões corretas em arquivos
- [ ] Verifico checksums de downloads
- [ ] Peço confirmação para ações destrutivas
- [ ] Não logo dados sensíveis
- [ ] Uso HTTPS para comunicações de rede

---

## Recursos Adicionais

- [OWASP Shell Injection](https://owasp.org/www-community/attacks/Command_Injection)
- [Bash Security Best Practices](https://mywiki.wooledge.org/BashGuide/Practices)
- [CWE-78: OS Command Injection](https://cwe.mitre.org/data/definitions/78.html)

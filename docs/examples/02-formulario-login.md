# Tutorial 02: Formulário de Login

Criando um formulário com campos de entrada e validação.

**Nível:** 🟢 Básico  
**Tempo estimado:** 10 minutos

---

## Objetivo

Criar um formulário de login com:

- Campo de usuário
- Campo de senha (oculto)
- Checkbox "Lembrar-me"
- Botões de ação

---

## Código Completo

```bash
#!/bin/bash
#
# Tutorial 02: Formulário de Login
# Demonstra campos de entrada e processamento de dados

# Capturar a saída do showbox
OUTPUT=$(showbox << 'EOF'
set title "Login"

# Ícone do sistema (opcional)
set icon "system-lock-screen"

# Agrupando os campos
add groupbox "Credenciais" grp_creds vertical

add textbox "Usuário:" txt_user
set txt_user placeholder "Digite seu email"

add textbox "Senha:" txt_pass password
set txt_pass placeholder "Digite sua senha"

end groupbox

# Checkbox fora do grupo
add checkbox "Lembrar-me neste computador" chk_remember

# Linha de botões
add separator

add frame frm_buttons horizontal
add stretch
add pushbutton "Cancelar" btn_cancel exit
add pushbutton "Entrar" btn_login apply exit default
end frame

show
EOF
)

# Verificar resultado
case $? in
    1)
        # Usuário clicou em "Entrar"
        eval "$OUTPUT"

        echo "==== Dados Recebidos ===="
        echo "Usuário: $txt_user"
        echo "Senha: (${#txt_pass} caracteres)"
        echo "Lembrar: $( [ "$chk_remember" = "1" ] && echo "Sim" || echo "Não" )"

        # Validação simples
        if [ -z "$txt_user" ] || [ -z "$txt_pass" ]; then
            echo "Erro: Campos obrigatórios não preenchidos"
            exit 1
        fi

        echo "Login processado com sucesso!"
        ;;
    *)
        echo "Login cancelado pelo usuário"
        ;;
esac
```

---

## Conceitos Abordados

### Campo de Senha

```bash
add textbox "Senha:" txt_pass password
```

A opção `password` oculta os caracteres digitados.

### Placeholder

```bash
set txt_user placeholder "Digite seu email"
```

Texto de dica exibido quando o campo está vazio.

### Groupbox

```bash
add groupbox "Credenciais" grp_creds vertical
...
end groupbox
```

Agrupa widgets visualmente com uma borda e título.

### Captura de Dados

```bash
OUTPUT=$(showbox << 'EOF' ... EOF)
eval "$OUTPUT"
```

- Captura a saída do showbox na variável `OUTPUT`
- `eval` transforma as linhas `nome=valor` em variáveis shell

### Código de Saída

```bash
case $? in
    1) # OK/Apply clicado
    *) # Cancelado
esac
```

- `$? = 1` → Botão com `apply` e `exit` foi clicado
- `$? = 0` → Diálogo fechado/cancelado

---

## Melhorias Sugeridas

### Adicionar validação de email

```bash
if [[ ! "$txt_user" =~ ^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Z|a-z]{2,}$ ]]; then
    echo "Email inválido!"
    exit 1
fi
```

### Adicionar ícone ao botão

```bash
set btn_login icon "system-log-out"
```

---

## Próximo Tutorial

[03. Painel de Controle →](03-painel-controle.md)

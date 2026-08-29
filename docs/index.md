# Documentação do Showbox

> **Showbox** é uma ferramenta poderosa para criar interfaces gráficas (GUI) a partir de shell scripts usando Qt6.

---

## 🚀 Início Rápido

```bash
# Seu primeiro diálogo em uma linha!
echo 'add label "Olá, Showbox!" | add pushbutton "Fechar" exit' | showbox
```

---

## 📚 Índice

### Instalação e Configuração

- [Guia de Instalação](installation.md) - DEB, AppImage, compilação

### Uso Básico

- [Início Rápido](getting-started.md) - Primeiros passos com exemplos progressivos

### Referência

- [Referência de Widgets](widgets-reference.md) - Todos os 20 widgets detalhados
- [Guia de Layouts](layouts.md) - Layouts complexos, sidebar + tabs
- [Referência Técnica](dialogbox-reference.md) - Tokens, enums, arquitetura

### Avançado

- [Funcionalidades Avançadas](advanced-features.md) - Pipes, stylesheets, gráficos
- [Boas Práticas](best-practices.md) - Otimização e organização
- [Segurança](security.md) - Validação, proteção, permissões

### Suporte

- [Resolução de Problemas](troubleshooting.md) - Erros comuns e FAQ

### Exemplos Práticos

- [Exemplos Ilustrados](examples/) - 5 tutoriais passo a passo

---

## ✨ Por que Showbox?

| Característica               | Showbox        | zenity      | kdialog        |
| ---------------------------- | -------------- | ----------- | -------------- |
| **Widgets Avançados**        | ✅ 22 tipos    | ⚠️ Limitado | ⚠️ Limitado    |
| **Layouts Flexíveis**        | ✅ 3 níveis    | ❌          | ❌             |
| **Abas e Páginas**           | ✅             | ❌          | ❌             |
| **Tabelas Editáveis**        | ✅             | ❌          | ❌             |
| **Gráficos**                 | ✅             | ❌          | ❌             |
| **Stylesheets CSS**          | ✅             | ⚠️ GTK      | ⚠️ Qt limitado |
| **Comunicação Bidirecional** | ✅ Pipes/FIFOs | ⚠️ Limitado | ⚠️ Limitado    |

---

## 🏗️ Arquitetura

```
┌─────────────┐     stdin      ┌─────────────┐     Qt Widgets     ┌─────────────┐
│ Shell Script│ ─────────────▶ │   Showbox   │ ─────────────────▶ │     GUI     │
└─────────────┘                └─────────────┘                    └─────────────┘
                                     │
                                     │ stdout (eventos)
                                     ▼
                               ┌─────────────┐
                               │   Script    │
                               │  (resposta) │
                               └─────────────┘
```

---

## 🔗 Links Úteis

- **Código-fonte**: [GitHub](https://github.com/helton-godoy/showbox)
- **Exemplos**: [examples/](../examples/)
- **Man page**: `man showbox`

---

## 📖 Licença

Showbox é software livre distribuído sob a [GNU GPL v3](../COPYING).

---

_Documentação versão 1.0 | Atualizada em Janeiro 2026_

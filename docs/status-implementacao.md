# Status da Implementação - Showbox

> Relatório detalhado do status de implementação do Showbox comparado com a referência do Dialogbox.

**Data:** 15/01/2026  
**Versão:** 1.1 (em refatoração)

---

## Resumo Executivo

| Categoria          | Status        | Progresso       |
| ------------------ | ------------- | --------------- |
| **Parser V1**      | 🟡 Quarentena | 100% (Legado)   |
| **Parser V2**      | 🔴 Deprecado  | Incompleto      |
| **Target Arch**    | 🟢 Iniciado   | 10%             |
| **Widgets Padrão** | 🟡 Migração   | 5% (PushButton) |
| **Testes**         | 🟢 Iniciado   | 10%             |

**Legenda:** 🟢 Completo/Ativo | 🟡 Parcial/Legado | 🔴 Deprecado/Ausente

---

## 1. Arquitetura

### Legacy V1 (Dialogbox Monolith)

- **Status:** 🟡 Mantido para referência em `src/code/showbox/legacy/v1_monolith`
- **Build:** Target `showbox-legacy` (QMake)

### Legacy V2 (Tentativa anterior)

- **Status:** 🔴 Deprecado em `src/code/showbox/legacy/v2_incomplete`
- **Nota:** Código incompleto, não deve ser usado como referência.

### Target Architecture (Golden Sample)

- **Status:** 🟢 Ativo em `src/code/showbox/core` e `libs/showbox-ui`
- **Padrões:**
  - **Builder Pattern:** `CLIBuilder` (implementado)
  - **Passive View:** `PushButtonWidget` (implementado)
  - **Modular Parser:** `ParserMain` (implementado)

---

## 2. Widgets (Migração para Lib)

| Widget       | Passive View | Builder | Testes | Status   |
| ------------ | ------------ | ------- | ------ | -------- |
| `pushbutton` | ✓            | ✓       | ✓      | 🟢       |
| `label`      | 🔴           | 🔴      | 🔴     | Pendente |
| ...          | ...          | ...     | ...    | Pendente |

---

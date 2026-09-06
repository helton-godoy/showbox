# Guia de Operação: Swarm Toolbelt

Este guia fornece a referência completa para a utilização dos agentes autônomos localizados em `scripts/swarm/`.

## 📋 Pré-requisitos

Antes de iniciar, garanta que o contexto do projeto está inicializado:

```bash
./scripts/setup_context.sh
```

## 🚀 Sequência Mínima (Happy Path)

Para desenvolver uma nova feature com segurança e qualidade:

1. **Planejar (Architect):**

   ```bash
   ./scripts/swarm/architect.sh "Implementar sistema de Login" --level 2
   ```

2. **Codificar (Coder):**

   ```bash
   ./scripts/swarm/coder.sh "Criar classe AuthManager seguindo o plano" src/auth/
   ```

3. **Testar (QA):**

   ```bash
   ./scripts/swarm/qa.sh "AuthManager"
   ```

---

## 🛠️ Referência de Comandos

| Agente         | Script          | Função                    | Parâmetros Chave               |
| :------------- | :-------------- | :------------------------ | :----------------------------- |
| **Architect**  | `architect.sh`  | Planejamento e Design     | `<descrição> [--level 0-4]`    |
| **Coder**      | `coder.sh`      | Implementação             | `<tarefa> [arquivos_contexto]` |
| **QA**         | `qa.sh`         | Testes Unitários          | `<nome_feature>`               |
| **Research**   | `research.sh`   | Pesquisa (Web/Docs)       | `<query>`                      |
| **Integrator** | `integrator.sh` | Git/GitHub/PRs            | `pr <N>` ou `review`           |
| **Autonomous** | `autonomous.sh` | Execução Longa (One-Shot) | `<tarefa_complexa>`            |
| **Manager**    | `manager.sh`    | Status do Projeto         | `<mensagem>`                   |

### Níveis do Architect (BMAD)

- **Level 0 (Atomic):** Correções rápidas, sem design.
- **Level 1 (Basic):** Features simples.
- **Level 2 (Standard):** Padrão (Arquitetura + Testes).
- **Level 3 (Detailed):** Design de sistemas complexos.
- **Level 4 (Strategic):** Decisões de alto nível e risco.

---

## 📊 Workflows (Mermaid)

### 1. Fluxo Padrão (Standard Development)

O ciclo mais seguro e recomendado para features novas.

```mermaid
graph TD
    Start([Início]) --> Context{"Contexto existe?"}
    Context -- Não --> Setup[setup_context.sh]
    Context -- Sim --> Plan
    Setup --> Plan

    subgraph Planning
        Plan["Architect Agent"] -->|Level 2| Spec["Especificação Técnica"]
    end

    subgraph Execution
        Spec --> Code["Coder Agent"]
        Code --> Source["Código Fonte"]
    end

    subgraph Verification
        Source --> QA["QA Agent"]
        QA --> Tests["Testes Unitários"]
        Tests --> Valid{"Passou nos Testes?"}
    end

    Valid -- Não --> Refine["Coder Agent (Refactor)"]
    Refine --> Tests
    Valid -- Sim --> PR["Integrator Agent"]
    PR --> End([Fim])
```

### 2. Fluxo Rápido (Quick Fix / YOLO)

Para correções pequenas (typos, bugs óbvios).

```mermaid
graph LR
    Start([Bug Report]) --> Architect[Architect --level 0]
    Architect --> Coder[Coder Agent]
    Coder --> Commit[Git Commit]
```

### 3. Fluxo Autônomo (One-Shot)

Para tarefas complexas onde você confia na IA para resolver múltiplos passos sozinha (ex: Refatoração em massa).

```mermaid
graph TD
    Start(["Tarefa Complexa"]) --> Auto["Autonomous Agent (Cline)"]

    subgraph "Autonomous Loop"
        Auto --> Think[Análise]
        Think --> Action[Edição de Arquivos]
        Action --> Check[Build/Test]
        Check -- Fail --> Fix[Auto-Correção]
        Check -- Pass --> Done
        Fix --> Check
    end

    Done --> Review[Human Review]
```

### 4. Fluxo Estratégico (Strategic)

Para grandes mudanças arquiteturais que exigem pesquisa prévia.

```mermaid
graph TD
    Start([Ideia Vaga]) --> Research[Research Agent]
    Research --> Insights[Relatório]
    Insights --> Architect[Architect --level 4]
    Architect --> Plan[Plano Estratégico]
    Plan --> Manager["Manager Agent (Conductor)"]
    Manager --> Tasks[Lista de Tarefas]
    Tasks --> Delegate([Delegar para Coder/QA])
```

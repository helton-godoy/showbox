# Workflow Híbrido: "Executive & Swarm"

Este documento define o padrão operacional para desenvolvimento de software no Showbox Studio, utilizando uma estratégia híbrida que combina a inteligência de alto nível do Agente Antigravity com a capacidade de execução rápida de agentes CLI especializados.

## Papéis e Responsabilidades

### 1. The Executive (Antigravity / Líder Técnico)

- **Foco:** Arquitetura, Planejamento, Segurança, Correção, Gestão de Risco.
- **Responsabilidades:**
  - Analisar requisitos complexos e desenhar a solução técnica.
  - Criar especificações detalhadas e listas de tarefas (`task.md`).
  - Escrever testes de aceitação ou unitários (TDD) _antes_ da implementação.
  - Orquestrar a delegação de tarefas para os agentes CLI.
  - Revisar o código gerado (Code Review) e integrar ao branch principal.

### 2. The Swarm (Agentes CLI: Gemini, Kilocode, Opencode, Cline)

- **Foco:** Implementação Tática, Refatoração, DevOps, Testes.
- **Responsabilidades:**
  - Processar comandos especializados para cada função.
  - Executar builds, testes e operações Git autonomamente.

## Fluxo de Trabalho (The Loop)

O ciclo de desenvolvimento segue o padrão **Plan -> Delegate -> Verify**.

### Fase 1: Planejamento (Executive)

1. **Análise:** O Executive analisa a solicitação do usuário.
2. **Breakdown:** Quebra a solicitação em tarefas pequenas e testáveis.
3. **Specs/Docs:** Cria arquivos de especificação ou esqueletos de classes.

### Fase 2: Delegação (Handoff via Swarm Toolbelt)

O Executive utiliza os scripts especializados em `scripts/swarm/` para delegar tarefas específicas.

**Scripts Disponíveis:**

- `./scripts/swarm/architect.sh "Descrição"`: Design (Suporta BMAD Levels 0-4).
- `./scripts/swarm/coder.sh "Tarefa" [arquivos]`: Implementação de código.
- `./scripts/swarm/qa.sh "Feature"`: Geração de testes unitários.
- `./scripts/swarm/research.sh "Query"`: Pesquisas e validação.
- `./scripts/swarm/manager.sh "Update"`: Gestão via Conductor.
- `./scripts/swarm/integrator.sh "pr <N>"`: Gestão de PRs e Code Review.
- `./scripts/swarm/autonomous.sh "Tarefa"`: Execução autônoma "One-Shot".

**Exemplo:**

```bash
./scripts/swarm/architect.sh "Criar novo subsistema de pagamento" --level 3
```

### Fase 3: Execução (Swarm)

O agente selecionado processa o prompt especializado:

1. **Architect:** Gera planos em Markdown ou diagramas Mermaid.
2. **Coder:** Gera/Modifica código fonte.
3. **QA:** Gera suítes de teste.
4. **Integrator:** Analisa/Mergeia PRs.
5. **Autonomous:** Executa tarefas multi-passo até a conclusão.

### Fase 4: Verificação e Integração (Executive)

O Executive retoma o controle:

1. **Review:** Analisa as mudanças feitas pelo agente.
2. **Verify:** Roda a suíte de testes oficial.
3. **Commit:** Realiza o commit das alterações aprovadas.

## Refinamento: BMAD & AI-Context

Integrado em [Date], este workflow utiliza methodologies avançadas:

### 1. Contexto (@ai-coders/context)

Todos os agentes operam sobre uma base de conhecimento estruturada (`.context/`).

- **Setup:** Execute `./scripts/setup_context.sh` para inicializar.
- **Benefício:** Agentes "lembram" das decisões arquiteturais e padrões do projeto.

### 2. Planejamento Adaptativo (BMAD-METHOD)

O agente `architect` agora suporta níveis de profundidade:

- `--level 0` (Atomic): Fixes rápidos.
- `--level 1` (Basic): Features simples.
- `--level 2` (Standard): Padrão com testes.
- `--level 3` (Detailed): Design de sistemas complexos.
- `--level 4` (Strategic): Arquitetura corporativa.

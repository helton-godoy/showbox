---
status: filled
generated: 2026-01-12
---

# Plano de Desenvolvimento do ShowBox

> Plano abrangente para desenvolvimento, testes e distribuição do projeto ShowBox - ferramenta CLI baseada em Qt6 para criação de diálogos GUI avançados para scripts shell

## Task Snapshot

- **Primary goal:** Desenvolver e lançar a versão 1.0 do ShowBox, uma ferramenta completa para criação de interfaces gráficas via linha de comando
- **Success signal:** Todos os widgets principais implementados e testados, scripts de exemplo funcionais, pacotes de instalação para múltiplas plataformas criados e distribuídos
- **Key references:**
  - [Documentation Index](../docs/README.md)
  - [Agent Handbook](../agents/README.md)
  - [Plans Index](./README.md)

## Codebase Context

- **Total files analyzed:** ~80 arquivos principais
- **Total symbols discovered:** ~1500+ símbolos (classes, funções, métodos)
- **Arquitetura:** C++17 + Qt6, padrão Command para extensibilidade
- **Componentes principais:** ShowBox (core), Command Registry, Parser, Widgets customizados

## Agent Lineup

| Agent                  | Role in this plan                                   | Playbook                                                      | First responsibility focus                              |
| ---------------------- | --------------------------------------------------- | ------------------------------------------------------------- | ------------------------------------------------------- |
| Code Reviewer          | Garantir qualidade e consistência do código C++/Qt6 | [Code Reviewer](../agents/code-reviewer.md)                   | Revisar implementação dos comandos e widgets            |
| Bug Fixer              | Identificar e corrigir bugs na implementação        | [Bug Fixer](../agents/bug-fixer.md)                           | Analisar bugs nos widgets customizados                  |
| Feature Developer      | Implementar novos widgets e funcionalidades         | [Feature Developer](../agents/feature-developer.md)           | Desenvolver widgets restantes (Calendar, Table, Charts) |
| Refactoring Specialist | Melhorar estrutura e performance do código          | [Refactoring Specialist](../agents/refactoring-specialist.md) | Otimizar gerenciamento de layouts                       |
| Test Writer            | Criar suite completa de testes                      | [Test Writer](../agents/test-writer.md)                       | Escrever testes para comandos e widgets                 |
| Documentation Writer   | Criar documentação técnica e de usuário             | [Documentation Writer](../agents/documentation-writer.md)     | Documentar API e exemplos de uso                        |
| Performance Optimizer  | Otimizar renderização e uso de memória              | [Performance Optimizer](../agents/performance-optimizer.md)   | Melhorar performance dos charts customizados            |
| Security Auditor       | Avaliar segurança da aplicação                      | [Security Auditor](../agents/security-auditor.md)             | Verificar segurança no parsing de comandos              |
| Devops Specialist      | Configurar CI/CD e empacotamento                    | [Devops Specialist](../agents/devops-specialist.md)           | Automatizar builds e releases                           |

## Documentation Touchpoints

| Guide                        | File                                                       | Primary Inputs                                          |
| ---------------------------- | ---------------------------------------------------------- | ------------------------------------------------------- |
| Project Overview             | [project-overview.md](../docs/project-overview.md)         | Roadmap, README, stakeholder notes                      |
| Architecture Notes           | [architecture.md](../docs/architecture.md)                 | ADRs, service boundaries, dependency graphs             |
| Development Workflow         | [development-workflow.md](../docs/development-workflow.md) | Branching rules, CI config, contributing guide          |
| Testing Strategy             | [testing-strategy.md](../docs/testing-strategy.md)         | Test configs, CI gates, known flaky suites              |
| Glossary & Domain Concepts   | [glossary.md](../docs/glossary.md)                         | Business terminology, user personas, domain rules       |
| Data Flow & Integrations     | [data-flow.md](../docs/data-flow.md)                       | System diagrams, integration specs, queue topics        |
| Security & Compliance Notes  | [security.md](../docs/security.md)                         | Auth model, secrets management, compliance requirements |
| Tooling & Productivity Guide | [tooling.md](../docs/tooling.md)                           | CLI scripts, IDE configs, automation workflows          |

## Risk Assessment

### Identified Risks

| Risk                                          | Probability | Impact | Mitigation Strategy                            | Owner     |
| --------------------------------------------- | ----------- | ------ | ---------------------------------------------- | --------- |
| Dependências Qt6 não disponíveis              | Medium      | High   | Usar Docker para garantir ambiente controlado  | DevOps    |
| Curva de aprendizado Qt6                      | Low         | Medium | Documentação técnica detalhada                 | Tech Lead |
| Problemas de compatibilidade multi-plataforma | Medium      | High   | Testes em múltiplas plataformas via Docker     | QA Team   |
| Complexidade de widgets customizados          | High        | Medium | Desenvolvimento incremental e testes unitários | Core Team |

### Dependencies

- **Internal:** Qt6, qmake6, Docker
- **External:** Debian/Ubuntu para builds, distribuições Linux para testes
- **Technical:** C++17 compiler, Qt6 development libraries

### Assumptions

- Qt6 estará disponível em todas as plataformas alvo
- Scripts shell podem executar binários externos
- Usuários têm permissões para instalar pacotes

## Resource Estimation

### Time Allocation

| Phase                          | Estimated Effort   | Calendar Time   | Team Size      |
| ------------------------------ | ------------------ | --------------- | -------------- |
| Phase 1 - Core Implementation  | 15 person-days     | 3-4 weeks       | 2-3 people     |
| Phase 1.5 - Migração Parser V2 | 8 person-days      | 1-2 weeks       | 1-2 people     |
| Phase 2 - Widgets & Examples   | 15 person-days     | 3-4 weeks       | 2-3 people     |
| Phase 3 - Testing & Packaging  | 10 person-days     | 2-3 weeks       | 2-3 people     |
| **Total**                      | **48 person-days** | **10-13 weeks** | **2-3 people** |

### Required Skills

- C++17 e Qt6 expertise
- Desenvolvimento GUI
- Shell scripting (Bash)
- Empacotamento Linux (deb, rpm, AppImage)
- Docker e CI/CD

### Resource Availability

- **Available:** Desenvolvedor C++/Qt6, DevOps Engineer
- **Blocked:** Nenhum
- **Escalation:** Product Owner

## Working Phases

### Phase 1 — Core Implementation (Semanas 1-4)

**Steps:**

1. Finalizar implementação da classe ShowBox principal
2. Completar sistema de comandos (add, set, query)
3. Implementar parser e tokenizer
4. Criar sistema básico de layouts

**Commit Checkpoint:**

- Build funcionando, comandos básicos operacionais
- `git commit -m "feat: complete core implementation phase"`

### Phase 1.5 — Migração Completa Parser V2 (Semanas 4-5)

> [!IMPORTANT]
> Esta fase é crítica para deprecar o parser legado (`dialog_parser.cc`) e usar exclusivamente a nova arquitetura modular (`parser.cpp` + `tokenizer.cpp` + Command Pattern).

**Contexto:**

O Parser V1 (legado) é monolítico e processa comandos via máquina de estados.
O Parser V2 usa arquitetura modular com `CommandRegistry`, `Tokenizer` separado e padrão Command.

**Comandos Faltantes no V2:**

| Comando    | Descrição                              | Arquivo Alvo                           |
| ---------- | -------------------------------------- | -------------------------------------- |
| `position` | Move widget para antes/depois de outro | `commands/position_command.cpp` (novo) |
| `unset`    | Reverter propriedades de widgets       | `commands/unset_command.cpp` (novo)    |

**Widgets Faltantes no `add_command.cpp`:**

| Widget    | Descrição                 | Método ShowBox |
| --------- | ------------------------- | -------------- |
| `textbox` | Campo de entrada de texto | `addTextBox()` |
| `chart`   | Gráficos e visualizações  | `addChart()`   |

**Lógica Incompleta:**

| Comando | Problema                                     | Solução                              |
| ------- | -------------------------------------------- | ------------------------------------ |
| `end`   | Não diferencia tipo (tabs, page, table, etc) | Adicionar parsing de tipo como no V1 |

**Steps:**

1. Implementar `textbox` no `add_command.cpp`
2. Implementar `chart` no `add_command.cpp`
3. Criar `PositionCommand` em `commands/position_command.cpp`
4. Criar `UnsetCommand` em `commands/unset_command.cpp`
5. Melhorar `EndCommand` com lógica de tipo (groupbox, frame, listbox, combobox, table, tabs, page)
6. Registrar novos comandos no `dialog_main.cc`
7. Criar testes unitários para novos comandos
8. Validar compatibilidade com scripts existentes do dialogbox

**Commit Checkpoint:**

- Parser V2 com paridade completa ao V1
- `git commit -m "feat(parser): complete V2 migration with full V1 parity"`

---

### Phase 2 — Widgets & Examples (Semanas 6-9)

**Steps:**

1. Validar todos os widgets via Parser V2
2. Melhorar sistema de temas
3. Criar scripts de exemplo abrangentes para todos os widgets
4. Documentar API completa com exemplos V2

**Commit Checkpoint:**

- Todos os widgets funcionais via Parser V2, exemplos testados
- `git commit -m "feat: complete widgets and examples with V2 parser"`

### Phase 3 — Testing & Packaging (Semanas 10-12)

**Steps:**

1. Implementar suite de testes completa
2. Configurar CI/CD pipeline
3. Criar pacotes para todas as plataformas
4. Testes de integração e validação

**Commit Checkpoint:**

- Release candidate pronto para distribuição
- `git commit -m "feat: complete testing and packaging"`

## Rollback Plan

### Rollback Triggers

- Build quebrado por mudanças incompatíveis
- Regressões críticas nos widgets principais
- Problemas de segurança no parsing
- Falhas nos pacotes de distribuição

### Rollback Procedures

#### Phase 1 Rollback

- Action: Reverter para commit anterior ao Phase 1
- Data Impact: Perda de código implementado
- Estimated Time: 2-4 hours

#### Phase 2 Rollback

- Action: Reverter branch de features, manter core funcional
- Data Impact: Perda de widgets implementados
- Estimated Time: 4-8 hours

#### Phase 3 Rollback

- Action: Remover pacotes problemáticos, reverter CI config
- Data Impact: Nenhum (artefatos de build)
- Estimated Time: 1-2 hours

### Post-Rollback Actions

1. Documentar causa do rollback
2. Revisar plano e ajustar abordagem
3. Re-planejar implementação com lições aprendidas

## Evidence & Follow-up

### Artefatos a Coletar

- Logs de build e testes
- Screenshots dos widgets funcionais
- Scripts de exemplo executáveis
- Pacotes de instalação testados

### Follow-up Actions

1. Release notes da versão 1.0
2. Documentação de usuário final
3. Roadmap para versão 1.5
4. Feedback da comunidade

---

_Plano criado para guiar o desenvolvimento estruturado do ShowBox v1.0._

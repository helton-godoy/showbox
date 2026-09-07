# ADR 0006 — Governança remota e transição dos repositórios antigos

Estado: implementado na SB-006; proteções ativas e verificadas por API.

## Contexto

Nas tarefas anteriores (SB-001 a SB-005) o monorepo Showbox existia apenas
localmente, construído sobre uma cadeia de commits de outros repositórios
(`dialogbox`, `SHantilly`, `SHantilly-Studio`, `showbox-studio`). O CI era
valido localmente, mas o repositório canônico `helton-godoy/showbox` ainda
apontava para a estrutura pré-monorepo e sem regras de proteção. Operações
remotas dependiam de autorização do mantenedor (único usuário, conta única).

## Decisão

1. **Showbox é o repositório canônico** (`helton-godoy/showbox`). Todo
   desenvolvimento passa a acontecer nele; os quatro repositórios antigos são
   descontinuados com README de redirecionamento e arquivamento (`archived:
   true`), conservando seu histórico como referência de arquivo.
2. **Estratégia de integração**: `main` (entrega) recebe
   `integration/showbox-v1` (integração contínua) por PR com merge commit,
   preservando os SHAs da cadeia; tarefas entram por branches `feat/SB-*`
   integradas sequencialmente. Não há push direto de agentes.
3. **Proteção de branches**: `main` e `integration/showbox-v1` com PR
   obrigatório, branch atualizada antes do merge, required status checks do
   `Showbox CI` (build-test e sanitizers), enforce admins, sem force-push e sem
   exclusão. Aprovação humana em zero (repositório de conta única; garantia
   técnica por PR + checks).
4. **Tags `v*`**: protegidas contra sobrescrita/exclusão; releases criados
   somente pelo fluxo do `release.yml`.
5. **Release nesta etapa**: `release.yml` é exercitado via `workflow_dispatch`
   gerando artefatos e smokes, mas o job `release` só roda em tags — sem tag
   criada, **nenhum release público nem draft** é produzido na SB-006. O primeiro
   draft associado a uma tag real fica para a SB-007.
6. **Transição e arquivamento**: primeiro `main` consolidada e CI verde; depois
   READMEs de redirecionamento nos repositórios antigos e arquivamento por API.
   Branches obsoletas (`develop`, `bugfix/quick-fixes`) são removidas somente
   após inventário e verificação de alcançabilidade dos commits.

## Consequências

- O ciclo de push direto e o uso dos repositórios antigos ficam encerrados.
- Proteções impossibilitam regressão de força bruta (force-push/exclusão) em
  `main`, `integration/showbox-v1` e tags `v*`.
- Revisões exigem PR mesmo sem aprovação humana; a proteção consulta os checks
  reais produzidos pelo workflow.
- Repositórios arquivados ficam somente para leitura; migração de issues/PR não
  compõe esta tarefa.

## Verificação

Configuração efetiva conferida por API (`gh api .../branches/.../protection` e
ruleset de tags `tags-v-protection`), push direto e force-push rejeitados em
teste destrutivo controlado (branch descartável), e `just doctor` consistente.
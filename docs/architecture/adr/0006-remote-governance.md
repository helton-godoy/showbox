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
   obrigatório, branch atualizada antes do merge, required status checks
   `build-test` e `sanitizers` vinculados ao GitHub Actions (app_id 15368),
   enforce admins, sem force-push e sem exclusão. Aprovação humana em zero
   (repositório de conta única; garantia técnica por PR + checks).
4. **Tags `v*`**: protegidas contra sobrescrita/exclusão; releases criados
   somente pelo fluxo do `release.yml`.
5. **Release nesta etapa**: `release.yml` é exercitado via `workflow_dispatch`
   gerando artefatos e smokes, mas o job `release` só roda em tags — sem tag
   criada, **nenhum release público nem draft** é produzido na SB-006. O primeiro
   draft associado a uma tag real fica para a SB-008.
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

## Notas de implementação (2026-09-07)

- **Correção dos required checks**: a proteção inicial usava os nomes compostos
  `Showbox CI / build-test`/`Showbox CI / sanitizers`, que nunca correspondiam
  aos check-runs reais (`build-test`/`sanitizers`, app_id 15368). Todo PR ficava
  `BLOCKED` indefinidamente. Corrigido por API (contexts vinculados ao app_id do
  GitHub Actions) e documentado em `GIT_REMOTE.md`/`doctor.sh`.
- **Desinstalação de GitHub Apps de terceiros**: `codegen-sh`, `prefect-horizon`,
  `cursor`, `continue`, `kilo-code-bot` e `trunk-io` foram desinstalados por não
  terem worker ativo; suas check-suites ficavam `queued` para sempre e
  bloqueavam merges. Remoção manual na conta (UI GitHub), sem perda funcional.
  O Trunk é re-integrado na SB-007 como ferramenta de validação.
- **Exercício do `release.yml`**: dispachado via `workflow_dispatch` em `main`
  (run 34149545912) — artefatos e smokes verdes, job `release` skipped (sem
  tag). Nenhum release/draft criado na SB-006.
- **Arquivamento**: os quatro repositórios antigos
  (`dialogbox`, `SHantilly`, `SHantilly-Studio`, `showbox-studio`) receberam
  README de redirecionamento na default branch e foram arquivados
  (`archived: true`).
- **Branches obsoletas**: `develop` e `bugfix/quick-fixes` removidas do remoto
  após confirmação de que seu head (`2848cb3`) é ancestral de `main`.

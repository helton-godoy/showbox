# SB-006 — Proteção Git remota e transição dos repositórios antigos

Estado: concluída em 2026-09-07.

- Objetivo: proteger as branches de integração do `helton-godoy/showbox` no
  GitHub, fazer o primeiro push do monorepo (integration e main consolidada),
  ativar o CI remoto, exercitar a pipeline `release.yml` em draft e descontinuar
  os repositórios antigos (`dialogbox`, `SHantilly`, `SHantilly-Studio`,
  `showbox-studio`) com redirecionamento para o monorepo — sem publicar release
  final nem alterar serviços fora dos repositórios do produto.
- Base: integration/showbox-v1 (8929724, encerramento da SB-006A; base efetiva
  atualizada após fast-forward nesta worktree).
- Branch prevista: feat/SB-006-protecao-remota.
- Responsável: integrador desta sessão (mantenedor do repositório).
- Dependências: SB-005 (packaging, `release.yml` e smoke prontos).
- Decisões do mantenedor (registradas nesta sessão):
  1. Arquivar os 4 repositórios antigos no GitHub;
  2. Proteger `main` e `integration/showbox-v1` com PR obrigatório + status
     checks do CI, **sem exigir aprovação humana** (repositório de conta única);
  3. Incluir nesta tarefa: primeiro push, CI remoto ativo e exercício manual do
     `release.yml` via `workflow_dispatch`, **sem tag e sem draft**. O
     `workflow_dispatch` constrói/testa os artefatos, mas o job que cria o
     release só roda em tags; a lacuna do contrato original foi ajustada para
     não criar tag temporária. O primeiro draft associado a uma tag real fica
     para a SB-007, após esta tarefa publicar e proteger o canônico.
- Escopo:
  - Governança local: criar `docs/development/GIT_REMOTE.md` (política de
    branches protegidas, PRs obrigatórios, tags `v*`, descontinuação do ciclo
    antigo e do fluxo de push direto) e ADR 0006 (governança de remotos e
    descontinuação dos repositórios antigos). `just doctor` passa a verificar
    autenticação `gh` e proteção das branches.
  - Push inicial (antes de proteger): enviar `integration/showbox-v1` para
    `origin` (fast-forward, base `a5dd820` remota) e abrir PR
    `integration/showbox-v1 → main` com merge commit (mantém os SHAs da cadeia),
    consolidando o monorepo em `main` com CI remoto verde.
  - Proteção remota via GitHub API (`gh api`): `main` e
    `integration/showbox-v1` com required status checks, "branches up to date",
    enforce admins, sem force-push e sem exclusão; proteção de tags pattern `v*`
    (bloqueia sobrescrita/exclusão) via ruleset `tags-v-protection`. Aplicar
    somente após os runs do CI existirem; verificar o resultado via API e
    documentar na ADR 0006. Os nomes efetivos dos contexts são `build-test` e
    `sanitizers` (app_id 15368 / GitHub Actions), conforme os check-runs reais
    do workflow `Showbox CI`.
  - CI remoto e pipeline: primeira execução dos jobs do `ci.yml` e exercício do
    `release.yml` via `workflow_dispatch` **sem tag** (jobs de artefato e
    instalação/smoke em container; o job `release` fica inativo sem tag e o
    primeiro draft fica para a SB-007).
  - Transição dos repositórios antigos: adicionar README de redirecionamento no
    default branch de cada um (`dialogbox`, `SHantilly`, `SHantilly-Studio`,
    `showbox-studio`) apontando para `helton-godoy/showbox` e arquivar
    (`archived: true`) via API. Descontinuar as branches obsoletas `develop` e
    `bugfix/quick-fixes` do remoto após `main` consolidada.
  - Não alterar o conteúdo/layout do monorepo nesta tarefa; mudanças de produto
    continuam em SB-00X separadas.
- Fora do escopo: release final publicado (tag real e assets públicos ficam na
  SB-007), assinatura/notarização de pacotes, GPG, migração de issues/PR dos
  repositórios antigos, exclusão de clones locais, novos runners/plataformas de
  CI, mudança das estratégias de merge habilitadas no repositório.
- Aceite:
  - `integration/showbox-v1` e `main` consolidadas no remoto com o monorepo
    completo (heads verificados por `git ls-remote origin`).
  - Branch protection ativa e comprovável por API para `main` e
    `integration/showbox-v1` (push direto rejeitado; PR com status checks
    obrigatórios); tags `v*` protegidas.
  - `ci.yml` e `release.yml` executados no remoto: jobs de build/test/smoke
    verdes, artefatos validados; **nenhum release público nem draft criado** na
    SB-006 (primeiro draft fica para SB-007 com tag real).
  - Repositórios antigos com README de redirecionamento e `archived: true`.
  - `just doctor` verde (inclui verificação de proteção remota);
    `just build`/`just test` verde.
  - Sem alterações de produto sem tarefa própria; sem force-push em branches
    remotas não autorizadas; sem publicação de release final.

## Handoff

- SHA base: `8929724` (integration/showbox-v1, encerramento da SB-006A; base
  efetiva do push inicial após fast-forward). SHA final de `main`: `73c2c60`;
  `integration/showbox-v1`: `66860c3`.
- PRs abertos e merged nesta tarefa (todos com checks verdes — build-test,
  sanitizers, SonarCloud):
  - PR #1 `integration/showbox-v1 → main`: merge commit `a2a0669` (pais
    `eab3300` + `66860c3`; cadeia de SHAs preservada, verificada com
    `git merge-base --is-ancestor`).
  - PR #2 `chore/SB-006-governanca-tags → main`: merge commit `93f6d07`
    (correção dos contexts + docs de governança + registros SB-007).
  - PR #3 `chore/SB-006-conclusao → main`: merge commit `73c2c60` (este
    handoff e marcação de conclusão).
- SHAs remotos finais: `main` = `73c2c60`; `integration/showbox-v1` = `66860c3`
  (verificados por `gh api .../branches`). Branch `chore/SB-006-conclusao`
  removida após o merge (política de branches de tarefa).
- Proteção de branches (`main` e `integration/showbox-v1`), configurada por
  API e confirmada em `just doctor`:
  - PR obrigatório, strict (`Update branch`), approvals = 0 (conta única);
  - required checks: `build-test` e `sanitizers` vinculados ao app_id 15368
    (GitHub Actions) — contextos corrigidos para os nomes reais dos check-runs;
  - enforce admins = true; `allows_force_pushes` = false; `allows_deletions` =
    false.
- Ruleset de tags: `tags-v-protection` (id 22465807), enforcement `active`,
  condição `refs/tags/v*`, regras `deletion` (bloqueada) e `non_fast_forward`
  (bloqueada). Criação de tag `v*` é permitida (não bloqueada pelo ruleset);
  deleção via push e via API (`DELETE /git/refs/tags`) é rejeitada.
- Runs relevantes:
  - CI remoto (`Showbox CI`): runs verdes em `integration/showbox-v1`
    (34140117838), em `main` (34142557487) e nos PRs #1/#2/#3.
  - `Showbox Release` via `workflow_dispatch` em `main`: run 34149545912 —
    jobs `deb-ubuntu`, `deb-debian`, `rpm`, `appimage`, `install-smoke` verdes;
    job `release` **skipped** (sem tag). Artefatos: `debs-ubuntu` (267754 B),
    `debs-debian` (7558704 B), `rpms` (4751407 B), `appimage` (26696663 B).
  - Run indesejado da tag de teste `v1.0.0-test` (34149471303) cancelado antes
    de criar qualquer draft — `gh run cancel` + verificação de releases = 0.
- Releases: **nenhum** release público nem draft. `gh api .../releases` = 0.
- Repositórios arquivados (commit de redirecionamento na default branch +
  `archived: true`): `dialogbox` (default `master`, head `e1cb356`),
  `SHantilly` (head `2b52ed6`), `SHantilly-Studio` (head `4e1e45c`),
  `showbox-studio` (head `ce3f2e5`) — todos arquivados via `PATCH ... -F
  archived=true` e confirmados com `isArchived=true`.
- Branches remotas removidas: `develop` e `bugfix/quick-fixes` (head comum
  `2848cb3`, confirmado ancestral de `main` por `git merge-base --is-ancestor`;
  commits preservados no monorepo). Branch temporária `tmp/SB-006-destructive-
  test` e `chore/SB-006-governanca-tags` removidas após uso.
- Verificações finais (locais, em `main` = `73c2c60`): `just doctor` verde
  (branches, proteções, checks e ruleset de tags); `just build` verde;
  `just test` verde (23/23).
- Problemas encontrados e correções:
  1. **Required checks com nomes compostos** (`Showbox CI / build-test`, `Showbox
     CI / sanitizers`) não correspondiam aos check-runs reais (`build-test`,
     `sanitizers`), deixando todo PR `BLOCKED` com `mergeStateStatus` indefinido.
     Corrigido por API: contexts = `build-test`/`sanitizers` com app_id 15368.
     Documentação e `doctor.sh` alinhados no mesmo commit (`4b5f5ba`, PR #2).
  2. **Check-suites `queued` de GitHub Apps de terceiros** (`codegen-sh`,
     `trunk-io`, `prefect-horizon`, `cursor`, `continue`, `kilo-code-bot`)
     instalados sem worker ativo ficavam `queued` para sempre e bloqueavam
     merges. Removidos manualmente pelo mantenedor na UI
     (`https://github.com/settings/installations`); o token `gh` não cobre a
     API de instalações (`/user/installations` → 403), por isso a desinstalação
     foi manual. Confirmado por novo SHA: só restaram GitHub Actions + SonarCloud.
  3. **Tag de teste `v1.0.0-test`** criada durante o teste destrutivo (criação
     de tag não é bloqueada pelo ruleset); como deleção era bloqueada, a
     remoção exigiu desativar temporariamente o ruleset (PUT `enforcement:
     disabled`), apagar a ref e reativar (`active`), e cancelar o run de release
     que ela disparou. Ruleset reativado e conferido.
- Limitações transferidas para a SB-007:
  - Primeiro release: draft associado a uma tag real `v*` (arquivos + notas)
    será publicado na SB-007; a SB-006 exercitou apenas os artefatos sem tag.
  - Trunk: tende a ser reintroduzido na SB-007 como ferramenta central de
    validação (contexto: `tasks/SB-007-trunk.md`); se instalado, exigirá worker
    ativo para não reincidir no bloqueio de check-suites `queued`.
  - Blobs grandes `core.*` no histórico de commits (núcleos antigos já fora da
    árvore) permanecem no histórico — candidatos a limpeza futura, fora desta
    tarefa.
  - Proteções padrão exigem conta com permissões de administrador no
    repositório (mantenedor único); regras e rulesets são operados via API `gh`.
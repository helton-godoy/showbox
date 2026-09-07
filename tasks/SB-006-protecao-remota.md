# SB-006 — Proteção Git remota e transição dos repositórios antigos

Estado: em execução.

- Objetivo: proteger as branches de integração do `helton-godoy/showbox` no
  GitHub, fazer o primeiro push do monorepo (integration e main consolidada),
  ativar o CI remoto, exercitar a pipeline `release.yml` em draft e descontinuar
  os repositórios antigos (`dialogbox`, `SHantilly`, `SHantilly-Studio`,
  `showbox-studio`) com redirecionamento para o monorepo — sem publicar release
  final nem alterar serviços fora dos repositórios do produto.
- Base: integration/showbox-v1 (c25a9ae, encerramento da SB-005).
- Branch prevista: feat/SB-006-protecao-remota.
- Responsável: integrador desta sessão (mantenedor do repositório).
- Dependências: SB-005 (packaging, `release.yml` e smoke prontos).
- Decisões do mantenedor (registradas nesta sessão):
  1. Arquivar os 4 repositórios antigos no GitHub;
  2. Proteger `main` e `integration/showbox-v1` com PR obrigatório + status
     checks do CI, **sem exigir aprovação humana** (repositório de conta única);
  3. Incluir nesta tarefa: primeiro push, CI remoto ativo e execução do
     `release.yml` em draft via `workflow_dispatch`.
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
    `integration/showbox-v1` com required status checks
    (`Showbox CI / build-test` e `Showbox CI / sanitizers`), "branches up to
    date", enforce admins, sem force-push e sem exclusão; proteção de tags
    pattern `v*` (bloqueia sobrescrita/exclusão). Aplicar somente após os runs
    do CI existirem; verificar o resultado via API e documentar na ADR 0006.
  - CI remoto e pipeline: primeira execução dos jobs do `ci.yml` e exercício do
    `release.yml` via `workflow_dispatch` gerando **draft** (jobs de artefato e
    instalação/smoke em container; o job `release` fica inativo sem tag).
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
    verdes e draft de release criado (sem publicar).
  - Repositórios antigos com README de redirecionamento e `archived: true`.
  - `just doctor` verde (inclui verificação de proteção remota);
    `just build`/`just test` verde.
  - Sem alterações de produto sem tarefa própria; sem force-push em branches
    remotas não autorizadas; sem publicação de release final.
- Handoff ao final com SHA base/final, branches remotas alteradas, comandos
  executados (gh/api) e limitações (draft/smoke dependentes do runner do
  GitHub; proteções exigem conta com permissões no repositório).
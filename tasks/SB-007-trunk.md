# SB-007 — Trunk como ferramenta central de validação

Estado: proposta.

- Objetivo: tornar o Trunk a ferramenta central de todas as checagens do
  projeto. Todo conteúdo presente no repositório que tenha suporte pelo Trunk
  deve passar pela validação dele (lint, formatação, segurança/diff e
  segredos). Se houver material no projeto que ainda não seja validado por
  qualquer meio, mas que possa ser coberto pelo Trunk, ele deve ser incluído no
  escopo de cobertura desta tarefa.
- Origem: registrada durante a SB-006 (2026-09-07) ao constatar que o
  repositório contém `.trunk/trunk.yaml` já configurado (lint: actionlint,
  checkov, git-diff-check, hadolint, markdownlint, prettier, shellcheck, shfmt,
  trufflehog, yamllint; runtimes go/node/python; ações trunk-announce,
  trunk-check-pre-push, trunk-fmt-pre-commit, trunk-upgrade-available) e
  `.trunk/configs/` (`.hadolint.yaml`, `.markdownlint.yaml`, `.shellcheckrc`,
  `.yamllint.yaml`, `.gitignore`), mas sem execução integrada no fluxo local
  (`just check`) nem no CI (`ci.yml`).
- Gap atual identificado (cobertura por outros meios):
  - `shellcheck`: apenas `shell/lib/runtime.sh` e `examples/hello-world/run.sh`
    (2 de 59 `.sh`); o restante passa somente por `bash -n`.
  - `git diff --check` e `bash -n`: já existem no `just check`.
  - `hadolint`: inexistente (8 Dockerfiles no repositório).
  - `markdownlint`, `yamllint`, `actionlint`, `trufflehog`, `checkov`,
    `shfmt`, `prettier`: inexistentes por outros meios.
- Base: após a SB-006 (main consolidada e CI verde; proteções ativas).
- Branch prevista: feat/SB-007-trunk.
- Responsável: integrador desta sessão.
- Dependências: SB-006 (main publicada e protegida; ambiente remoto com
  integração Trunk habilitada).
- Escopo:
  - Integrar o Trunk ao fluxo local (`just check`/`just doctor`) e ao CI
    (`ci.yml`): rodar `trunk check` (lint + formatação + segredos + verificação
    de diff) sobre todo o repositório, com versões pinadas pelo `trunk.yaml`.
  - Definir cobertura completa: todo arquivo no repo com suporte Trunk passa
    pela validação; para conteúdo hoje não validado mas suportado, adicioná-lo
    (ex.: Dockerfiles, YAML, Markdown, GitHub Actions, scripts shell
    restantes).
  - Tratar bloqueios de material existente que falhar nas novas checagens:
    corrigir, documentar exceções justificadas em `.trunk/trunk.yaml` ou
    `.trunk/configs/*` (não desativar em massa).
  - Instalar/habilitar a integração do Trunk no GitHub ("Quality" / checks de
    PR) para que o `trunk check` rode como check obrigatório nos PRs, convivendo
    com `Showbox CI`; ajustar branch protection se necessário após run real do
    check do Trunk.
  - Documentar em `docs/development/` e `justfile` (`just check` passa a
    invocar o Trunk ou mantém os passos locais equivalentes + Trunk).
- Fora do escopo: alterações de produto sem tarefa própria, publicação de
  release, mudança de protocolo do motor.
- Aceite:
  - `trunk check` executa local e no CI com 100% de cobertura dos arquivos
    suportados do repositório.
  - Todo conteúdo do repo com suporte Trunk está sob validação; conteúdo que
    seria elegível passa a ser validado nesta tarefa.
  - `just check`/`just doctor` e CI verdes com Trunk integrado (incluindo o
    check de PR quando aplicável).
  - Documentado em `docs/development/` e no ROADMAP.
- Handoff ao final com SHA base/final, comandos executados, arquivos cobertos,
  exceções configuradas e resultado dos checks/CI.
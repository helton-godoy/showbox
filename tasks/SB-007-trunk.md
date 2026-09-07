# SB-007 — Trunk como ferramenta central de validação

Estado: concluída.

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
- Base: main consolidada da SB-006 (sincronizada em 2026-09-07).
- Branch prevista: feat/SB-007-trunk.
- Responsável: integrador desta sessão.
- Dependências: SB-006 (main publicada e protegida; CI verde). **O GitHub App do
  Trunk permanece desinstalado** nesta tarefa — ver implementação 1 a 5.
- Estratégia de implementação (definida na abertura da tarefa, 2026-09-07):
  1. Instalar e fixar o **Trunk CLI** (versão `1.25.0`, conforme `trunk.yaml`) e
     verificá-lo no `doctor.sh`.
  2. Executar `trunk check --all` localmente **sem alterações automáticas** e
     salvar a lista de falhas agrupadas por ferramenta e diretório.
  3. Corrigir cada grupo em commits separados (Shell; YAML e workflows;
     Markdown; Dockerfiles; formatação; segurança e segredos).
  4. Usar exclusões somente para falsos positivos identificados
     individualmente (não desativar em massa).
  5. Integrar `trunk check` ao `just check`.
  6. Criar um job próprio **`trunk-check`** no GitHub Actions (`ci.yml`).
  7. Validar o job em um PR (run real verde).
  8. Adicionar `trunk-check`, vinculado ao GitHub Actions, aos required checks
     de `main`/`integration/showbox-v1` — **somente após run real verde**.
     O GitHub App do Trunk não é reinstalado nesta etapa: ele está associado
     principalmente ao Merge Queue (exige organização ativa no serviço para
     processar eventos) e reprisaria o bloqueio de check-suites `queued` da SB-006
     se instalado isoladamente. O Merge Queue pode ser avaliado em tarefa futura.
- Baseline registrado (passo 2 — 2026-09-07, Trunk CLI 1.25.0, commit
  `0e59cb4`):
  - Comando: `trunk check --all --no-fix --print-failures` (saída salva no
    trabalho local; sem alterações aplicadas).
  - Arquivos verificados: **325**.
  - Totais: **275 lint issues** (89 auto-fixable), **13 security issues**,
    **30 unformatted files**.
  - Lint por linter (275): `shellcheck` 181, `markdownlint` 81, `hadolint` 10,
    `yamllint` 3.
  - Segurança (13): `checkov` 13 — CKV_DOCKER_2 ×6 (HEALTHCHECK ausente),
    CKV_DOCKER_3 ×6 (usuário não criado no container), CKV_DOCKER_7 ×1 (imagem
    base sem tag explícita).
  - Formatação (30): `shfmt` 18 arquivos `.sh` + `prettier` 11 arquivos
    (markdown/yaml), 1 arquivo adicional na contagem agregada da execução
    registrada.
  - shfmt (18): examples/demos/demo1_compat.sh, examples/demos/demo2_compat.sh,
    examples/demos/visual_layout_test.sh, examples/hello-world/run.sh,
    examples/showbox_calendar.sh, examples/showbox_charts.sh,
    examples/showbox_pushbutton.sh, examples/showbox_slider.sh,
    examples/showbox_table.sh, examples/showbox_textbox.sh,
    shell/lib/runtime.sh, tests/compatibility/build_legacy_oracle.sh,
    tests/compatibility/golden_contract.sh, tests/installation/install_smoke.sh,
    tests/integration/cli_contract.sh, tests/integration/display_backends.sh,
    tools/setup/debian.sh, tools/setup/doctor.sh.
  - Ferramenta que falhou: `shfmt` apresentou crash interno transiente
    (`absl::container_internal::raw_hash_map<>::at`) ao processar batches ≥ 21
    arquivos; a reexecução com cache estabilizou. Limitação conhecida a
    validar no CI (job `trunk-check` deve rodar em lote único).
  - Sem achados: `actionlint`, `trufflehog`, `git-diff-check`.
  - Arquivos afetados: 78 caminhos listados (docs/`*.md`, `tasks/*.md`,
    `.github/workflows/*.yml`, `packaging/**/Dockerfile` e scripts,
    `src/dev.Dockerfile`, `examples/**`, `tests/**`, `tools/setup/*.sh`,
    `shell/lib/runtime.sh`, ADRs).
  - Falsos positivos candidatos (a decidir nos commits de correção, sem
    desativar em massa): shellcheck SC2034 (variáveis exportadas/usadas
    externamente), SC2249 (arquivos sem extensão `.sh`), markdownlint MD040 em
    blocos sem idioma (avaliar converter ou anotar), yamllint quoted-strings
    (aspas redundantes em fluxos de texto).
- Execução (passo 3 — 2026-09-07, commits em `feat/SB-007-trunk`):
  - `2d37695` chore(trunk): instala e verifica Trunk CLI 1.25.0
    (`tools/setup/trunk.sh` + `doctor.sh`).
  - `cbef604` style(shell): shfmt + shellcheck em todos os 67 arquivos shell.
  - `bd58183` style(docs): prettier em 62 Markdown + markdownlint zerado.
  - `b58a49c` fix(ci): aspas redundantes em workflows (yamllint).
  - `e5e4d38` fix(packaging): Dockerfiles (hadolint + checkov; fedora pinado em
    `fedora:46`; `--no-install-recommends`; `HEALTHCHECK NONE`).
  - Estado final local: `trunk check --all --no-fix --cache=false` → **0
    issues** (325 arquivos).
  - Exceções configuradas (todas com justificativa por achado, sem desativar em
    massa):
    - `.trunk/configs/.markdownlint.yaml` → `MD051: false`: falso positivo do
      parser micromark do markdownlint 0.47 em âncoras de TOC válidas pt-BR
      quando o documento contém blocos de código (comprovado por caso isolado).
    - `.trunk/configs/.shellcheckrc` → `SC2154` (pré-existente, mantido).
    - Dockerfiles: `# hadolint ignore=DL3008`/`DL3041` por arquivo (pacotes do
      repositório da distro, fixado pelo tag da imagem base); `checkov:skip`
      `CKV_DOCKER_3` por imagem (build/CI efêmero roda como root para gerar
      artefatos e montar volume do host). CKV_DOCKER_2 resolvido com
      `HEALTHCHECK NONE` (config real, não exceção).
    - Usos únicos de `# shellcheck disable=SC2312/SC2310` com justificativa
      inline onde mascarar o exit code é intencional (heredoc/pipe de
      extração).
  - Cobertura final: 67 shell (inclusive AppRun), 76 Markdown, 8 Dockerfiles,
    2 workflows YAML; sem achados em actionlint/trufflehog/git-diff-check.
  - Limitação conhecida: crash transiente do `shfmt` em batches ≥ 21 arquivos;
    o job `trunk-check` roda em lote único (`check-mode: all`) — validar no CI.
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
  - Publicar o `trunk-check` como job do GitHub Actions (check obrigatório de
    PR), convivendo com `Showbox CI`; ajustar branch protection **somente após
    run real verde** do job no CI. O GitHub App do Trunk permanece desinstalado
    nesta tarefa (integrado ao Merge Queue/serviço, para avaliar em tarefa
    futura).
  - Documentar em `docs/development/` e `justfile` (`just check` passa a
    invocar o Trunk ou mantém os passos locais equivalentes + Trunk).
- Handoff (2026-09-07):
  - SHA base: `0e59cb4` (main) → SHA final: `186ee01` (feat/SB-007-trunk),
    integrado em `main` via PR #6 (merge commit `b46f2ad`).
  - Comandos executados: `trunk check --all --no-fix --cache=false` (0 issues
    em 325 arquivos), `just check`, `just test` (23/23), oráculo legado
    (`tests/compatibility/build_legacy_oracle.sh`) e contrato golden
    (`tests/compatibility/golden_contract.sh` — diferença de locale
    decimal vírgula/ponto pré-existente no main, sem relação com a SB-007),
    `tests/integration/cli_contract.sh`, build podman de
    `packaging/deb/debian.Dockerfile` e `packaging/rpm/fedora.Dockerfile`.
  - CI: run real verde do job `trunk-check` (app `github-actions`, app_id 15368) junto com `build-test`, `sanitizers` e SonarCloud no PR #6.
  - Proteção aplicada em `main` e `integration/showbox-v1`: required checks
    `build-test`, `sanitizers` e `trunk-check` (app_id 15368, strict).
  - Prova de bloqueio: PR #8 (canário com SC2086 deliberado) → `trunk-check`
    falhou e o GitHub reportou `mergeStateStatus: BLOCKED`; canário fechado
    sem merge. PR #7 (canário sobre main antiga, sem o job) validou que a
    ausência do check também bloqueia.
  - Integridade do launcher: SHA-256 `89fbdd8c...` fixado em
    `tools/setup/trunk.sh` e validado antes da execução (rejeição testada com
    hash inválido).
  - Limitações: diferença de locale no golden contract (pré-existente);
    smoke de instalação em container não executado (exige imagens de distro
    completas — build das imagens de empacotamento validado).
  - Auto-merge nativo: não habilitado neste PR, conforme decisão de revisão
    manual; avaliar na SB-008.
- Fora do escopo: alterações de produto sem tarefa própria, publicação de
  release (primeiro draft fica para a SB-008), Merge Queue do Trunk, mudança de
  protocolo do motor.
- Aceite:
  - `trunk check` executa local e no CI com 100% de cobertura dos arquivos
    suportados do repositório.
  - Todo conteúdo do repo com suporte Trunk está sob validação; conteúdo que
    seria elegível passa a ser validado nesta tarefa.
  - `just check`/`just doctor` e CI verdes com Trunk integrado; job
    `trunk-check` verde em run real e adicionado aos required checks
    (`main`/`integration/showbox-v1`), sem o GitHub App do Trunk instalado.
  - Documentado em `docs/development/` e no ROADMAP.
- Handoff ao final com SHA base/final, comandos executados, arquivos cobertos,
  exceções configuradas, job de CI e resultado dos checks.

# SB-007 — Trunk como ferramenta central de validação

Estado: em execução.

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
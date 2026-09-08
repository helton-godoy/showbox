# SB-008 — Primeiro release candidato e validação dos artefatos

Estado: concluída (corretivo P1 aplicado, 2026-09-08).

- Objetivo: produzir o primeiro release candidato da suíte Showbox a partir de
  uma tag real `v*`, validar os artefatos (deb ubuntu/debian, rpm, AppImage) e
  publicar o primeiro **draft** de release associado à tag — sem promover a
  pressa: a validação acontece antes de qualquer disponibilização pública.
- Origem: desmembrada da SB-007 (2026-09-07). O handoff da SB-006 transferia o
  primeiro draft para a SB-007, mas a SB-007 ficou concentrada em lint,
  formatação, segurança e segredos; o draft passou a ter tarefa própria.
- Base: `c4a1148` (main consolidada da SB-007, com Trunk integrado ao fluxo
  local e ao CI como check obrigatório).
- Branch prevista: feat/SB-008-release-candidate.
- Responsável: integrador desta sessão.
- Dependências: SB-005 (packaging e `release.yml` prontos), SB-007 (validação
  integrada).
- Escopo:
  - Criar tag `v*` real (ex.: `v1.0.0-rc.1`) na `main` consolidada — a
    proteção `tags-v-protection` já bloqueia sobrescrita/exclusão, mas permite
    criação.
  - Disparar o `release.yml` via push da tag; validar jobs de artefato e
    `install-smoke` verdes e o job `release` criando o **draft** com arquivos e
    notas de release.
  - Validar os artefatos gerados (instalação em container, smokes de CLI
    contratual, conteúdo dos pacotes) e conferir o draft no GitHub.
- Handoff (2026-09-08):
  - Base: `c4a1148`; tag criada sobre `fa9443e` (main consolidada com o fix do
    filtro de debug packages do `release.yml`).
  - Validação pré-tag (critério do contrato): `workflow_dispatch` do
    `release.yml` em `main` → 4 jobs de artefato + 4 smokes verdes (run
    34172441535).
  - Tag `v1.0.0-rc.1` (SHA `8f1db53`): run 34172749025 com 6/6 jobs verdes,
    draft criado, porém com assets `*dbgsym` escapando do filtro (só cobria
    nomes RPM). Draft descartado antes da publicação; fix em PR #12
    (`081c506`, merge `fa9443e`).
  - Tag **`v1.0.0-rc.2`** (SHA `fa9443e`): run 34173567000 com 6/6 jobs
    verdes (deb-ubuntu, deb-debian, rpm, appimage, install-smoke, release).
  - Draft final: `ShowBox v1.0.0-rc.2` (draft: true) com 5 assets —
    `showbox_1.0.0-1_amd64.deb`, `showbox-studio_1.0.0-1_amd64.deb`,
    `showbox-1.0.0-1.fc46.x86_64.rpm`,
    `showbox-studio-1.0.0-1.fc46.x86_64.rpm`,
    `ShowBox-Studio-1.0.0-x86_64.AppImage` — e notas geradas
    (`generate_release_notes: true`).
  - Nenhum release público; promoção draft → público é decisão do mantenedor.
  - Limitações: validação dependente dos runners do GitHub (docker como
    engine); draft não promovido; smokes rodam em runners ubuntu-24.04, não
    em nativas das distros.
  - Proteção de tags validada em produção: `tags-v-protection` permitiu a
    criação de `v1.0.0-rc.1`/`rc.2` sem sobrescrita.
- Corretivo P1 (2026-09-08) — parecer de revisão bloqueando a promoção do rc.2:
  1. **Pacotes não carregam a versão candidata**: todo o ecossistema estava fixo
     em `1.0.0` (changelog, spec, AppImage, binários), o que levaria
     gerenciadores a tratar o RC como estável. Corrigido com **fonte única**
     `VERSION` (SemVer) → `tools/version.sh` converte para cada formato:
     Debian `1.0.0~rc3-1`, RPM `Version 1.0.0`/`Release 0.3.rc3`, AppImage e
     binários `1.0.0-rc.3`.
  2. **Colisão de basenames DEB Ubuntu/Debian no draft**: os dois builds geravam
     `showbox_*_amd64.deb` idênticos; apenas uma variante sobrevivia. Nomes agora
     carregam a distribuição (`_ubuntu24.04_`, `_debian13_`) e o job `release`
     valida basenames únicos antes do upload.
  3. **Draft sem `prerelease`**: o `release.yml` agora calcula
     `prerelease: true` quando a tag tem sufixo (ex.: `v1.0.0-rc.3`), preservando
     `v1.0.0` como estável no futuro.
  - Corretivo executado (todos os P1 validados):
    - PR #14 (`9b417d8` → `fix/SB-008-p1-version-single-source`, gates
      build-test/sanitizers/trunk-check/SonarCloud verdes) implementou a fonte
      única `VERSION`/`tools/version.sh`, nomes DEB com distribuição,
      `prerelease` calculado pela tag e a validação tag == `v$VERSION` com
      checagem de basenames únicos no `release.yml`.
    - Validação pré-tag: `workflow_dispatch` do `release.yml` em `main` (run 34177126323) → 4 artefatos + install-smoke verdes; job `release` pulado
      (sem tag), como previsto.
    - Validação local: 23/23 testes; `showbox --version` e
      `showbox-studio --version` reportam `1.0.0-rc.3`; smokes deb ubuntu e
      debian, rpm e AppImage verdes; 7 artefatos com basenames únicos.
    - Tag **`v1.0.0-rc.3`** (SHA `9b417d8`): run 34177442699 com 6/6 jobs
      verdes → draft `ShowBox v1.0.0-rc.3` (`draft: true`, `prerelease: true`)
      com 7 assets — 2 debs ubuntu, 2 debs debian, 2 rpms e o AppImage do
      Studio, todos com a versão candidata (deb interno `1.0.0~rc3-1`; rpm
      `1.0.0-0.3.rc3.fc46`; AppImage `1.0.0-rc.3`).
    - Draft `rc.2` removido após a validação do `rc.3` (tags preservadas; a
      proteção `tags-v-protection` continua validando criação sem sobrescrita).
    - Observação: o GitHub normaliza `~` → `.` apenas **no nome do asset**
      (ex.: `showbox_1.0.0.rc3-1_ubuntu24.04_amd64.deb`); a versão interna do
      pacote mantém `1.0.0~rc3-1`, correta para ordenação no apt.
- Fora do escopo: release público/promovido, assinatura/notarização de pacotes,
  GPG, distribuição em repositórios de terceiros (PPA etc.), Merge Queue do
  Trunk. A promoção do realease (de draft para público) é decisão do
  mantenedor posterior.
- Aceite:
  - Tag `v*` real em `main`, protegida por `tags-v-protection`.
  - `release.yml` executado: jobs de artefato e smoke verdes; **draft** criado
    com assets e notas; nenhum release público.
  - Artefatos validados no CI/dispatch antes do registro do draft.
  - Documentado no ROADMAP e no handoff da tarefa.
- Handoff ao final com SHA da tag, run do release.yml, artefatos validados,
  URL do draft e limitações (draft não promovido; validação dependente do
  runner do GitHub).

# SB-008 — Primeiro release candidato e validação dos artefatos

Estado: proposta.

- Objetivo: produzir o primeiro release candidato da suíte Showbox a partir de
  uma tag real `v*`, validar os artefatos (deb ubuntu/debian, rpm, AppImage) e
  publicar o primeiro **draft** de release associado à tag — sem promover a
  pressa: a validação acontece antes de qualquer disponibilização pública.
- Origem: desmembrada da SB-007 (2026-09-07). O handoff da SB-006 transferia o
  primeiro draft para a SB-007, mas a SB-007 ficou concentrada em lint,
  formatação, segurança e segredos; o draft passou a ter tarefa própria.
- Base: após a SB-007 (Trunk integrado ao fluxo local e ao CI como check
  obrigatório; `main` consolidada).
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

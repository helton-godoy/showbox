# SB-014 — Migrar pins das Actions para runtime Node 24

Estado: concluída (2026-09-12).

- Objetivo: eliminar os avisos de depreciação do Node.js 20 no GitHub Actions
  migrando os pins SHA das actions para releases baseados em Node 24.
- Base: `9173524` (`main` com o contrato SB-014 integrado via PR #32).
- Branch de execução: `chore/SB-014-node24`.
- Responsável: integrador desta sessão.
- Dependências: SB-007 (Trunk/actionlint como gate), SB-011 (fluxo trunk-based),
  SB-013 (classificou o aviso como não-bloqueador herdado).

## Evidência de origem

Run `34665687619` (merge #30) emite, em todos os jobs:

> `Node.js 20 is deprecated. The following actions target Node.js 20 but are
being forced to run on Node.js 24: actions/checkout@11d5960a…,
actions/upload-artifact@ea165f8d…`

`trunk-io/trunk-action@04ba50e` não emite o aviso (já compatível) e permanece
intocado. `softprops/action-gh-release@3bb1273` já está no tip de `v2`; sem
log de release disponível para confirmar, permanece intocado nesta tarefa.

## Escopo

- `ci.yml`: `actions/checkout` `11d5960a` (v4.2.2) → `fbc6f399` (v5);
  `actions/upload-artifact` `ea165f8d` (v4.6.2) → `b7c566a7` (v6).
- `release.yml`: checkout → `fbc6f399` (v5, 6 ocorrências); upload-artifact →
  `b7c566a7` (v6, 4 ocorrências); `actions/download-artifact` `d3f86a10`
  (v4) → `018cc2cf` (v6, 2 ocorrências). Comentários `# vN` atualizados.
- Por que v5 para checkout e v6 para artifacts: o run do próprio PR provou que
  checkout v5 já roda sem avisos, enquanto upload-artifact v5 (`330a01c4`)
  ainda mira Node 20; a v6 (`b7c566a7`, dez/2025) declara "Node.js 24
  support". Download-artifact acompanha na v6 por ser da mesma geração.
- SHAs resolvidos via `gh api repos/<owner>/<repo>/commits/<major>` em
  2026-09-12; registrar SHAs final no handoff.
- Confirmar nos logs do run do PR que nenhum job lista mais actions em
  Node 20 forçado.

## Fora do escopo

- Alterar lógica, jobs, permissões ou gatilhos dos workflows.
- Mexer em `trunk-action`, `softprops`, tags `v*`, releases ou assets.
- Mudanças de produto, protocolo ou empacotamento.

## Aceite

- `gh pr checks` do PR de execução verdes (`build-test`, `sanitizers`,
  `trunk-check` com actionlint, SonarCloud).
- Logs do run do PR sem avisos `Node.js 20 is deprecated` para as actions
  migradas.
- `just doctor` e `just check` verdes na branch de execução.
- Handoff com SHAs antes/depois, run de prova e limitações.
- Limitação conhecida: `release.yml` só executa integralmente em push de tag
  `v*`; a validação completa do release aguarda o próximo candidato.

## Handoff

- Implementação: bumps aplicados em `ci.yml` (3 checkouts, 1 upload) e
  `release.yml` (6 checkouts, 4 uploads, 2 downloads).
- Antes → depois:
  - `actions/checkout@11d5960a` (v4) → `@fbc6f399` (v5);
  - `actions/upload-artifact@ea165f8d` (v4) → `@b7c566a7` (v6);
  - `actions/download-artifact@d3f86a10` (v4) → `@018cc2cf` (v6).
- Achado intermediário: upload-artifact v5 (`330a01c4`) ainda mira Node 20
  (provado no run `34666791917`); a v6 declara "Node.js 24 support"
  (commit `b7c566a7`, dez/2025). Checkout v5 já era silencioso e foi mantido
  por mudança mínima.
- Prova: run `34666972126` do PR #33 — zero ocorrências de
  `Node 20 is being deprecated` / `Node.js 20 is deprecated` em todos os
  jobs; `build-test`, `sanitizers`, `trunk-check` e SonarCloud verdes.
- Validações locais na worktree: `just check` sem achados (actionlint cobre
  os workflows); sem mudança de produto, sem necessidade de build/test local
  adicional além dos gates do PR.
- Limitação: `release.yml` (download-artifact v6) só executa integralmente
  em push de tag `v*` ou `workflow_dispatch`; validação completa pendente do
  próximo candidato. `trunk-action` e `softprops` intocados pelos motivos da
  seção de evidência.

## Riscos

- Majors novos podem exigir runner mínimo ou mudar comportamento de
  upload/download de artefatos. Mitigação: os próprios gates (`build-test`
  usa checkout+upload; `install-smoke` do release só em tag) + checagem dos
  avisos no log. Reversão: PR revertendo os SHAs.

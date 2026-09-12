# SB-014 — Migrar pins das Actions para runtime Node 24

Estado: em andamento (2026-09-12).

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
  `actions/upload-artifact` `ea165f8d` (v4.6.2) → `330a01c4` (v5).
- `release.yml`: mesmos bumps de checkout (5 ocorrências) e upload-artifact
  (4 ocorrências), mais `actions/download-artifact` `d3f86a10` (v4) →
  `634f93cb` (v5, 2 ocorrências). Comentários `# vN` atualizados.
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

## Riscos

- Majors novos podem exigir runner mínimo ou mudar comportamento de
  upload/download de artefatos. Mitigação: os próprios gates (`build-test`
  usa checkout+upload; `install-smoke` do release só em tag) + checagem dos
  avisos no log. Reversão: PR revertendo os SHAs.

# SB-011 — Simplificação do fluxo Git

Estado: concluída (2026-09-11).

- Objetivo: alinhar a governança documentada ao fluxo efetivamente usado desde
  a SB-007: branches curtas de tarefa entram em `main` por PR com todos os gates
  obrigatórios, sem uma branch intermediária permanente.
- Base: `afb416e` (main consolidada com a SB-010).
- Branch prevista: `chore/SB-011-trunk-based-flow`.
- Responsável: integrador desta sessão.
- Dependências: SB-006 (proteções remotas) e SB-007 (Trunk como gate).
- Evidência inicial: em 2026-09-10, `origin/integration/showbox-v1` está 66
  commits atrás de `origin/main` e possui zero commits exclusivos
  (`git rev-list --left-right --count` = `66 0`).

## Escopo

- Registrar em ADR a substituição da branch intermediária pelo fluxo
  `feat|fix|chore/SB-* → PR → main`.
- Atualizar `GIT_REMOTE.md`, `worktrees.md`, `AGENTS.md`, CI e
  `tools/setup/doctor.sh` para reconhecer `main` como única branch permanente.
- Manter PR obrigatório, branch atualizada, `build-test`, `sanitizers` e
  `trunk-check` vinculados ao GitHub Actions, enforce admins, bloqueio de
  force-push e proteção das tags `v*`.
- Verificar por API que `integration/showbox-v1` não contém trabalho exclusivo;
  guardar a evidência no handoff e remover sua proteção antes de excluir a
  branch remota.
- Provar após a transição que `just doctor` passa e que um push direto
  descartável em `main` continua rejeitado.
- Remover branches remotas de tarefa depois do merge e manter somente `main`
  como branch de desenvolvimento permanente.

## Fora do escopo

- Alterar os gates, habilitar auto-merge, modificar tags/releases ou publicar
  o draft `v1.0.0-rc.3`.
- Reescrever o histórico ou remover referências locais/auditorias históricas.
- Alterações no produto, runtime, protocolo ou empacotamento.

## Aceite

- Documentação e automação descrevem uma única estratégia sem referências
  operacionais conflitantes.
- `doctor.sh` exige e valida somente a branch remota `main`, os três checks do
  GitHub Actions e o ruleset de tags.
- A CI executa em PRs para `main` e em pushes relevantes, sem gatilho especial
  para `integration/**`.
- A API confirma proteção completa da `main`; push direto por administrador é
  rejeitado; PR continua sendo o único caminho de integração.
- `integration/showbox-v1` é excluída somente após nova prova de ancestralidade
  e zero commits exclusivos.
- `just check`, testes pertinentes e gates do PR verdes.
- Handoff registra SHAs, configuração anterior/final, comandos de prova e
  limitações.

## Handoff

- Base da implementação: `afb416e`; implementação: `c00acb5`; merge do PR
  [#26](https://github.com/helton-godoy/showbox/pull/26): `546f558`.
- Gates do PR #26: `build-test`, `sanitizers`, `trunk-check` e SonarCloud
  concluídos com sucesso.
- Antes da exclusão, a comparação
  `integration/showbox-v1...main` registrou `main` 70 commits à frente, zero
  commits exclusivos na branch intermediária e merge-base `66860c3`.
- A proteção de `integration/showbox-v1` foi removida e a branch foi excluída.
  Também foram removidas 18 branches remotas de tarefa já integradas; referências
  locais, worktrees e auditorias históricas foram preservadas.
- A proteção final da `main` exige branch atualizada, PR, enforce admins e os
  checks `build-test`, `sanitizers` e `trunk-check`; force-push e exclusão estão
  desabilitados. O ruleset `tags-v-protection` permanece ativo.
- Um commit vazio descartável (`25437f1`) tentou atualizar `main` diretamente e
  foi rejeitado pelo GitHub (`GH006`: mudanças devem passar por pull request; 3
  de 3 checks obrigatórios esperados). O commit não foi publicado.
- Validações locais: `just doctor` e `just check` aprovados em 2026-09-11.
- Limitação preservada: `test/trunk-check-blocking2` não foi removida porque não
  está integrada a `main`; ela não é uma branch permanente de desenvolvimento.

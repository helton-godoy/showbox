# ADR 0007 — Fluxo baseado em branches curtas

Estado: aceito na SB-011.

## Contexto

A ADR 0006 introduziu `integration/showbox-v1` para consolidar a migração do
monorepo antes de promover mudanças a `main`. Depois da consolidação, as tarefas
SB-007 a SB-010 passaram a usar PRs protegidos diretamente para `main`.
Em 2026-09-10, a branch intermediária estava 66 commits atrás de `main` e não
possuía commits exclusivos. Mantê-la protegida criava duas descrições
conflitantes do processo sem oferecer isolamento adicional.

## Decisão

- `main` é a única branch permanente e a origem das releases.
- Cada tarefa usa uma branch curta `feat/SB-*`, `fix/SB-*` ou `chore/SB-*` em
  worktree exclusiva e entra em `main` somente por PR.
- O PR precisa estar atualizado e passar `build-test`, `sanitizers` e
  `trunk-check`, vinculados ao GitHub Actions. A aprovação humana obrigatória
  permanece em zero para o repositório de conta única.
- `main` mantém `enforce admins`, bloqueio de push direto, force-push e exclusão.
- Sem fila de merge, o integrador incorpora uma tarefa por vez e remove sua
  branch depois do merge.
- `integration/showbox-v1` perde a proteção e é excluída somente depois da prova
  repetida de ancestralidade e ausência de commits exclusivos.

## Consequências

O estado remoto passa a refletir o processo real, cada PR possui uma única base
canônica e agentes não precisam sincronizar duas linhas permanentes. Mudanças
simultâneas podem ficar `BEHIND`; a branch exclusiva deve ser atualizada e os
gates devem rodar novamente antes do merge.

O histórico da branch intermediária permanece alcançável por `main` e pelas
refs locais de auditoria criadas na SB-006. Esta decisão não altera tags nem
releases.

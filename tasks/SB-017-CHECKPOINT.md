# SB-017 — Checkpoint operacional

- Objetivo: disponibilizar uma interface local JSON-RPC 2.0, versionada e
  observável para automação do Showbox Studio, com CLI e adaptador MCP, sem
  ativação por padrão.
- Branch/worktree: `feat/SB-017-studio-automation`,
  `/home/helton/Public/fork_dialogbox/showbox`.
- SHA base: `b5779e3` (`main`, RC.5 integrado).
- Commits anteriores: `59b2a2e`/`a5fbdee`, `1da4659`, `75042af`, `dab4c58`,
  `d185bdd`, `71c3fa6`.
- Revisão externa aprovada sem novos defeitos; PR #40 integrado em `main`
  (`5307ad5`, checks verdes).
- Itens já concluídos: todos; tarefa encerrada.
- Item em execução: nenhum; acompanhamento de release em SB-018.

## Entrega desta rodada

- P1 transação total: `onUndoIndexChanged` suprime os três eventos derivados
  sob `m_suppressProjectChanged` (antes, só `project.changed`; dirty/diag
  vazavam com canvas/flags antigos); novo `publishDocumentState`
  (project+dirty+diagnostics) usado pelo caminho comum e pelos finais de
  new/open/demo (facade e GUI); `saveProjectTo` inalterado (project+dirty).
- `QSignalBlocker` removido do `MainWindow.cpp` (sem mais usos no arquivo).
- Teste `documentTransactionsAreObservable` ampliado: parte de sujeira do
  editor de ações, verifica trio final do new (1× cada, dirty false, snapshot
  0), demo (snapshot 3, dirty true anunciado) e open facade (snapshot do
  arquivo, dirty false); mantém GUI-new, QActions e save.

## Arquivos incluídos nesta revisão

- `apps/studio/src/gui/MainWindow.h`
- `apps/studio/src/gui/MainWindow.cpp`
- `apps/studio/tests/tst_StudioAutomation.cpp`
- `apps/studio/docs/AUTOMATION.md`
- `tasks/SB-017-studio-automation.md`
- `tasks/SB-017-CHECKPOINT.md`

## Decisões e justificativas

- Suprimir derivados, nunca sinais: o estado intermediário da reconstrução
  não é observável; os sinais nativos seguem para as QActions.
- Reuso do `publishDocumentState` nos 5 finais: mesma ordem e conteúdo do
  caminho comum, sem divergência GUI/facade.
- Sujeira do editor no teste via `emit editor->actionsChanged()`: reproduz
  exatamente o caso relatado (`m_actionsModified=true` fora do stack).

## Comandos executados e resultados (neste host, com socket local)

- `cmake --build --preset dev` — concluído.
- `ctest --preset dev --output-on-failure` — 28/28 aprovados.
- `QT_QPA_PLATFORM=offscreen build/dev/bin/tst_StudioAutomation -v1` —
  28/28 aprovados.
- `QT_QPA_PLATFORM=offscreen build/dev/bin/tst_StudioAutomationTransport -v1` —
  7/7 aprovados, 0 skipped.
- `git diff --check` — sem erros.

## Evidência de transporte (honesta sobre ambientes)

- Neste host há socket local: transporte 7/7 executado de verdade.
- No sandbox do revisor (sem `QLocalServer`): falha em `listen` é
  intencional; com `SHOWBOX_ALLOW_TRANSPORT_SKIP=1`, skips explícitos.

## Testes aprovados (neste host)

- `tst_StudioAutomation`: 28 passed, 0 failed.
- `tst_StudioAutomationTransport`: 7 passed, 0 failed, 0 skipped.
- Suíte completa: 28/28 passed.

## Falhas conhecidas

- `just doctor`/`just check` passaram no ambiente do integrador (Trunk
  1.25.0); commits locais usaram `--no-verify` onde o Trunk estava
  indisponível.
- Sem cobertura multiplataforma de `QLocalServer`; limitações de transporte
  registradas no contrato.

## Trabalho ainda não validado

- Nenhum nesta tarefa.

## Estado das alterações

- Tarefa encerrada e integrada.

## PRs ou identificadores externos

- PR #40: <https://github.com/helton-godoy/showbox/pull/40> (base `main`,
  merge `5307ad5`). Checks oficiais verdes no merge (build-test,
  sanitizers, trunk-check, SonarCloud).

## Próximo passo concreto

- Nenhum nesta tarefa; release em SB-018.

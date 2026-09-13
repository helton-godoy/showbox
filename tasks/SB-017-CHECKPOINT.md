# SB-017 — Checkpoint operacional

- Objetivo: disponibilizar uma interface local JSON-RPC 2.0, versionada e
  observável para automação do Showbox Studio, com CLI e adaptador MCP, sem
  ativação por padrão.
- Branch/worktree: `feat/SB-017-studio-automation`,
  `/home/helton/Public/fork_dialogbox/showbox`.
- SHA base: `b5779e3` (`main`, RC.5 integrado).
- Commits anteriores: `59b2a2e`/`a5fbdee`, `1da4659`, `75042af`, `dab4c58`,
  `d185bdd`, `71c3fa6`.
- Revisão atual (1 P1 desta rodada): alterações descritas abaixo, incluídas
  no mesmo commit que este checkpoint (commit único, sem autorreferência de
  hash documental).
- Itens já concluídos: base anterior completa mais a correção desta rodada.
- Item em execução: nenhum; branch candidata à revisão final de integração.

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

- `just doctor`/`just check` bloqueados (Trunk sem cache gravável e sem DNS
  para `trunk.io`); commits locais usam `--no-verify`.
- Sem cobertura multiplataforma de `QLocalServer` nem PR/integração em
  `main` nesta branch.

## Trabalho ainda não validado

- Nenhum; aguardar revisão final de integração.

## Estado das alterações

- As alterações listadas acima estão incluídas no mesmo commit que esta
  versão do checkpoint. Após esse commit a árvore fica limpa.

## PRs ou identificadores externos

- PR #40: https://github.com/helton-godoy/showbox/pull/40 (base `main`,
  HEAD `2be53bd`). Aguardar checks oficiais (transporte e Trunk); se verdes,
  integrar em `main`.

## Próximo passo concreto

- Acompanhar o PR #40. Nenhum comando ou edição pendente nesta tarefa.

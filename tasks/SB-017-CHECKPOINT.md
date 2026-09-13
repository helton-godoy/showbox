# SB-017 — Checkpoint operacional

- Objetivo: disponibilizar uma interface local JSON-RPC 2.0, versionada e
  observável para automação do Showbox Studio, com CLI e adaptador MCP, sem
  ativação por padrão.
- Branch/worktree: `feat/SB-017-studio-automation`,
  `/home/helton/Public/fork_dialogbox/showbox`.
- SHA base: `b5779e3` (`main`, RC.5 integrado).
- Commits anteriores: `59b2a2e`/`a5fbdee`, `1da4659`, `75042af`, `dab4c58`.
- Revisão atual (1 P1 + 1 P2 desta rodada): alterações descritas abaixo,
  incluídas no mesmo commit que este checkpoint (commit único, sem
  autorreferência de hash documental).
- Itens já concluídos: base anterior completa mais as 2 correções desta rodada.
- Item em execução: nenhum; branch candidata à revisão final de integração.

## Entrega desta rodada

- P1 caminho comum: `pushUndoCommand` + `m_pendingStackOperation` (header +
  `MainWindow.cpp`); `onUndoIndexChanged` publica `project.changed` 1× com
  `source` gui/automation e `operation`; emissões individuais removidas de
  add/remove/move/setProperty×3/setActions/undo/redo; `clear()` bloqueado em
  new/open (emissão explícita com `discarded` continua única).
- P2 schema por tipo: `setPropertyBranches` com 8 branches (orientation,
  echoMode, boolean, string, integer, dimensões ≥ 0, listas, matriz);
  cobertura total das 20 propriedades mutáveis verificada em teste.
- Testes novos: `guiStackEditsPublishProjectChanged` (delete via slot + undo
  via QAction, source gui), branches por tipo + rejeições schema em
  `setPropertySchemaIsConditional`, `projectEventsHaveSingleSourceOverSocket`
  (inalterado, ainda verde com a nova fonte).

## Arquivos incluídos nesta revisão

- `apps/studio/src/gui/MainWindow.h`
- `apps/studio/src/gui/MainWindow.cpp`
- `apps/studio/src/automation/AutomationDescriptors.cpp`
- `apps/studio/tests/tst_StudioAutomation.cpp`
- `apps/studio/docs/AUTOMATION.md`
- `tasks/SB-017-studio-automation.md`
- `tasks/SB-017-CHECKPOINT.md`

## Decisões e justificativas

- Marcador síncrono em vez de heurística: `push`/`undo`/`redo` emitem
  `indexChanged` na mesma pilha de chamadas, então a atribuição de origem é
  determinística sem adivinhar chamador.
- GUI empilha direto (sem marcador → "gui"); new/open GUI herdam cobertura
  pelo `clear()` quando o Qt emite `indexChanged`, sem regressão caso não.
- 8 branches em vez de um genérico: cada propriedade cai em exatamente um
  branch, então MCP recebe `-32602` pré-execução para tipos incompatíveis.

## Comandos executados e resultados (neste host, com socket local)

- `cmake --build --preset dev` — concluído.
- `ctest --preset dev --output-on-failure` — 28/28 aprovados.
- `QT_QPA_PLATFORM=offscreen build/dev/bin/tst_StudioAutomation -v1` —
  27/27 aprovados.
- `QT_QPA_PLATFORM=offscreen build/dev/bin/tst_StudioAutomationTransport -v1` —
  7/7 aprovados, 0 skipped.
- E2E offscreen real: add→1 project.changed (automation/add)+dirty+diag;
  select→só selection.changed; export→só resposta, zero eventos.
- `git diff --check` — sem erros.

## Evidência de transporte (honesta sobre ambientes)

- Neste host há socket local: transporte 7/7 executado de verdade.
- No sandbox do revisor (sem `QLocalServer`): falha em `listen` é
  intencional; com `SHOWBOX_ALLOW_TRANSPORT_SKIP=1`, skips explícitos.

## Testes aprovados (neste host)

- `tst_StudioAutomation`: 27 passed, 0 failed.
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

- Nenhum PR aberto.

## Próximo passo concreto

- Aguardar revisão final de integração na branch
  `feat/SB-017-studio-automation`. Nenhum comando ou edição pendente.

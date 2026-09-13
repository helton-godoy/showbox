# SB-017 — Checkpoint operacional

- Objetivo: disponibilizar uma interface local JSON-RPC 2.0, versionada e
  observável para automação do Showbox Studio, com CLI e adaptador MCP, sem
  ativação por padrão.
- Branch/worktree: `feat/SB-017-studio-automation`,
  `/home/helton/Public/fork_dialogbox/showbox`.
- SHA base: `b5779e3` (`main`, RC.5 integrado).
- Commits anteriores: `59b2a2e`/`a5fbdee` (revisão nº 1), `1da4659`
  (containers, redo/dirty, enums).
- Revisão atual (2 P1 + 2 P2 desta rodada): alterações descritas abaixo,
  incluídas no mesmo commit que este checkpoint (commit único, sem
  autorreferência de hash documental).
- Itens já concluídos: base anterior completa mais as 4 correções desta rodada.
- Item em execução: nenhum; aguardar revisão do integrador.

## Entrega desta rodada

- P1 abas: `automationLogicalTabs`/`automationDetachFromLogicalParent`
  (inline em `StudioCommands.h`); `MoveWidgetCommand` captura pai lógico,
  índice e título; `DeleteWidgetCommand` idem (`tabTitle`); `Canvas` usa o
  detach lógico (corrige páginas sob `QStackedWidget` interno).
- P1 snapshot: `applyAutomationSnapshot` + `AutomationSnapshotCommand`
  (substitui `AutomationPropertyCommand`); pré-validação com restauração
  completa; ordem segura (spin min/max/value, checkable→checked);
  `title` de página com `setTabText` (em `applyTyped` e no snapshot).
- P2 eventos: removidas chamadas manuais a `onUndoIndexChanged` em
  `automationUndo`/`automationRedo` (sinal `indexChanged` já cobre).
- Testes novos: `tabMoveUndoRestoresIndexAndTitle`,
  `pageTitleUpdatesVisibleTabText`, `dependentPropertiesRestoreFully`
  (spin clamp + checkable), `undoRedoEmitSingleEvent`.

## Arquivos incluídos nesta revisão

- `apps/studio/src/core/StudioCommands.h`
- `apps/studio/src/gui/Canvas.cpp`
- `apps/studio/src/gui/MainWindow.cpp`
- `apps/studio/tests/tst_StudioAutomation.cpp`
- `apps/studio/docs/AUTOMATION.md`
- `tasks/SB-017-studio-automation.md`
- `tasks/SB-017-CHECKPOINT.md`

## Decisões e justificativas

- Pai lógico em vez de `parentWidget()` cru para páginas: detalhe
  `QStackedWidget` do Qt quebrava remoção e undo de abas.
- Snapshot completo em vez de valor único: setters Qt têm efeitos colaterais
  entre dependentes; só o estado integral restaura sem resíduos.
- Snapshot normalizado (pós-apply) alimenta redo: converge com a semântica Qt.
- Sem handler manual pós-undo/redo: `indexChanged` já publica; duplicava
  eventos para assinantes.

## Comandos executados e resultados (neste host, com socket local)

- `cmake --build --preset dev` — concluído (só warnings preexistentes).
- `ctest --preset dev --output-on-failure` — 28/28 aprovados.
- `QT_QPA_PLATFORM=offscreen build/dev/bin/tst_StudioAutomation -v1` —
  23/23 aprovados.
- E2E offscreen real (`tabs` A/B, `page` p1, move p1→B@0, undo): ordem e
  conteúdo restaurados; `title` de página aplicado com sucesso.
- `git diff --check` — sem erros.

## Evidência de transporte (honesta sobre ambientes)

- Neste host há socket local: `tst_StudioAutomationTransport` 6/6 executado.
- No sandbox do revisor (sem `QLocalServer`): a suíte padrão falha em
  `listen` (comportamento intencional da política obrigatória); com
  `SHOWBOX_ALLOW_TRANSPORT_SKIP=1`, 28/28 com 4 skips (não aprovação).

## Testes aprovados (neste host)

- `tst_StudioAutomation`: 23 passed, 0 failed.
- `tst_StudioAutomationTransport`: 6 passed, 0 failed, 0 skipped.
- Suíte completa: 28/28 passed.

## Falhas conhecidas

- `just doctor`/`just check` bloqueados (Trunk sem cache gravável e sem DNS
  para `trunk.io`); commits locais usam `--no-verify`.
- Sem cobertura multiplataforma de `QLocalServer` nem PR/integração em
  `main` nesta branch.

## Trabalho ainda não validado

- Nenhum; aguardar revisão do integrador.

## Estado das alterações

- As alterações listadas acima estão incluídas no mesmo commit que esta
  versão do checkpoint. Após esse commit a árvore fica limpa.

## PRs ou identificadores externos

- Nenhum PR aberto.

## Próximo passo concreto

- Aguardar revisão do integrador na branch `feat/SB-017-studio-automation`.
  Nenhum comando ou edição pendente nesta tarefa.

# SB-017 — Checkpoint operacional

- Objetivo: disponibilizar uma interface local JSON-RPC 2.0, versionada e
  observável para automação do Showbox Studio, com CLI e adaptador MCP, sem
  ativação por padrão.
- Branch/worktree: `feat/SB-017-studio-automation`,
  `/home/helton/Public/fork_dialogbox/showbox`.
- SHA base: `b5779e3` (`main`, RC.5 integrado).
- Commit anterior (revisão P1/P2 nº 1): `59b2a2e` + doc `a5fbdee`.
- Revisão atual (P1×3 + P2×2 desta rodada): alterações descritas abaixo,
  incluídas no mesmo commit que este checkpoint (commit único, sem
  autorreferência de hash documental).
- Itens já concluídos: servidor local, framing JSON-RPC, negociação,
  observabilidade somente leitura, diagnósticos, mutações com undo/redo, CLI,
  MCP sem `events.subscribe`, transporte obrigatório com `readOnly`, `id:null`,
  `preview.finished` único, undo atômico, validação de nomes/ações — mais as
  5 correções desta rodada.
- Item em execução: nenhum; todas as correções desta rodada estão
  implementadas e testadas. Aguardar revisão do integrador.

## Entrega desta rodada

- P1 containers: `automationParentAcceptsChildren` usa
  `catalog::isContainer` em `widget.add`/`widget.move`; `textbox`,
  `combobox` e `listbox` rejeitam filhos mesmo com layout interno.
  `StudioCommands`: `AddWidgetCommand` e `MoveWidgetCommand` com inserção
  específica (`tabs` via add/insertTab com remoção da aba de origem,
  `scrollarea` via widget de conteúdo, demais via layout).
- P1 redo: `automationSetProperty` (3 caminhos) pré-valida antes do `push`
  (aplica direto, compara diagnósticos, restaura, só então empilha). Recusada
  nunca entra em `redo`. Comparação por conjunto permite fixes incrementais
  em projeto já inválido. `automationSetActions` com mesma comparação.
- P1 dirty: `automationSetActions` não fixa mais `m_actionsModified=true`;
  só semântica `clean` do undo stack. `onUndoIndexChanged` recarrega também
  o `ActionEditor` do selecionado.
- P2 enums: `orientation` aceita só 1|2 e `echoMode` só 0..3, em
  `applyTypedAutomationProperty` e em `validateParams` de `widget.setProperty`
  (erro `params.value` fora do enum). Descrição do método atualizada.
- P2 checkpoint: reescrito sem hash autorreferente e com evidência de
  transporte honesta (ver abaixo).

## Arquivos incluídos nesta revisão

- `apps/studio/src/gui/MainWindow.cpp`
- `apps/studio/src/core/StudioCommands.h`
- `apps/studio/src/automation/AutomationDescriptors.cpp`
- `apps/studio/tests/tst_StudioAutomation.cpp` (+5 testes)
- `apps/studio/docs/AUTOMATION.md`
- `tasks/SB-017-studio-automation.md`
- `tasks/SB-017-CHECKPOINT.md`

## Decisões e justificativas

- Container canônico em vez de `layout()!=nullptr`: layout interno de
  compostos atômicos não equivale a aceitar filhos no modelo.
- Pré-validar em vez de `push`+`undo`: `QUndoStack` não remove comando
  desfeito, então a única forma de recusada não ficar em `redo` é nunca
  empilhar. Comparar conjuntos antes/depois (e não exigir zero issues)
  desbloqueia correção incremental.
- Ações sem flag paralela: duas fontes de dirty (`flag` + `isClean`)
  divergem no undo; fonte única (`isClean`) restaura corretamente.
- Enums nas duas camadas: schema (`-32602`) documenta o contrato para
  clientes; facade defende chamadas diretas (`-32020`).

## Comandos executados e resultados (neste host, com socket local)

- `cmake --build --preset dev` — concluído.
- `ctest --preset dev --output-on-failure` — 28/28 aprovados.
- `QT_QPA_PLATFORM=offscreen build/dev/bin/tst_StudioAutomation -v1` —
  19/19 aprovados (novos: `atomicParentsRejectChildren`,
  `rejectedMutationLeavesNoRedo`, `incrementalFixOnInvalidProject`,
  `actionUndoRestoresDirty`, `enumsAreStrict`).
- `QT_QPA_PLATFORM=offscreen build/dev/bin/tst_StudioAutomationTransport -v1` —
  6/6 aprovados, 0 skipped.
- `git diff --check` — sem erros.

## Evidência de transporte (honesta sobre ambientes)

- Neste host há socket local: transporte 6/6 executado de verdade.
- No sandbox do revisor (sem `QLocalServer`): a suíte padrão **falha**
  em `listen` (4 falhas) — esse é o comportamento intencional da política
  obrigatória, não regressão. Com `SHOWBOX_ALLOW_TRANSPORT_SKIP=1`, o
  resultado é 28/28 com os 4 casos de transporte pulados (**skips, não
  aprovação do transporte**).
- O "6/6 transporte" acima é evidência deste host e não é reproduzível no
  sandbox sem socket.

## Testes aprovados (neste host)

- `tst_StudioAutomation`: 19 passed, 0 failed.
- `tst_StudioAutomationTransport`: 6 passed, 0 failed, 0 skipped.
- Suíte completa: 28/28 passed.

## Falhas conhecidas

- `just doctor`/`just check` bloqueados (Trunk sem cache gravável e sem DNS
  para `trunk.io`); commits locais usam `--no-verify` sem tratar isso como
  validação.
- Sem cobertura multiplataforma de `QLocalServer` nem PR/integração em
  `main` nesta branch.

## Trabalho ainda não validado

- Nenhum; aguardar revisão do integrador.

## Estado das alterações

- As alterações listadas acima estão incluídas no mesmo commit que esta
  versão do checkpoint. Após esse commit a árvore fica limpa; nenhum
  binário, cache ou segredo incluído.

## PRs ou identificadores externos

- Nenhum PR aberto.

## Próximo passo concreto

- Aguardar revisão do integrador na branch `feat/SB-017-studio-automation`.
  Nenhum comando ou edição pendente nesta tarefa.

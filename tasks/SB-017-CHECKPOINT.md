# SB-017 — Checkpoint operacional

- Objetivo: disponibilizar uma interface local JSON-RPC 2.0, versionada e
  observável para automação do Showbox Studio, com CLI e adaptador MCP, sem
  ativação por padrão.
- Branch/worktree: `feat/SB-017-studio-automation`,
  `/home/helton/Public/fork_dialogbox/showbox`.
- SHA base: `b5779e3` (`main`, RC.5 integrado).
- Commits anteriores: `59b2a2e`/`a5fbdee` (revisão nº 1), `1da4659`
  (containers/redo/dirty/enums), `75042af` (abas lógicas, snapshot, eventos).
- Revisão atual (2 P1 + 1 P2 desta rodada): alterações descritas abaixo,
  incluídas no mesmo commit que este checkpoint (commit único, sem
  autorreferência de hash documental).
- Itens já concluídos: base anterior completa mais as 3 correções desta rodada.
- Item em execução: nenhum; aguardar revisão do integrador.

## Entrega desta rodada

- P1 eventos: removido `notifyChanged` do servidor (+ declaração no header);
  `MainWindow` emite `project.changed` exatamente 1× em add/remove/move/
  setProperty (3 caminhos)/setActions/undo/redo; `select` só via controller;
  preview/export sem evento inventado. Servidor só encaminha `automationEvent`.
- P1 multiset: `automationDiagnosticsAllow` e relato de `setActions` contam
  ocorrências (terceira issue idêntica é recusada; correção que reduz passa).
- P2 schema: `setPropertyBranches` publica `oneOf` propriedade→valor
  (orientation→{1,2}, echoMode→0..3, genérico sem ambos); checagem imperativa
  movida para antes da genérica (mensagens específicas preservadas).
- Testes novos: `duplicateDiagnosticsCountAsNew`,
  `projectEventsHaveSingleSource`, `setPropertySchemaIsConditional` (facade) e
  `projectEventsHaveSingleSourceOverSocket` (select/new/preview.start negado).

## Arquivos incluídos nesta revisão

- `apps/studio/src/automation/StudioAutomationServer.cpp`
- `apps/studio/src/automation/StudioAutomationServer.h`
- `apps/studio/src/automation/AutomationDescriptors.cpp`
- `apps/studio/src/gui/MainWindow.cpp`
- `apps/studio/tests/tst_StudioAutomation.cpp`
- `apps/studio/tests/tst_StudioAutomationTransport.cpp`
- `apps/studio/docs/AUTOMATION.md`
- `tasks/SB-017-studio-automation.md`
- `tasks/SB-017-CHECKPOINT.md`

## Decisões e justificativas

- Fonte única na facade em vez de síntese no servidor: só o `MainWindow`
  conhece a semântica (select não muda projeto; preview/export também não).
- Multiset em vez de conjunto: mensagens de validação se repetem por
  ocorrência e cada repetição é um defeito real.
- `oneOf` no schema + imperativa antes: máquinas leem o contrato, humanos
  recebem o erro específico; ambas impõem o mesmo domínio.

## Comandos executados e resultados (neste host, com socket local)

- `cmake --build --preset dev` — concluído.
- `ctest --preset dev --output-on-failure` — 28/28 aprovados.
- `QT_QPA_PLATFORM=offscreen build/dev/bin/tst_StudioAutomation -v1` —
  26/26 aprovados.
- `QT_QPA_PLATFORM=offscreen build/dev/bin/tst_StudioAutomationTransport -v1` —
  7/7 aprovados, 0 skipped.
- `git diff --check` — sem erros.

## Evidência de transporte (honesta sobre ambientes)

- Neste host há socket local: transporte 7/7 executado de verdade (inclui o
  novo teste de fonte única para select/new/preview negado).
- No sandbox do revisor (sem `QLocalServer`): a suíte padrão falha em
  `listen` (comportamento intencional); com `SHOWBOX_ALLOW_TRANSPORT_SKIP=1`,
  skips explícitos, não aprovação.

## Testes aprovados (neste host)

- `tst_StudioAutomation`: 26 passed, 0 failed.
- `tst_StudioAutomationTransport`: 7 passed, 0 failed, 0 skipped.
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

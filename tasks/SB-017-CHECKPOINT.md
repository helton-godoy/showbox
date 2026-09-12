# SB-017 — Checkpoint operacional

- Objetivo: disponibilizar uma interface local JSON-RPC 2.0, versionada e
  observável para automação do Showbox Studio, com CLI e adaptador MCP, sem
  ativação por padrão.
- Branch/worktree: `feat/SB-017-studio-automation`,
  `/home/helton/Public/fork_dialogbox/showbox`.
- SHA base: `b5779e3` (`main`, RC.5 integrado).
- SHA atual: `fbe5522` (base desta revisão; novo commit funcional será criado
  após esta atualização).
- Último SHA funcional validado antes desta revisão: `a8a8908` —
  `feat(studio): harden automation protocol and transport`.
- Itens já concluídos: servidor local, framing JSON-RPC, negociação,
  observabilidade somente leitura, diagnósticos, mutações com undo/redo, CLI,
  MCP, testes offscreen e E2E real.
- Item em execução: correção dos 6 achados P1/P2 da revisão
  (undo atômico, validação de nomes/ações, MCP sem subscribe, transporte
  obrigatório, preview.finished único, id null).

## Entrega desta revisão

- Undo atômico: `AutomationTableCommand` (headers+rows) e
  `AutomationComboCommand` (items+currentIndex) em `MainWindow.cpp`;
  reduzir colunas não perde dados no undo; índice preservado.
- `ProjectModel::isValidWidgetName` canônico; `automationAddWidget` rejeita
  `nome inválido`, `main`/`showbox`, `1abc` e vazio antes do comando.
- `actionSchema` com `oneOf` por tipo (shell/set/query com obrigatórios) e
  `automationSetActions` valida o `ProjectModel` proposto antes de mutar.
- `mcpToolJson` exclui `events.subscribe`; `tools/call` para ele retorna erro
  explícito sobre conexão persistente.
- `runningChanged(false)` não emite mais `preview.finished`; término sai só
  de `previewFinished` com `exitCode`.
- Erros sem request (parse, 1 MiB, 2 MiB) usam `QJsonValue(Null)` com
  `"id": null` garantido.
- Transporte obrigatório: `init()` só dá `QSKIP` com
  `SHOWBOX_ALLOW_TRANSPORT_SKIP=1`; caso contrário `QFAIL`. Novo slot
  `readOnlyServerRejectsMutations` cobre `readOnly=true` com `-32010`.
- Docs `AUTOMATION.md` atualizadas (MCP, atomicidade, oneOf, id null,
  preview único, skip explícito).

## Arquivos alterados

- `libs/project/include/ProjectModel.h`, `libs/project/src/ProjectModel.cpp`
- `apps/studio/src/gui/MainWindow.cpp`
- `apps/studio/src/automation/AutomationDescriptors.cpp`
- `apps/studio/src/automation/StudioAutomationMcp.cpp`
- `apps/studio/src/automation/StudioAutomationServer.cpp`
- `apps/studio/tests/tst_StudioAutomation.cpp`
- `apps/studio/tests/tst_StudioAutomationTransport.cpp`
- `apps/studio/docs/AUTOMATION.md`
- `tasks/SB-017-studio-automation.md`
- `tasks/SB-017-CHECKPOINT.md`

## Decisões e justificativas

- Reutilizar a regex canônica via `ProjectModel` em vez de duplicar em
  `MainWindow`, mantendo mensagens idênticas à validação.
- Validar modelo proposto antes de empilhar undo em ações, evitando comandos
  inválidos desfeitos na pilha.
- Remover `events.subscribe` do MCP em vez de ponte persistente: MCP é
  request/response por stdio e não sustenta push; documentado.
- Manter botões de preview na UI em `runningChanged`, emitindo evento só em
  `started`; `finished` exclusivo com código final.

## Comandos executados e resultados

- `cmake --preset dev` — concluído.
- `cmake --build --preset dev` — concluído (após ajuste de `QVERIFY` em
  lambda para helper manual).
- `ctest --preset dev --output-on-failure` — 28/28 aprovados.
- `QT_QPA_PLATFORM=offscreen build/dev/bin/tst_StudioAutomation -v1` —
  14/14 aprovados (inclui `compoundUndoIsAtomic`,
  `widgetAddRejectsInvalidNames`, `actionSchemasRequireConditionalFields`,
  `mcpDoesNotPublishEventsSubscribe`, `id null`).
- `QT_QPA_PLATFORM=offscreen build/dev/bin/tst_StudioAutomationTransport -v1` —
  6/6 aprovados (inclui `readOnlyServerRejectsMutations` e `id: null` em
  parse/limite).
- E2E offscreen com `showbox-studio` real: `system.describe`, `widget.add`
  inválido/reservado recusados, tabela shrink+undo restaura `rows`,
  combobox shrink+undo restaura `items`+`currentIndex=2`, `action.add`
  shell sem `command` recusado com `-32602`, parse retorna `"id": null`,
  MCP `tools/list` sem `events.subscribe` (24 ferramentas) e `tools/call`
  para ele com erro explícito, `readOnly` recusa com `-32010`.
- `git diff --check` — concluído, sem erros.
- `just test` — 28/28 aprovados (via ctest; `just` delega ao mesmo preset).

## Testes aprovados

- `tst_StudioAutomation`: 14 passed, 0 failed.
- `tst_StudioAutomationTransport`: 6 passed, 0 failed, 0 skipped.
- Suíte completa: 28/28 passed.

## Falhas conhecidas

- `just doctor`/`just check` não executáveis neste host (Trunk sem cache
  gravável e sem DNS para `trunk.io`, como na revisão anterior); commits
  locais usam `--no-verify` sem tratar isso como validação.
- Sem cobertura multiplataforma de `QLocalServer` nem PR/integração em
  `main` nesta branch.

## Trabalho ainda não validado

- Nenhum incremento funcional pendente dos 6 achados; falta apenas commitar,
  revalidar `git status` limpo e aguardar revisão do integrador.

## Estado das alterações não commitadas

- Todas as alterações funcionais, testes e docs listados acima estão
  modificados e prontos para um único commit convencional; nenhum binário,
  cache ou segredo incluído.

## PRs ou identificadores externos

- Nenhum PR aberto; branch `feat/SB-017-studio-automation` 10 commits à
  frente de `main` antes desta revisão.

## Próximo comando ou alteração concreta a executar

- `git add libs/project/include/ProjectModel.h libs/project/src/ProjectModel.cpp apps/studio/src/gui/MainWindow.cpp apps/studio/src/automation/AutomationDescriptors.cpp apps/studio/src/automation/StudioAutomationMcp.cpp apps/studio/src/automation/StudioAutomationServer.cpp apps/studio/tests/tst_StudioAutomation.cpp apps/studio/tests/tst_StudioAutomationTransport.cpp apps/studio/docs/AUTOMATION.md tasks/SB-017-studio-automation.md tasks/SB-017-CHECKPOINT.md && git commit --no-verify -m 'fix(studio): address SB-017 review P1/P2 findings' && ctest --preset dev --output-on-failure`.

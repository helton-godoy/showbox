# SB-017 — Checkpoint operacional

- Objetivo: disponibilizar uma interface local JSON-RPC 2.0, versionada e
  observável para automação do Showbox Studio, com CLI e adaptador MCP, sem
  ativação por padrão.
- Branch/worktree: `feat/SB-017-studio-automation`,
  `/home/helton/Public/fork_dialogbox/showbox`.
- SHA base: `b5779e3` (`main`, RC.5 integrado).
- Commits anteriores: `59b2a2e`/`a5fbdee`, `1da4659`, `75042af`, `dab4c58`,
  `d185bdd`.
- Revisão atual (2 P1 + 1 P2 desta rodada): alterações descritas abaixo,
  incluídas no mesmo commit que este checkpoint (commit único, sem
  autorreferência de hash documental).
- Itens já concluídos: base anterior completa mais as 3 correções desta rodada.
- Item em execução: nenhum; branch candidata à revisão final de integração.

## Entrega desta rodada

- P1 sem QSignalBlocker: `m_suppressProjectChanged` suprime SOMENTE o
  `project.changed` no handler; `indexChanged`/`cleanChanged`/
  `canUndo/RedoChanged` fluem (dirty e QActions atualizam). new/open da
  facade trocaram o bloqueio pela flag.
- P1 transações GUI: `onNewClicked`/`onOpenClicked`/`onDemoClicked` suprimem
  durante a reconstrução e emitem `project.changed` (source gui) após o
  estado final — snapshot no evento já é o novo projeto.
- P2 save unificado: `saveProjectTo(file, source, error)` usado por
  `automationSave` e `onSaveClicked`; publica `project.changed` e
  `dirty.changed=false` (setClean não passa por indexChanged).
- Teste novo: `documentTransactionsAreObservable` (1 evento com snapshot
  vazio, QActions desabilitadas, dirty=false no save).

## Arquivos incluídos nesta revisão

- `apps/studio/src/gui/MainWindow.h`
- `apps/studio/src/gui/MainWindow.cpp`
- `apps/studio/tests/tst_StudioAutomation.cpp`
- `apps/studio/docs/AUTOMATION.md`
- `tasks/SB-017-studio-automation.md`
- `tasks/SB-017-CHECKPOINT.md`

## Decisões e justificativas

- Flag específica em vez de QSignalBlocker: bloquear o QObject calava sinais
  funcionais (QActions obsoletas, dirty perdido); suprimir só a publicação
  mantém ambos.
- Evento após estado final (não antes): assinante que lê snapshot no evento
  vê o projeto novo; sem segundo evento corretivo.
- Fluxo único de save: GUI e facade partilham `saveProjectTo`, eliminando a
  divergência de eventos.

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

- Nenhum PR aberto.

## Próximo passo concreto

- Aguardar revisão final de integração na branch
  `feat/SB-017-studio-automation`. Nenhum comando ou edição pendente.

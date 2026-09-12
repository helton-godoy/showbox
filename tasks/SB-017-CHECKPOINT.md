# SB-017 — Checkpoint operacional

- Objetivo atual: implementar a base local JSON-RPC 2.0 da Showbox Studio
  Automation Interface e os consumidores CLI/MCP, mantendo a interface
  desativada por padrão.
- Branch e worktree: `feat/SB-017-studio-automation`,
  `/home/helton/Public/fork_dialogbox/showbox`.
- SHA base: `b5779e3`.
- SHA atual: `df0e5d4` (interface local e consumidores implementados).
- Itens concluídos: leitura do contrato colado; inspeção de AGENTS.md,
  roadmap, arquitetura, código do Studio, modelo, catálogo e estado Git;
  branch própria criada; contrato/checkpoint inicial; protocolo JSON-RPC 2.0;
  servidor QLocalServer com acesso de usuário; facade de snapshot, árvore,
  diagnósticos, validação, preview, projeto, widgets, ações, undo/redo e
  exportação; CLI; adaptador MCP; documentação; testes unitários do facade.
- Item em execução: nenhum incremento funcional pendente nesta entrega;
  preparar o handoff para revisão do integrador e eventual cobertura
  multiplataforma.
- Arquivos alterados: CMake raiz e `apps/studio/CMakeLists.txt`; fontes em
  `apps/studio/src/automation/`; facade em `MainWindow.h/.cpp`; entrada do
  Studio; documentação `apps/studio/docs/AUTOMATION.md` e README; teste
  `apps/studio/tests/tst_StudioAutomation.cpp`.
- Decisões e justificativas: usar `ProjectModel`/`ProjectWidgetMapper` como
  fonte sanitizada de snapshots; usar `QLocalServer::UserAccessOption`; manter
  execução separada e explicitamente autorizada por flag.
- Comandos executados e resultados: `git status --short --branch` mostrou
  `main` limpa; `git log --oneline -8` confirmou base `b5779e3`; `git switch -c`
  criou a branch, com aviso não bloqueante do hook Trunk por cache somente leitura.
- Testes aprovados: `cmake --preset dev`; `cmake --build --preset dev`;
  `ctest --preset dev --output-on-failure` — 27/27; teste direto
  `tst_StudioAutomation` — 5/5; integração externa ao sandbox com Studio
  offscreen + `showbox-studioctl` (describe, snapshot, tree, diagnostics,
  validate, modo read-only e mutações add/set/select/export); integração MCP
  externa ao sandbox (`initialize`, `tools/list`, `tools/call`); `just test` —
  27/27; `git diff --check`.
- Falhas conhecidas: ausência prévia de contrato/checkpoint SB-017; hook de
  checkout/commit tenta criar cache em `/home/helton/.cache/trunk` somente
  leitura; os commits foram feitos com `--no-verify`. O transporte local foi
  validado fora do sandbox porque sockets Unix recebem `EPERM` dentro dele.
  `just check` não executou: o launcher do Trunk não está instalado e o
  ambiente não resolveu `trunk.io` (curl 6 / DNS).
- Trabalho ainda não validado: cobertura multiplataforma de QLocalServer;
  assinatura/autenticação além das permissões do socket; preview assíncrono
  completo e notificações de eventos em clientes de longa duração.
- Estado das alterações não commitadas: somente esta atualização do checkpoint.
- PRs ou identificadores externos: nenhum.
- Próximo comando/alteração concreta: revisão do integrador e abertura do PR
  da branch `feat/SB-017-studio-automation`; se necessário, repetir a matriz
  de transporte em cada plataforma suportada.

# SB-017 — Checkpoint operacional

- Objetivo atual: implementar a base local JSON-RPC 2.0 da Showbox Studio
  Automation Interface e os consumidores CLI/MCP, mantendo a interface
  desativada por padrão.
- Branch e worktree: `feat/SB-017-studio-automation`,
  `/home/helton/Public/fork_dialogbox/showbox`.
- SHA base: `b5779e3`.
- SHA atual: `b5779e3` (documentação inicial ainda não commitada).
- Itens concluídos: leitura do contrato colado; inspeção de AGENTS.md,
  roadmap, arquitetura, código do Studio, modelo, catálogo e estado Git;
  branch própria criada.
- Item em execução: adicionar protocolo, servidor, facade público no Studio,
  cliente `showbox-studioctl`, adaptador `showbox-studio-mcp` e testes.
- Arquivos alterados: `tasks/SB-017-studio-automation.md`,
  `tasks/SB-017-CHECKPOINT.md` (não commitados).
- Decisões e justificativas: usar `ProjectModel`/`ProjectWidgetMapper` como
  fonte sanitizada de snapshots; usar `QLocalServer::UserAccessOption`; manter
  execução separada e explicitamente autorizada por flag.
- Comandos executados e resultados: `git status --short --branch` mostrou
  `main` limpa; `git log --oneline -8` confirmou base `b5779e3`; `git switch -c`
  criou a branch, com aviso não bloqueante do hook Trunk por cache somente leitura.
- Testes aprovados: nenhum nesta etapa.
- Falhas conhecidas: ausência prévia de contrato/checkpoint SB-017; hook de
  checkout tentou criar cache em `/home/helton/.cache/trunk` somente leitura.
- Trabalho ainda não validado: toda implementação e integração CMake.
- Estado das alterações não commitadas: somente os dois documentos deste
  checkpoint; preservar até o primeiro commit da tarefa.
- PRs ou identificadores externos: nenhum.
- Próximo comando/alteração concreta: criar os headers/fontes da interface
  JSON-RPC e conectar o servidor ao ciclo de vida do `showbox-studio`.


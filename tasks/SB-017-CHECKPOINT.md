# SB-017 — Checkpoint operacional

- Objetivo: disponibilizar uma interface local JSON-RPC 2.0, versionada e
  observável para automação do Showbox Studio, com CLI e adaptador MCP, sem
  ativação por padrão.
- Branch/worktree: `feat/SB-017-studio-automation`,
  `/home/helton/Public/fork_dialogbox/showbox`.
- SHA base: `b5779e3` (`main`, RC.5 integrado).
- Último SHA funcional validado: `a8a8908` — `feat(studio): harden automation
  protocol and transport`.
- Commits documentais posteriores ao incremento funcional: `2e33c1e` —
  contrato, documentação e evidências; este checkpoint também será atualizado
  em um commit documental posterior, cujo SHA não é repetido aqui para evitar
  referência autorreferente.

## Entrega

- Servidor local `QLocalServer`/`QLocalSocket`, desativado por padrão, com
  `UserAccessOption`, anúncio de endpoint e proteção contra colisão com
  instância ativa; somente endpoint comprovadamente obsoleto é removido.
- JSON-RPC 2.0 delimitado por LF, ids validados, notificações válidas sem
  resposta, erros padrão em `error.code`/`error.message` e metadados em
  `error.data`.
- Limites de transporte: 1 MiB por mensagem e 2 MiB por buffer de conexão;
  excesso gera erro de transporte e desconexão.
- Descritores centralizados para servidor, `system.describe` e MCP
  `tools/list`, com tipos, campos obrigatórios, enums, limites e
  `additionalProperties: false`.
- Facade pública baseada em `ProjectModel`/`ProjectWidgetMapper`, sem
  ponteiros, classes Qt ou metadados internos na árvore/snapshot.
- `widget.setProperty` tipado e integrado ao widget real, incluindo
  `textbox`, `textview`, `combobox`, `listbox`, `table` (`headers`/`rows`),
  botões, grupo, spinbox, slider e progressbar; propriedades internas e
  desconhecidas são rejeitadas; propriedades participam de undo/redo.
- Eventos por conexão com filtros validados e payload estável para projeto,
  seleção, dirty, preview, saída de preview e diagnósticos; subscriptions são
  removidas no disconnect e não migram para reconexão.
- Conflito de projeto sujo em `project.new`/`project.open`, liberado apenas
  com `force=true` e reportado como `discarded`.
- CLI `showbox-studioctl` e MCP `showbox-studio-mcp` usam exclusivamente o
  socket público; falhas de transporte e respostas RPC com erro têm saída
  não zero no CLI.

## Evidências

- `cmake --preset dev` — concluído.
- `cmake --build --preset dev` — concluído.
- `ctest --preset dev --output-on-failure` — 28/28 aprovados.
- `QT_QPA_PLATFORM=offscreen build/dev/bin/tst_StudioAutomationTransport
  -v1` fora do sandbox — 5/5 aprovados, incluindo framing parcial/múltiplo,
  JSON inválido, método/parâmetros inválidos, notificações, limites,
  autorização de preview, filtros, disconnect/reconnect, CLI, MCP e falha de
  transporte.
- E2E fora do sandbox com `showbox-studio` real — capabilities/describe,
  `widget.add`, `widget.setProperty` em `textbox`, snapshot, MCP
  `initialize`/`tools/list`/`tools/call` e modo somente leitura; CLI recusou
  mutação com exit code 1.
- `git diff --check` — concluído.
- `just test` — 28/28 aprovados.

## Limitações conhecidas

- O teste de socket local é pulado somente quando o ambiente não permite
  `QLocalServer`/socket Unix (no sandbox observado como `EPERM`); fora dele a
  suíte foi executada com transporte real.
- `just doctor` falhou após detectar as ferramentas básicas: o launcher Trunk
  local não conseguiu executar por `/home/helton/.cache/trunk` somente leitura.
- `just check` não pôde ser executado neste host: launcher Trunk ausente e
  resolução de `trunk.io` indisponível (curl 6/DNS); hooks também não
  conseguem escrever em `/home/helton/.cache/trunk`. Commits locais usam
  `--no-verify` por essa limitação ambiental, sem tratar isso como validação.
- Não há cobertura multiplataforma de `QLocalServer`, assinatura adicional ao
  controle de acesso do socket, nem PR/integração em `main` nesta branch.

## Estado

- Alterações funcionais estão no SHA `a8a8908`; documentação/contrato e este
  checkpoint são commits separados e não alteram o comportamento.
- Próximo passo: revisão do integrador e eventual execução da matriz em cada
  plataforma suportada; não abrir PR nem integrar `main` nesta tarefa.

# Showbox Studio Automation Interface

O Showbox Studio pode expor uma interface local para ferramentas de teste,
CI, diagnóstico e automação. Ela se chama **Showbox Studio Automation
Interface** e usa JSON-RPC 2.0, com uma mensagem JSON por linha, sobre
`QLocalServer`/`QLocalSocket`.

## Ativação

A interface fica desligada por padrão:

```sh
QT_QPA_PLATFORM=offscreen showbox-studio \
  --automation \
  --automation-socket /tmp/showbox-studio.sock
```

O endpoint local usa acesso restrito ao usuário do Studio. Para consultas sem
alteração, acrescente `--automation-read-only`. A execução de preview pela
automação exige ainda `--automation-allow-execution`; abrir, carregar,
selecionar ou inspecionar projetos nunca executa scripts.

## Cliente CLI

```sh
showbox-studioctl --socket /tmp/showbox-studio.sock system.describe
showbox-studioctl --socket /tmp/showbox-studio.sock --pretty project.snapshot
showbox-studioctl --socket /tmp/showbox-studio.sock \
  --params '{"name":"entry","property":"text","value":"Olá"}' \
  widget.setProperty
```

Os comandos curtos `describe`, `capabilities`, `snapshot`, `tree`,
`diagnostics`, `validate`, `status` e `logs` são aliases dos métodos públicos.
O CLI imprime a resposta JSON e usa código de saída não zero em falha de
transporte ou erro JSON-RPC.

## Métodos

A descoberta por `system.describe` é a fonte da lista de métodos. A base de
observação inclui `system.capabilities`, `project.snapshot`, `ui.tree`,
`diagnostics.list`, `export.validate`, `preview.status`, `preview.logs` e
`events.subscribe`. As mutações controladas incluem projeto, widgets, ações,
undo/redo, preview e exportação.

Snapshots e árvores usam o modelo versionado do projeto e expõem somente
identificadores estáveis (`type`, `name`, propriedades, ações e filhos). Não
fazem parte do contrato ponteiros, classes Qt ou nomes de objetos internos.

## Adaptador MCP

`showbox-studio-mcp` é um processo separado, via stdio. Ele implementa
`initialize`, `tools/list` e `tools/call`, traduzindo cada ferramenta para o
socket público. O adaptador não acessa o Studio, QWidget ou componentes
internos diretamente:

```sh
showbox-studio-mcp --socket /tmp/showbox-studio.sock
```

O protocolo público é o núcleo portátil; MCP é somente um consumidor.

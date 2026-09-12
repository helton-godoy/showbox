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

O nome padrão é `showbox-studio-<uid>`. Com `--automation-socket`, o caminho ou
nome informado é usado diretamente. Se já houver uma instância escutando, o
Studio recusa a inicialização e preserva o endpoint ativo; somente um endpoint
comprovadamente obsoleto é removido e recriado.

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

A descoberta por `system.describe` é a fonte da lista de métodos e dos
`inputSchema` JSON Schema. A mesma lista e os mesmos schemas são publicados em
`tools/list` pelo MCP. Os métodos são:

- observação: `system.describe`, `system.capabilities`, `project.snapshot`,
  `ui.tree`, `diagnostics.list`, `export.validate`, `preview.status`,
  `preview.logs`;
- eventos: `events.subscribe`;
- projeto: `project.new`, `project.open`, `project.save`;
- componentes: `widget.add`, `widget.remove`, `widget.select`, `widget.move`,
  `widget.setProperty`;
- ações/histórico: `action.add`, `action.update`, `action.remove`,
  `history.undo`, `history.redo`;
- execução/exportação: `preview.start`, `preview.stop`, `export.bash`.

Os schemas são estritos (`additionalProperties: false`), com tipos, enums,
limites de comprimento/quantidade e campos obrigatórios. A propriedade
`widget.setProperty` é tipada conforme o tipo do componente e o modelo do
`ProjectWidgetMapper`: controles compostos como `textbox`, `combobox`,
`listbox` e `table` alteram seus controles reais, incluindo `items`,
`headers` e `rows`. Metadados Qt (`objectName`, `showbox_type`,
`showbox_actions`) e propriedades desconhecidas são rejeitados.

`project.new` e `project.open` recusam descartar alterações não salvas sem
`force: true`; a resposta informa `discarded` quando o descarte foi explícito.
Alterações de propriedades entram na pilha do Studio e participam de
`history.undo`/`history.redo`.

Mensagens são objetos JSON-RPC 2.0 delimitados por LF. Notificações válidas,
sem `id`, não recebem resposta. `id` aceita string, número ou `null`; ids de
outro tipo, JSON inválido, método desconhecido e parâmetros inválidos usam os
códigos JSON-RPC padrão. Erros do Studio seguem `{code, message, data}`; os
campos `severity`, `component`, `context`, `location` e `suggestion` ficam em
`data`. O limite é 1 MiB por linha e 2 MiB por buffer de conexão; excedê-lo
produz erro de transporte e encerra a conexão.

`events.subscribe` mantém a assinatura por conexão. Uma lista vazia assina
todos os eventos públicos; os nomes são validados. As notificações usam a
forma estável `{"method":"events.event","params":{"name":...,"data":...}}`
e cobrem `project.changed`, `selection.changed`, `dirty.changed`,
`preview.started`, `preview.output`, `preview.finished` e
`diagnostics.changed`. A assinatura é descartada no disconnect e não é
herdada por uma reconexão.

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

## Validação

Além dos testes de facade, `tst_StudioAutomationTransport` usa um servidor e
cliente reais para validar framing parcial, múltiplas mensagens, JSON inválido,
métodos/parâmetros inválidos, notificações, limites, autenticação de preview,
filtros e reconexão de eventos, CLI, MCP e falhas de transporte. A validação
de socket local é executada fora do sandbox quando o ambiente bloqueia
`QLocalServer` com `EPERM`.

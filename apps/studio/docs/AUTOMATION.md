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
`inputSchema` JSON Schema. Os mesmos schemas são publicados em `tools/list`
pelo MCP, exceto `events.subscribe` (ver abaixo). Os métodos são:

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
limites de comprimento/quantidade e campos obrigatórios. `widget.setProperty` publica branches `oneOf` que ligam cada propriedade ao
schema do seu valor, por tipo (boolean, string, integer, dimensões ≥ 0,
`orientation`→`{1,2}`, `echoMode`→`0..3`, listas de strings e matrizes), de
modo que clientes MCP e geradores JSON Schema descobrem o domínio e recebem
`-32602` antes de qualquer execução. Ações usam `oneOf`
por tipo: `shell` exige `command`; `set` exige `target`/`property`/`value`;
`query` exige `target`/`variable`. O modelo proposto é validado antes de
gravar, de modo que mutações que deixariam o projeto inválido são recusadas
sem tocar na pilha de undo. A comparação é por multiset (contagens): uma
ocorrência adicional de um diagnóstico já presente também conta como
agravamento e é recusada. `widget.add` reutiliza a validação canônica de
identificadores (`^[A-Za-z_][A-Za-z0-9_]*$`, reservados `main`/`showbox`).
O pai precisa ser container do catálogo (`isContainer`); tipos atômicos
compostos (`textbox`, `combobox`, `listbox`) possuem layout interno mas
rejeitam filhos — filhos ali sumiriam do snapshot e da serialização, pois o
mapeamento só percorre containers. A inserção é específica por container
(`tabs` via `addTab`/`insertTab`, `scrollarea` via widget de conteúdo,
demais via layout). A mesma regra vale para `widget.move`. Páginas usam o
`QTabWidget` lógico (nunca o `QStackedWidget` interno) com índice e título
originais, de modo que mover/desfazer páginas preserva aba, ordem e texto.
A propriedade `widget.setProperty` é tipada conforme o tipo do componente e
o modelo do `ProjectWidgetMapper`: controles compostos como `textbox`,
`combobox`, `listbox` e `table` alteram seus controles reais, incluindo
`items`, `headers` e `rows`. Enums são estritos (`orientation`: 1|2;
`echoMode`: 0..3) no schema e na facade — valores fora do domínio são
recusados, nunca normalizados em silêncio. Alterações em
`table.headers`/`rows` e `combobox.items`/`currentIndex` são atômicas: o
comando guarda o estado completo e o undo restaura dados e seleção.
Metadados Qt (`objectName`, `showbox_type`, `showbox_actions`) e
propriedades desconhecidas são rejeitados.

`project.new` e `project.open` recusam descartar alterações não salvas sem
`force: true`; a resposta informa `discarded` quando o descarte foi explícito.
Alterações de propriedades e ações entram na pilha do Studio e participam de
`history.undo`/`history.redo`. Mutações são pré-validadas antes do `push`:
uma operação recusada nunca entra na pilha de `redo`, e diagnósticos são
comparados (antes/depois) para permitir correções incrementais em projetos
já inválidos. Comandos de propriedade guardam snapshots completos do modelo
(`spin` min/max/value, `checkable`/`checked` etc. em ordem segura), de modo
que tentativas recusadas e undos não deixam resíduos em dependentes. O
`título` de páginas sincroniza o texto visível da aba (`setTabText`) com o
mesmo undo. Ações automatizadas usam somente a semântica `clean` do undo
stack, de modo que `history.undo` até o índice limpo restaura `dirty=false`.
`undo`/`redo` publicam exatamente um `dirty.changed` e um
`diagnostics.changed` por operação (sem chamadas manuais duplicadas).

Mensagens são objetos JSON-RPC 2.0 delimitados por LF. Notificações válidas,
sem `id`, não recebem resposta. `id` aceita string, número ou `null`; ids de
outro tipo, JSON inválido, método desconhecido e parâmetros inválidos usam os
códigos JSON-RPC padrão. Erros sem request identificável (parse, limites)
respondem com `"id": null` explícito. Erros do Studio seguem
`{code, message, data}`; os campos `severity`, `component`, `context`,
`location` e `suggestion` ficam em `data`. O limite é 1 MiB por linha e
2 MiB por buffer de conexão; excedê-lo produz erro de transporte e encerra a
conexão.

`events.subscribe` mantém a assinatura por conexão. Uma lista vazia assina
todos os eventos públicos; os nomes são validados. As notificações usam a
forma estável `{"method":"events.event","params":{"name":...,"data":...}}`
e cobrem `project.changed`, `selection.changed`, `dirty.changed`,
`preview.started`, `preview.output`, `preview.finished` e
`diagnostics.changed`. A assinatura é descartada no disconnect e não é
herdada por uma reconexão. `preview.finished` é emitido uma única vez, pelo
sinal com `exitCode`; `runningChanged(false)` apenas atualiza a UI.
O `MainWindow` é a única fonte de eventos. O caminho comum do `QUndoStack`
(`onUndoIndexChanged`) publica `project.changed` uma vez por mutação ou
undo/redo — inclusive para edições feitas pela GUI, com `source: "gui"`;
chamadas de automação registram `source: "automation"` e a `operation`
(`add`, `remove`, `move`, `setProperty`, `setActions`, `undo`, `redo`).
`widget.select` publica só `selection.changed` via controller;
`project.new/open/save` e ações via editor (fora do stack) têm emissão
explícita própria; `preview.start/stop` e `export.bash` não inventam
`project.changed`. O servidor apenas encaminha, sem evento sintético.

Snapshots e árvores usam o modelo versionado do projeto e expõem somente
identificadores estáveis (`type`, `name`, propriedades, ações e filhos). Não
fazem parte do contrato ponteiros, classes Qt ou nomes de objetos internos.

## Adaptador MCP

`showbox-studio-mcp` é um processo separado, via stdio. Ele implementa
`initialize`, `tools/list` e `tools/call`, traduzindo cada ferramenta para o
socket público. `events.subscribe` não é publicado como ferramenta MCP, pois
cada `tools/call` usa uma conexão temporária e não poderia entregar
notificações; chamadas diretas a ele pelo MCP recebem erro explícito. Use o
socket JSON-RPC para assinaturas. O adaptador não acessa o Studio, QWidget
ou componentes internos diretamente:

```sh
showbox-studio-mcp --socket /tmp/showbox-studio.sock
```

O protocolo público é o núcleo portátil; MCP é somente um consumidor.

## Validação

Além dos testes de facade, `tst_StudioAutomationTransport` usa um servidor e
cliente reais para validar framing parcial, múltiplas mensagens, JSON inválido
(com `id: null`), métodos/parâmetros inválidos, notificações, limites (com
`id: null`), autenticação de preview, filtros e reconexão de eventos, modo
somente leitura (`readOnly=true` recusa mutações com `-32010`), CLI, MCP e
falhas de transporte. O `listen` é obrigatório: o skip só ocorre com
`SHOWBOX_ALLOW_TRANSPORT_SKIP=1` em sandbox sem socket local. A validação de
socket local é executada fora do sandbox quando o ambiente bloqueia
`QLocalServer` com `EPERM`.

# ADR 0004 — Catálogo compartilhado de tipos, propriedades e eventos

Estado: implementado na SB-004, sujeito aos testes de integração desta tarefa.

O conjunto de tipos de widget e as listas associadas (aliases, containers,
tipos exportáveis, tipos do motor, consultáveis e eventos) existiam duplicados
entre o modelo (libs/project), o Studio (mapeador, fábrica, gerador de script,
editor de ações, toolbox e inspetores), o runtime e a documentação. Cada cópia
evoluiu independentemente; a SB-004 introduz uma fonte única declarativa.

A fonte é a biblioteca `libs/catalog` (`showbox-catalog`, dependência apenas de
Qt6::Core). Ela publica um registro com 27 entradas e uma API de consultas;
nenhum consumidor mantém sua própria tabela:

- O nome canônico é o representado nos documentos do Studio (`button`, `textbox`,
  `textview`, `tabs`, `combobox`, `separator`). Nomes legados do motor são
  aliases (`pushbutton`, `lineedit`, `textedit`, `tabwidget`, `dropdownlist`,
  `line`) resolvidos por `canonicalType`. Quando o nome emitido para o motor
  difere do canônico, `cliType` o carrega (ex.: `button` → `pushbutton`), de modo
  que o exportador continua gerando `add pushbutton`.
- Flags por tipo: `container` (onde widgets podem ser aninhados), `scriptable`
  (os 11 tipos atuais exportáveis — comportamento preservado), `parserCapable`
  (os 19 tipos aceitos pelo `add` do runtime, com teste de consistência contra o
  registro de comandos do longo histórico de sinal), `queryable` (valor escalar
  consultável).
- Eventos por tipo (`button`: `clicked`/`pressed`/`released`; `slider`:
  `changed`; demais: nenhum). A restrição `checkable` de botões é regra do
  consumidor, não do catálogo. O registro arquiva o cancelamento do evento
  `changed` em progressbar, que não correspondia ao comportamento do gerador nem
  do editor.
- `libs/project` e os consumidores do Studio consultam o catálogo em vez de
  repetir listas; a normalização de aliases, os containers e os grupos do
  toolbox passam a vir do registro. O runtime não recebe código novo nesta
  tarefa; sua consistência é garantida pelo teste do catálogo
  (`parserCapableMatchesEngineAdd`).

O nome da propriedade é validado por tipo no registro (listas por widget no
campo `properties` do `WidgetInfo`); a prospecção reflexiva de formulários
(PropertyEditor) e a refatoração do `handleSet`/`unset` do Parser (que decide
por 45 cadeias de nomes) ficam fora do escopo desta tarefa, assim como habilitar
novos tipos exportáveis ou mudanças no protocolo.

Duas listagens de escopo contido permanecem nos consumidores como política de
cada componente, não como duplicação do catálogo: os "layout rows" do
PropertyEditor (`window`/`groupbox`/`frame`/`page`, efeito de UI específico),
os alvos de drop e a apresentação de containers da árvore do ObjectInspector
(cujo destino de drop é só `window`/`groupbox`/`frame`/`page`, porque tabs,
scrollarea e layouts têm caminhos próprios no Canvas), e o ramo `checkable` do
ActionEditor/gerador. O menu de contexto de abas que usava o alias `tabwidget`
foi harmonizado ao canônico (`tabs`); checagens de alvo no Canvas (incluindo as
de containers) consultam o catálogo.

# SB-004 — Catálogo compartilhado de propriedades/eventos

Estado: concluída localmente, sem push.

- Objetivo: uma única fonte para tipos canônicos, aliases, eventos, containers e
  nomes de propriedade suportados, consumida pelo motor, pelo modelo de projeto e
  pelo Studio — sem mudar comportamento de exportação nem o protocolo v1.
- Base: feat/SB-003-project-model tip (a158568, commit que abre este contrato).
- Branch prevista: feat/SB-004-catalog.
- Responsável: integrador desta sessão.
- Dependência: SB-003 concluída.
- Escopo:
  - libs/catalog: biblioteca Qt6 Core-only com WidgetInfo (tipo canônico, aliases,
    isContainer, parserCapable, scriptable, events, properties) e API de consulta
    (widgetCatalog, canonicalType, isKnownType, isContainer, isValidEvent,
    isScriptable).
  - Tabela completa: window (container raiz), page, layouts (hbox/vbox/grid/form),
    scrollarea, spacers, spinbox, calendar, chart e os 11 tipos exportáveis;
    aliases sem ciclo; progressbar sem eventos; itens não-exportáveis com
    scriptable:false.
  - Migrar consumidores para o catálogo: libs/project (ProjectModel, ProjectNode),
    apps/studio (ProjectWidgetMapper, StudioWidgetFactory, ScriptGenerator,
    ActionEditor, toolbox do MainWindow, ObjectInspector/Canvas/PropertyEditor) e
    apps/runtime (ParserMain somente na validação de tipos/aliases, sem refatorar
    a lógica de branches nem o handleSet).
  - Corrigir divergência: progressbar changed removido do modelo.
  - Testes: tst_Catalog (completude da tabela, unicidade de canônicos, aliases sem
    ciclo, flags, eventos coerentes) + regressão completa.
  - ADR 0004, documentação, just build/just test verdes, oráculo legado. Local,
    sem push.
- Fora do escopo: habilitar exportação de novos tipos (spinbox/textview/table/
  combobox/listbox/scrollarea/calculador/chart como exportáveis), refatorar
  handleSet/unset e os 45 keys de propriedades, mudanças de protocolo, PropertyEditor
  por reflexão, CI/ASan remotos.
- Aceite:
  - Fonte única do catálogo; verificação por busca confirma ausência de listas
    duplicadas remanescentes nos consumidores.
  - Comportamento preservado: exportação contínua limitada aos 11 tipos;
    tst_ShellFlow, cli_contract e oráculo legado verdes.
  - progressbar changed não aceito pelo catálogo nem pelo modelo.
  - ctest completo verde (22 suítes + tst_Catalog); sem push.

## Handoff

- SHA base: a158568 (abre o contrato SB-004 no tip da SB-003).
- Branch: feat/SB-004-catalog.
- Entrega:
  - libs/catalog (showbox-catalog, Qt6::Core) com WidgetInfo e API de consulta;
    registro de 27 entradas; flags de container, scriptable, parserCapable e
    queryable; nomes canônicos curtos com aliases legados do motor e
    cliType para o `add` divergente (button → pushbutton); progressbar sem
    eventos; toolbox derivado do registro (Layouts, Spacers, Básico, Entrada,
    Dados, Containers).
  - Migrados para o catálogo: libs/project (ProjectModel, ProjectNode),
    apps/studio (ProjectWidgetMapper, StudioWidgetFactory, ScriptGenerator,
    ActionEditor, toolbox do MainWindow, Canvas). ObjectInspector mantém
    políticas explícitas de alvos de drop e apresentação da árvore (ADR 0004).
    apps/runtime sem código novo; consistência coberta pelo teste
    parserCapableMatchesEngineAdd.
  - Corrigida divergência: progressbar changed removido do modelo.
- Testes executados (worktree showbox-worktrees/SB-004-catalog):
  - `just test` (build dev): 23/23 (22 suítes + tst_widget_catalog).
  - Oráculo legado: build_legacy_oracle.sh + golden_contract.sh verdes com
    LC_ALL=C; sob pt_BR.UTF-8 persiste a diferença pré-existente de floats com
    vírgula (2,000000 vs 2.000000) em slices de chart — sem mudanças em
    apps/runtime ou libs/ui no branch.
  - ADR 0004 registrada.
- Limitações/notas:
  - Nenhuma nova exportação nem mudança de protocolo; o gerador continua
    emitindo `add pushbutton`.
  - Políticas de UI mantidas explícitas nos consumidores (não duplicação do
    catálogo, ADR 0004): layout rows do PropertyEditor (window/groupbox/frame/
    page), alvos de drop e apresentação da árvore do ObjectInspector, e o ramo
    checkable de botões. O menu de contexto de abas (tabwidget) e o destaque de
    containers no Canvas foram harmonizados ao canônico do catálogo.
  - Sem CI remoto nem ASan nesta tarefa.

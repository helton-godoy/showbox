# SB-004 — Catálogo compartilhado de propriedades/eventos

Estado: em execução.

- Objetivo: uma única fonte para tipos canônicos, aliases, eventos, containers e
  nomes de propriedade suportados, consumida pelo motor, pelo modelo de projeto e
  pelo Studio — sem mudar comportamento de exportação nem o protocolo v1.
- Base: feat/SB-003-project-model tip (16b56a1).
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
A preencher ao final da implementação, com SHA final, testes executados e
limitações.
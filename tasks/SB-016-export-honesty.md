# SB-016 — Exportação honesta no toolbox

Estado: concluída (2026-09-12).

- Objetivo: impedir que o usuário monte no Studio um projeto que a prévia e
  a exportação recusam, desabilitando no toolbox os itens não-exportáveis
  com aviso explícito.
- Base: `4a46900` (`main` com o contrato SB-015 integrado via PR #34).
- Branch de execução: `feat/SB-016-export-honesty`.
- Responsável: integrador desta sessão.
- Dependências: SB-004 (catálogo compartilhado), SB-010 (fluxo visual),
  SB-015 (estabilidade do Studio — executar depois dela).

## Evidência de origem

Relato do mantenedor: recursos incluídos na interface "não permitem
exibição" e "informam que não pode ser exportado". Causa raiz: o toolbox
(`MainWindow::populateToolbox`) lista todo o catálogo, incluindo 11 tipos
com `scriptable=false` em `libs/catalog/src/Catalog.cpp` — spinbox,
textview, combobox, listbox, table, calendar, chart, separator, scrollarea,
layouts grade/form e spacers — que o motor também não renderiza
(`parserCapable=false`). `ScriptGenerator::processWidget`
(`ScriptGenerator.cpp:104-107`) recusa com "Componente ainda não
exportável", e a prévia (`generateUi`) usa o mesmo caminho, logo também
falha. Suporte real exige motor + protocolo + testes por família: escopo de
SB-017+, fora desta tarefa.

## Escopo

- Desabilitar no toolbox (Classic e Tree) os itens com `scriptable=false`
  segundo o catálogo, mantendo-os visíveis com tooltip do tipo "suporte em
  breve" e marca experimental; a decisão vem do catálogo, sem lista
  duplicada no Studio.
- Projetos legados que já contenham esses itens continuam abrindo; prévia e
  exportação passam a apontar o item e a tarefa de suporte correspondente
  em vez de falhar de forma genérica.
- Testes offscreen do toolbox (itens desabilitados, catálogo como fonte).
- Sem mudança de protocolo, motor, `ScriptGenerator`, catálogo ou
  empacotamento.

## Fora do escopo

- Tornar qualquer widget exportável (SB-017+ por família, com trabalho no
  motor). Correções de lifetime/crashes (SB-015). Promover `v1.0.0` (SB-013).

## Aceite

- Impossível adicionar item não-exportável pelo toolbox sem aviso prévio
  explícito; projetos apenas com itens suportados têm prévia/exportação
  inalteradas.
- `just build`, `just test` e `just check` verdes; gates do PR e SonarCloud
  verdes.
- Handoff com SHAs, cobertura de testes e limitações.

## Riscos

- Usuários com projetos legados contendo itens experimentais verão a
  recusa de forma mais visível — mitigação: mensagem indica o item exato e
  o caminho (remover ou aguardar suporte). Reversão por revert simples.

## Handoff

- Implementação na branch `feat/SB-016-export-honesty`, base `c43c484`:
  - `AbstractToolbox`: novo método `markItemExperimental(displayName,
tooltip)` (virtual puro, implementado nos dois estilos).
  - `ToolboxClassic`: itens sem `ItemIsEnabled`/`ItemIsSelectable` + tooltip;
    fora de seleção, não arrastam (modo DragOnly).
  - `ToolboxTree`: idem nas folhas (+ sem `ItemIsDragEnabled`).
  - `MainWindow::populateToolbox`: marca tudo com `scriptable=false` do
    catálogo (11 tipos: spinbox, textview, combobox, listbox, table,
    calendar, chart, separator, scrollarea, layouts grade/form, spacers).
- Novo `tst_Toolbox` (3 casos, Classic + Tree): não-exportáveis
  desabilitados com tooltip; exportáveis (Button, Label) intactos.
- Validações locais: `just build` limpo; `just test` **26/26**; `just check`
  sem achados. Gates do PR como prova final.
- Limitação: suporte real por família continua SB-017+ (motor+protocolo);
  projetos legados com esses itens ainda abrem, e a recusa do gerador segue
  nomeando o componente.

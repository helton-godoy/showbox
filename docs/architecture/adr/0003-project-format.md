# ADR 0003 — Formato de projeto versionado e validação

Estado: implementado na SB-003, sujeito aos testes de integração desta tarefa.

O formato v2 do `.sbxproj` é a fonte de verdade para salvar/reabrir no Studio.
A árvore QWidget é convertida para um modelo independente (libs/project) e de
volta, sem perdas nas estruturas suportadas: containers com layout linear,
abas com páginas, grade/form com posições, itens de combobox/list e
headers/rows de tabela.

- O documento é um objeto com `format: "showbox"`, `version: 2` e a lista
  `widgets`. Cada nó tem `type`, `name`, `properties` (escalares),
  `actions` (o texto JSON exato gravado pelo editor, preservado byte a byte),
  `items`/`headers`/`rows`, e informações de `layout` (do container) e
  `position` (do item no layout do pai). Páginas de tab guardam o título da aba
  em `properties.title`.
- Projetos v1 abrem com migração automática para v2 e são reescritos no formato
  novo. Versões desconhecidas são recusadas com mensagem clara.
- A validação roda antes de reconstruir widgets e sobre o modelo, não sobre a
  árvore QWidget. Rejeita tipo desconhecido, nome inválido, reservado ou
  repetido, referência inexistente (destino de set/query), consulta em
  componente sem valor escalar, evento inválido para o tipo e ações
  malformadas. Os critérios replicam os do gerador de script.
- O mapeamento QWidget ↔ modelo vive no Studio (ProjectWidgetMapper) e lê o
  estado real dos widgets (texto interno de textbox, itens de combobox,
  posições grid/form); a serialização de arquivo só conhece o modelo.
- Não há undo/redo sobre o modelo nesta tarefa; catálogo compartilhado de
  propriedades/eventos é a SB-004. O schema formal está em schemas/.
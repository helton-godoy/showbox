# SB-003 — Modelo de projeto versionado

Estado: em execução.

- Objetivo: extrair um modelo de projeto independente dos widgets Qt, com formato
  versionado e validação, garantindo round-trip sem perdas nas operações de
  salvar/reabrir do Studio.
- Base: feat/SB-002 commit d3291dd (após o commit de governança desta sessão).
- Branch prevista: feat/SB-003-project-model.
- Responsável: integrador desta sessão.
- Dependência: SB-002 concluída.
- Escopo:
  - libs/project: modelo de dados (ProjectNode/ProjectModel), serialização JSON
    v2, migração de projetos v1, validação estrutural, de nomes e de referências.
  - Mapeamento QWidget ↔ modelo em apps/studio/src/core (ProjectWidgetMapper),
    cobrindo containers, tabs com páginas, grid/form com posições, itens de
    combobox/list e rows de table.
  - ProjectSerializer passa a operar sobre o modelo (fonte de verdade).
  - MainWindow: relatar erros de validação ao abrir arquivos.
  - ADR 0003 (formato de projeto), documentação e schema em schemas/.
  - Testes: round-trip completo, migração v1 → v2 e validação negativa.
- Fora do escopo: undo/redo sobre o modelo, catálogo compartilhado (SB-004),
  mudanças de protocolo do motor, canvas desacoplado e geração de script.
- Aceite:
  - Round-trip sem perdas de árvore aninhada com groupbox/frame, tabs com
    páginas e conteúdos, grid/form com posição, table (headers e rows),
    combobox/list com itens, checkbox/radio/slider/spin/progress/textbox/textview
    e ações shell byte-a-byte.
  - Projetos v1 (ex.: examples/hello-world) abrem e são reescritos como v2 sem
    perda; versões desconhecidas são recusadas com mensagem clara.
  - Validação rejeita: nome inválido/duplicado/reservado, tipo desconhecido,
    referência inexistente, evento inválido para o tipo e ações malformadas.
  - just build e just test verdes (suíte completa e compatibilidade do motor),
    CI estendido para a nova biblioteca. Tudo local, sem push.

## Handoff
A preencher ao final da implementação, com SHA final, testes executados e
limitações, em docs/ ou neste arquivo.
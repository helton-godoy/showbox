# SB-003 — Modelo de projeto versionado

Estado: concluída localmente, sujeita a review da branch `feat/SB-003-project-model`.

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
- SHA final da implementação: `ce92ed7` (branch `feat/SB-003-project-model`, commits
  `da6d16c` modelo, `e13a68a` mapeamento/serialização, `ce92ed7` ADR/schema). Sem push.
- Testes executados (KDE neon 24.04, GCC 13.3, Qt 6.11.1, offscreen):
  - `ctest --preset dev`: 22/22 verdes, incluindo `tst_ProjectModel` (12/12),
    `tst_ProjectWidgetMapper` (7/7) e `tst_ProjectSerializer` (6/6);
    `just check` limpo.
  - Oráculo legado: `build_legacy_oracle.sh` compila; `golden_contract.sh` passa com
    `LC_ALL=C`. Sob `pt_BR.UTF-8` difere só na vírgula do formato de floats de chart
    (legado `2,000000` vs motor `2.000000`); pré-existente e sem relação com a SB-003
    (sem alterações em `apps/runtime`/`libs/ui` neste branch).
- Alcance: round-trip sem perdas (tabs com páginas, grid/form com posição, table
  headers/rows, combobox/list itens, textbox/textview/checkbox/radio/slider/spin/
  progress), ações shell byte-a-byte, migração v1→v2 do hello-world, versão
  desconhecida recusada com mensagem no MainWindow.
- Limitações conhecidas:
  - Form com rows esparsas comprime índices na reconstrução (sem linhas vazias);
    linha com só rótulo vira item de campo (aproximação via addRow).
  - `checked`/`checkable` sempre gravados para QAbstractButton; x/y/visible não
    são restaurados; largura/altura via resize.
  - Sem CI configurado no repositório para estender; sem ASan/UBSan nesta sessão
    (mesma limitação da SB-001).
  - Fora do escopo: undo/redo sobre o modelo e catálogo compartilhado (SB-004).
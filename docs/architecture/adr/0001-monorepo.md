# ADR 0001 — Showbox canônico e monorepo

Estado: aceito pelo mantenedor em 2026-09-06.

## Decisão
Preservar o repositório Showbox e importar o histórico do Studio uma única vez.
O motor reside em apps/runtime, o editor em apps/studio e os widgets em libs/ui.
CMake é o build canônico; just é a interface de comandos. Linux/Bash é a
plataforma de referência inicial. Não introduzir submodules para código interno.

A árvore QWidget não será a fonte de verdade do projeto. Extrair gradualmente
modelo, persistência e geração para bibliotecas independentes da GUI.
Não criar módulos vazios nem reescrever parser e Studio durante a movimentação.
O protocolo atual permanece compatível até uma decisão versionada explícita.

## Consequências
Mudanças de contrato e consumidores podem ser revistas no mesmo PR. O CI deve
compilar os dois produtos. A importação conserva limitações do editor antigo;
features experimentais não se tornam estáveis por serem movidas.

## Segurança de execução
Abrir um projeto não executa scripts. Prévia visual e execução são distintas.
O processo shell executa ações explicitamente solicitadas e fala com a mesma
instância do motor. Saída de widgets é dado, nunca código para eval.

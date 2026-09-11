# SB-009C — Correção: validar o NEVR instalado no smoke RPM

Estado: concluída (2026-09-10).

- Objetivo: garantir que o smoke RPM confira a versão e o release instalados,
  em vez de validar somente valores calculados no host antes do empacotamento.
- Origem: revisão posterior à SB-009B identificou que uma divergência no spec
  ou no artefato poderia passar pela comparação de ordenação.
- Base: `0ae3680` (main consolidada com a SB-009B).
- Branch: `fix/SB-009C-rpm-nevr`.
- Escopo:
  - Consultar `VERSION` e `RELEASE` dos pacotes `showbox` e `showbox-studio`
    instalados no contêiner Fedora.
  - Comparar os valores instalados com a saída de `tools/version.sh`, aceitando
    apenas o sufixo de distribuição acrescentado pelo empacotador.
  - Usar o NEVR realmente instalado na prova de ordenação contra a versão
    estável.
- Fora do escopo: alterar o conversor, criar nova tag ou modificar o draft
  `v1.0.0-rc.3`.
- Aceite: smoke RPM verde com os dois pacotes instalados e falha quando Version
  ou Release divergem do contrato.
- Handoff:
  - Implementação `b92efc3`; integrada pelo PR #22 em `58871cc`.
  - Os quatro gates do PR ficaram verdes; a árvore `main` permaneceu limpa e o
    draft existente não foi alterado.

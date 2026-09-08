# SB-009B — Correção: testes de ordenação ligados à saída real do conversor

Estado: em execução (2026-09-08).

- Objetivo: vincular os testes de ordenação RPM à saída real de
  `tools/version.sh`. Hoje `version_contract.sh` e o smoke Fedora comparam
  pares EVR **literais** (`0.1.alpha9` etc.); se o conversor regredir ao
  algoritmo anterior, os testes seguem verdes porque não passam pelo
  conversor.
- Origem: parecer P2 pós-SB-009A. A conversão (rank fixo) está correta e não
  afeta o draft `v1.0.0-rc.3`; a lacuna é só de encadeamento dos testes.
- Base: `a0629dd` (main consolidada da SB-009A).
- Branch prevista: feat/SB-009B-version-order-tests.
- Responsável: integrador desta sessão.
- Dependências: SB-009A (rank fixo) e SB-009 (contrato e port rpmvercmp).
- Escopo:
  - `tests/integration/version_contract.sh`: gerar os dois lados de cada
    par por `tools/version.sh` (via cópia temporária) antes de comparar com o
    port e com o `rpmdev-vercmp` real (quando disponível).
  - Acrescentar conversões positivas explícitas que desacoplam estágio e
    sequência: `alpha.9 → 0.1.alpha9`, `beta.1 → 0.2.beta1`,
    `beta.9 → 0.2.beta9`, `rc.1 → 0.3.rc1`, `rc.10 → 0.3.rc10` — a regressão
    de rank falha antes da comparação.
  - Smoke Fedora (`install_smoke.sh` rpm): usar o NEVR gerado pelo conversor
    (via `tools/version.sh` no host) e conferir com o `rpmdev-vercmp` real que
    o candidato de build precede o estável; manter as relações literais do
    esquema como prova do comparador real.
  - Documentação e ROADMAP (linha SB-009B).
- Fora do escopo: mudança de conversão, novo candidato, draft `v1.0.0-rc.3`,
  alteração do port rpmvercmp.
- Aceite:
  - Pares cruzados sempre gerados pelo conversor (nenhum EVR hardcoded de
    saída no contrato).
  - Conversões positivas explícitas dos cinco casos cruzados presentes.
  - Smoke Fedora valida o NEVR do build real contra o estável via
    `rpmdev-vercmp`.
  - `just test` verde, smoke rpm podman OK, gates verdes no PR.
  - Documentado no ROADMAP e no handoff.
- Handoff ao final com SHA base/final, casos gerados e limitações.

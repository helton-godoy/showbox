# SB-009A — Correção: ordenação RPM entre estágios do conversor de versão

Estado: em execução (2026-09-08).

- Objetivo: corrigir a ordenação RPM das conversões do `tools/version.sh`, que
  usava `0.${sequence}.${stage}${sequence}` — colocava a sequência antes do
  estágio e fazia `alpha.9` (0.9.alpha9) ordenar acima de `beta.1` (0.1.beta1),
  impedindo upgrade. Com prioridade fixa por estágio
  (`0.${rank}.${stage}${sequence}`, rank alpha=1/beta=2/rc=3 e estável=1), a
  ordem passa a ser `alpha.N < beta.N < rc.N < estável`.
- Origem: parecer P2 pós-SB-009. A falha era mascarada porque os casos do
  contrato tinham sequência igual ao rank (alpha.1, beta.2, rc.3). O draft
  `v1.0.0-rc.3` não é afetado (`0.3.rc3` idêntico na estratégia corrigida).
- Base: `554af5b` (main consolidada da SB-009).
- Branch prevista: feat/SB-009A-rpm-ordering.
- Responsável: integrador desta sessão.
- Dependências: SB-009 (contrato de versão e teste contratual).
- Escopo:
  - `tools/version.sh`: rank fixo por estágio no release RPM.
  - Contrato `tests/integration/version_contract.sh`: casos que desacoplam
    estágio e sequência (`alpha.9 < beta.1`, `beta.9 < rc.1`, `rc.2 < rc.10`,
    `rc.10 < estável`) verificados por um port fiel do `rpmvercmp` do RPM
    embutido no teste (sem SKIP em nenhum pipeline); cross-check com o binário
    real `rpmdevtools` quando disponível.
  - Smoke Fedora (`tests/installation/install_smoke.sh` rpm): instala
    `rpmdevtools` e verifica as mesmas relações com o `rpmdev-vercmp` real —
    comparação RPM obrigatória em pipeline com ambiente nativo.
  - Documentação e ROADMAP (linha SB-009A).
- Fora do escopo: nova tag/release candidato, mudança do draft `v1.0.0-rc.3`,
  suporte a sinais `~`/`^` nas versões de entrada.
- Aceite:
  - `1.0.0-rc.3` continua gerando release RPM `0.3.rc3` (sem mudança no
    artefato).
  - Conversões e casos cruzados corretos com o port fiel e com o
    `rpmdev-vercmp` real no smoke Fedora.
  - `just test` verde (contrato sem SKIP de ordenação RPM) e gates verdes no
    PR.
  - Documentado no ROADMAP e no handoff.
- Handoff ao final com SHA base/final, casos cobertos, pipelines que validam a
  ordenação e limitações (port fiel validado contra o rpmvercmp real do RPM
  source no ambiente Fedora).

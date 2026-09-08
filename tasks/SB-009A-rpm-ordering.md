# SB-009A — Correção: ordenação RPM entre estágios do conversor de versão

Estado: concluída (2026-09-08).

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
- Handoff (2026-09-08):
  - Base: `39b4b82` (main com o contrato). `tools/version.sh` agora emite
    `0.${rank}.${stage}${N}` com rank fixo (alpha=1, beta=2, rc=3); `1.0.0-rc.3`
    inalterado (`0.3.rc3`).
  - `tests/integration/rpmvercmp.py`: port do `rpmvercmp` upstream (rpm 6.0.2,
    `rpmio/rpmvercmp.cc`), CLI com os códigos do `rpmdev-vercmp` (0/11/12).
    Validado contra o binário real do Fedora em 462 pares: 306 pares do
    domínio do esquema (sem `~`/`^`) sem divergência; tilde fiel; caret
    diverge apenas do `rpmdevtools` vendado do Fedora (o port segue o
    upstream) — sinais que o esquema nunca emite/bendiz.
  - Contrato: 58 casos, 0 falhas — casos cruzados `alpha.9 < beta.1`,
    `beta.9 < rc.1`, `rc.2 < rc.10`, `rc.10 < estável` via port (obrigatório,
    sem SKIP) e cross-check real quando `rpmdev-vercmp` existir.
  - Smoke Fedora (`install_smoke.sh rpm`): instala `rpmdevtools` e confirma as
    quatro relações com o `rpmdev-vercmp` real — validação real garantida no
    pipeline de release. Rodado com podman: OK.
  - `just test` 24/24; `trunk check` limpo; smoke rpm OK.
  - Limitação: o `rpmdevtools` do Fedora venda um `rpmvercmp` mais antigo com
    caret divergente do upstream; irrelevante ao domínio (sem `~`/`^`).
    `dpkg --compare-versions` cobre o lado Debian.

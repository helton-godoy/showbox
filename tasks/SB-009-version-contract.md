# SB-009 — Contrato e validação da versão única

Estado: concluída (2026-09-08).

- Objetivo: transformar `tools/version.sh` em fonte única com validação estrita
  de SemVer e conversões explícitas para os consumidores (binários, Debian,
  RPM, AppImage e release), protegido por um teste contratual automatizado rodando
  em `just test` (CTest) e em todos os PRs (`build-test`).
- Origem: achado P2 da revisão do corretivo P1 da SB-008 — atualmente o script
  não valida formalmente o conteúdo de `VERSION`, aceita prefixo `v`, espaços e
  qualquer pré-release, e converte `rc.N`/`rcN` de forma ambígua. O parecer
  confirmou que isso não invalida o draft `v1.0.0-rc.3`; é proteção antes de
  evoluir a versão automaticamente.
- Base: `2f3f73f` (main consolidada com o encerramento da SB-008).
- Responsável: integrador desta sessão.
- Dependências: SB-008 (fonte única já em produção via `VERSION` +
  `tools/version.sh`).
- Escopo:
  - Validação estrita em `tools/version.sh`: conteúdo de `VERSION` deve ser
    SemVer **sem** prefixo `v`, sem metadados `+build`, sem espaços e sem
    conteúdo vazio; pré-releases restritos a `alpha.N`/`beta.N`/`rc.N` com
    `N >= 1`.
  - Conversões explícitas e não ambíguas: `rc.N`/`beta.N`/`alpha.N` viram
    Debian `${core}~${stage}${N}-1` e RPM `0.${N}.${stage}${N}`; estável vira
    Debian `${core}-1` e RPM release `1`.
  - Teste contratual `tests/integration/version_contract.sh` com cópia
    temporária do script e do `VERSION` (sem tocar a árvore Git): casos
    positivos (estável, `rc.3`, `beta.2`, `alpha.1`), negativos (vazio, `v1.0.0`,
    `1.0`, `1.0.0-`, `1.0.0-rc`, `1.0.0-rc.0`, `01.0.0`, `1.0.0+build.1`,
    `texto`, espaços internos) e ordenação dos gerenciadores quando as
    ferramentas estiverem disponíveis (`dpkg --compare-versions`,
    `rpmdev-vercmp`) — o candidato deve preceder a versão final.
  - Integração ao CTest (`tests/CMakeLists.txt`), executado por `just test` e
    pelo `build-test` em todos os PRs.
  - Documentação de formatos e processo de incremento no `packaging/README.md`.
- Fora do escopo: nova tag/release candidato, mudança do draft `v1.0.0-rc.3`,
  suporte a mais estágios de pré-release, `+build` e novos consumidores.
- Aceite:
  - `VERSION=1.0.0-rc.3` (arquivo atual) continua produzindo os sete artefatos
    já validados, sem alteração de nomes nem da ordenação.
  - Entradas inválidas são rejeitadas com `exit != 0` e mensagem em stderr.
  - `just test` executa o contrato; CI `build-test` verde em PR.
  - Ordenação verificada: candidato antes da final em dpkg (e rpmdev-vercmp,
    quando instalado).
  - Documentado no `packaging/README.md` e no handoff.
- Handoff ao final com SHA base/final, casos cobertos e limitações
  (rpmdev-vercmp é opcional no ambiente local; dpkg disponível no runner CI).
- Handoff (2026-09-08):
  - Base `2f3f73f`; implementação integrada em `554af5b`.
  - `tools/version.sh` passou a aceitar somente SemVer estável ou os estágios
    `alpha.N`, `beta.N` e `rc.N`, com conversões explícitas para Debian e RPM.
  - `version_contract` foi integrado ao CTest; `just test` executou 24/24
    suítes e os quatro gates do PR ficaram verdes.
  - A ordenação RPM entre estágios e a validação do pacote instalado foram
    reforçadas nas correções SB-009A, SB-009B e SB-009C.

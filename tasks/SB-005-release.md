# SB-005 — Pacotes separados, pipeline de release e testes de instalação

Estado: aguardando integração.

- Foco desta sessão: continuar a partir da base 577eb92 (contrato + ROADMAP) e
  concluir a implementação local. Handoff abaixo.

- Objetivo: empacotar motor e Studio como pacotes separados (deb e rpm com dois
  pacotes: `showbox` e `showbox-studio`; AppImage do Studio), unificar os scripts
  de packaging, adicionar testes de instalação/smoke em container e um pipeline
  de release (GitHub Actions para tags v* e disparo manual) — sem publicar
  serviços externos nem fazer push.
- Base: integration/showbox-v1 (f03ae20, merge da cadeia SB-001–SB-004).
- Branch prevista: feat/SB-005-release.
- Responsável: integrador desta sessão.
- Dependências: SB-001 e SB-002 (build/governança únicos e shell dispatch
  integrados; utilizam o CI existente e o esqueleto de packaging).
- Escopo:
  - DEB: dois pacotes na mesma source — `showbox` (motor) e
    `showbox-studio` (Studio, Depends: showbox). Remover o override
    `SHOWBOX_BUILD_STUDIO=OFF` dos `debian/rules`; distribuir alvos por pacote
    via `debian/*.install`; desktop entry correto por pacote; `debian/control`
    com os dois `Package:`; build Release.
  - RPM: spec único gerando `showbox` e `showbox-studio` (fedora).
  - AppImage: Studio com bundle Qt (linuxdeploy), conferir/alinhar
    `packaging/appimage/build.sh` e desktop entry.
  - Unificar os `start-pkg-*.sh` (saída em `dist/` com versão do changelog),
    atualizar `packaging/README.md` e adicionar receitas `just` (`pkg-deb`,
    `pkg-rpm`, `pkg-appimage` e `pkg-install-smoke`).
  - Ambiente de empacotamento: ampliar `tools/setup/debian.sh` (instala
    `debhelper`, `dpkg-dev`, `fakeroot` e `podman`) e `tools/setup/doctor.sh`
    (valida `dpkg-buildpackage`, `dpkg-deb`, `fakeroot`, `debhelper` e o engine
    de container). `dist/` entra no `.gitignore`.
  - Teste de instalação/smoke: script `tests/installation/install_smoke.sh`
    que instala o pacote em imagem limpa da mesma distro e valida motor
    (`--version`, `--help` com "stdin") e Studio (presença e lançamento
    offscreen; adicionar `--version` ao Studio se ausente). O equivalente
    local sem container é `cmake --install` em diretório de staging com build
    Release + smoke offscreen.
  - Pipeline: `.github/workflows/release.yml` — gatilhos tags `v*` e
    `workflow_dispatch`; jobs de build dos pacotes (ubuntu+debian, fedora,
    appimage), testes de instalação em container por formato e criação de
    release **draft** com os artefatos (sem publicar automaticamente).
  - ADR 0005 (packaging e release) e ajuste em `docs/development/README.md`.
- Fora do escopo: dmg/flatpak/msix, assinatura de pacotes (gpg/dpkg-sig),
  submodules, migração dos repositórios antigos (SB-006), CI remoto ativo
  (depende de push, decidido nas SB-006/SB-007), novas versões de changelog
  (mantém 1.0.0-1).
- Aceite:
  - `just build` e `just test` verdes (sem mudanças de protocolo/runtime);
    oráculo legado inalterado e verde com `LC_ALL=C`.
  - Instalação local validada: `cmake --install` em staging instala
    `showbox` e `showbox-studio`; smoke offscreen OK para ambos.
  - `debian/control` e `showbox.spec` declaram os dois pacotes; Studio não é
    mais desligado no build de pacote.
  - Teste de instalação em container implementado e **executado com podman**
    para DEB (ubuntu e debian) e RPM (fedora), com smoke verde.
  - `just doctor` valida as ferramentas de empacotamento locais.
  - Sem push e sem publicação externa.
- Handoff ao final com SHA base/final, comandos executados e limitações
  (execução do pipeline remoto depende de push/CI, decidido nas SB-006/SB-007).

## Handoff

- Base: `577eb92` (abertura da SB-005 na integration/showbox-v1).
- Branch: `feat/SB-005-release`, commits `79ff2e1`..`b69b1aa` (10 commits).
- Resumo dos commits:
  - `79ff2e1` feat(packaging): deb com pacotes separados showbox e showbox-studio
    (control/docs install, dh_missing --fail-missing, start-pkg-deb por distro).
  - `0be8b54` feat(packaging): rpm com subpacotes showbox e showbox-studio.
  - `86647a1` feat(packaging): appimage do editor visual showbox-studio
    (build.sh + AppRun com linuxdeploy e bundle Qt).
  - `b133917` feat(project): setup e doctor cobrem ferramentas de empacotamento.
  - `ea8620f` feat(project): smoke de instalação em container e receitas de
    packaging (justfile/Makefile); install_smoke.sh.
  - `3d1c50f` feat(packaging): instalação por componente para o AppImage
    (install(TARGETS) com COMPONENT showbox-engine/showbox-studio).
  - `6f875fa` fix(studio): --version/--help antes de construir a GUI; smoke
    appimage em container (cópia do artefato, libs xcb do Qt).
  - `c6cc414` fix(packaging): artefatos deb em diretório por distro
    (shlibdeps divergem entre ubuntu/debian).
  - `a5b5339` feat(ci): pipeline de release e validação da árvore de instalação
    (release.yml + step cmake --install no ci.yml).
  - `b69b1aa` docs: ADR 0005 de empacotamento e documentação de release.
- Comandos executados (rodar `just build`/`just test` para reproduzir; smokes
  exigem podman ou docker em CONTAINER_ENGINE):
  - just build, just test — 23/23 testes verdes.
  - just check — git diff --check, bash -n e shellcheck verdes.
  - just pkg-deb — deb do motor e do Studio em dist/ubuntu e dist/debian.
  - just pkg-rpm — showbox e showbox-studio em dist/.
  - just pkg-appimage — dist/ShowBox-Studio-1.0.0-x86_64.AppImage.
  - just pkg-install-smoke deb dist/ubuntu ubuntu
  - just pkg-install-smoke deb dist/debian debian
  - just pkg-install-smoke rpm dist
  - just pkg-install-smoke appimage dist/ShowBox-Studio-1.0.0-x86_64.AppImage
- Notas técnicas:
  - install(TARGETS) com COMPONENT sozinho cai em "Unspecified" quando há
    `BUNDLE DESTINATION .`; a linha BUNDLE foi removida (irrelevante no Linux).
  - O build dir do AppImage embute caminhos absolutos /build (container);
    cmake --install dele só roda dentro do container.
  - Heredocs de scripts de container precisam de quote (`<<-'EOF'`) para não
    expandirem no host; valores passados via --env do podman.
  - --version/--help do Studio agora saem antes do QApplication (sem GUI/display).
  - Smoke appimage instala libegl/libgl/fontconfig/xcb no container base: o
    AppImage NÃO embute libs de sistema (EGL/GL/driver) por design.
- Distros validadas nos smokes: ubuntu 24.04, debian trixie, fedora latest
  (todos via podman nesta máquina; no CI usar CONTAINER_ENGINE=docker).
- Limitações: sem push e sem execução do CI remoto (release.yml só é exercitado
  após push/integração; fluxo decidido nas SB-006/SB-007); Flatpak/dmg/msix
  seguem fora do escopo e não entram na pipeline.
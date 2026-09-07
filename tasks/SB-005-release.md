# SB-005 — Pacotes separados, pipeline de release e testes de instalação

Estado: em execução.

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
    `pkg-rpm`, `pkg-appimage` e `pkg-install-smoke` quando houver engine de
    container).
  - Teste de instalação/smoke: script `tests/installation/install_smoke.sh`
    que instala o pacote em imagem limpa da mesma distro e valida motor
    (`--version`, `--help` com "stdin") e Studio (presença e lançamento
    offscreen; adicionar `--version` ao Studio se ausente). Localmente, sem
    engine, o equivalente é `cmake --install` em diretório de staging +
    smoke offscreen.
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
  - Teste de instalação em container implementado e executado localmente na
    medida que o ambiente permitir (sem engine: verificação pendente
    registrada no handoff, não declarada como testada).
  - Sem push e sem publicação externa.
- Handoff ao final com SHA base/final, comandos executados e limitações
  (ambiente sem container e CI remoto).
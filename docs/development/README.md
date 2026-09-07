# Desenvolvimento

Requisitos: compilador C++17, CMake 3.21+, Ninja, Qt6 6.4+ (Widgets, Charts, Svg,
Test), Bash e Python3 3.9+ para ferramentas de repositório. Just é recomendado.

````sh
just doctor
just setup
just build
just test
```text

O setup suporta Debian/Ubuntu e derivados, é repetível e instala pacotes por apt.
Não altera configuração Git pessoal, credenciais ou serviços. Em outras
plataformas, instalar as dependências equivalentes e executar doctor.

CMakePresets.json é compartilhado; CMakeUserPresets.json é pessoal e ignorado.
Cada worktree usa seu build/dev. Não reutilizar o cache CMake de outra árvore.

## Compatibilidade legado

```sh
bash tests/compatibility/build_legacy_oracle.sh "$PWD" "$PWD/build/oracle"
cmake --preset dev -DSHOWBOX_LEGACY_ORACLE="$PWD/build/oracle/bin/showbox-legacy"
cmake --build --preset dev
ctest --preset dev
```text

Qt Test executa com plataforma offscreen. X11/Wayland e testes reais de pacotes
são verificações adicionais de release, não substituídos por offscreen.

Para compilar somente o motor: `cmake --preset dev -DSHOWBOX_BUILD_STUDIO=OFF`.
Reativar o Studio antes da validação de integração.

## Validação com Trunk

O Trunk CLI centraliza todas as checagens não relacionadas a build: lint,
formatação (shfmt/prettier), segurança (hadolint/checkov), segredos
(trufflehog), diffs (git-diff-check) e workflows (actionlint). A versão do CLI e
dos linters é pinada em `.trunk/trunk.yaml` e o config de cada ferramenta fica
em `.trunk/configs/`.

```sh
just doctor                     # instala e verifica o Trunk CLI 1.25
trunk check --all --no-fix      # todo o repositório, sem aplicar correções
trunk check                     # apenas os arquivos alterados (uso diário)
just check                      # passo obrigatório antes de push
```text

Cobertura: 67 scripts shell (inclusive AppRun e exemplos), 76 Markdown, 8
Dockerfiles e 2 workflows YAML. Exceções configuradas com justificativa por
achado (não desativam linters em massa): ver `tasks/SB-007-trunk.md`.

## Empacotamento e release

Os artefatos (deb por distro, rpm, appimage) são construídos em container com
podman (padrão) ou docker (`CONTAINER_ENGINE=docker`):

```sh
just pkg-deb          # deb do motor e do Studio em dist/ubuntu e dist/debian
just pkg-rpm          # rpms em dist/
just pkg-appimage     # AppImage do Studio em dist/
```text

O smoke de instalação valida os artefatos em container limpo como usuário da
distro — ver `packaging/README.md` para os comandos. A verificação de instalação
via CMake (`cmake --install`) também roda no CI diário. A release em tag `v*`
é gerenciada por `.github/workflows/release.yml` (build, smoke por artefato e
draft de release com os arquivos).
````

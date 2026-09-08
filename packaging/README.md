# ShowBox Packaging

Empacotamento do ShowBox em formatos de distribuição. Cada formato produz **dois
pacotes**: `showbox` (motor) e `showbox-studio` (editor visual), evitando
conflito de caminho e refletindo a dependência de execução do Studio pelo motor.

## Formatos Suportados

| Formato                 | Diretório   | Detecção de Dependências             | Saída                            |
| ----------------------- | ----------- | ------------------------------------ | -------------------------------- |
| **DEB** (Debian/Ubuntu) | `deb/`      | Automática via `dpkg-shlibdeps`      | `dist/ubuntu/`, `dist/debian/`   |
| **RPM** (Fedora)        | `rpm/`      | Automática via `AutoReqProv`         | `dist/`                          |
| **AppImage** (Studio)   | `appimage/` | Bundle via `linuxdeploy` + Qt plugin | `dist/ShowBox-Studio-*.AppImage` |
| **Flatpak**             | `flatpak/`  | Runtime KDE Platform 6.6             | — (não na pipeline)              |

Os debs são separados por distro porque o `dpkg-shlibdeps` registra dependências
diferentes por imagem base (ex.: `libqt6core6t64` no Ubuntu 24.04 vs
`libqt6core6` no Debian Trixie). O nome do DEB carrega a distribuição
(`..._ubuntu24.04_amd64.deb` vs `..._debian13_amd64.deb`) para que as variantes
não colidam no mesmo release.

## Requisitos

- `just` (receitas `pkg-deb`, `pkg-rpm`, `pkg-appimage`, `pkg-install-smoke`);
- podman ou docker (padrão podman; configure com `CONTAINER_ENGINE=docker`).

As receitas constroem os artefatos **em container**, portanto o host não precisa
de tools do formato. Para validar o ambiente local: `just doctor` e `just setup`.

## Versão

A versão de toda a suíte vem de um único arquivo, `VERSION` na raiz (SemVer,
ex.: `1.0.0-rc.3`). Os scripts de packaging e o CI derivam a sintaxe de cada
formato via `tools/version.sh`:

| Consumidor         | Derivado                        | Exemplo (1.0.0-rc.3) |
| ------------------ | ------------------------------- | -------------------- |
| Aplicação/AppImage | `--app`                         | `1.0.0-rc.3`         |
| Debian             | `--deb`                         | `1.0.0~rc3-1`        |
| RPM                | `--rpm-version`/`--rpm-release` | `1.0.0` / `0.3.rc3`  |
| Release            | `--prerelease`                  | `true`               |

### Regras do `VERSION`

- SemVer **estrito**, sem prefixo `v` (o prefixo pertence só à tag Git) e sem
  metadados `+build`.
- Sem espaços e sem conteúdo vazio.
- Pré-releases somente nos estágios `alpha.N`, `beta.N` ou `rc.N` com `N >= 1`.
  Conversões: Debian `${core}~${stage}${N}-1` e RPM `0.${rank}.${stage}${N}`,
  em que `rank` é fixo por estágio (`alpha`=1, `beta`=2, `rc`=3); estável vira
  Debian `${core}-1` e RPM release `1`. O rank garante a ordem
  `alpha.N < beta.N < rc.N < estável` independente da sequência
  (ex.: `alpha.9 < beta.1`; `1.0.0-rc.3` → `1.0.0~rc3-1` e `0.3.rc3`).

Entradas inválidas são rejeitadas com `exit != 0` e mensagem em stderr; o
contrato `tests/integration/version_contract.sh` cobre conversões, entradas
inválidas e a ordenação (`dpkg --compare-versions`; RPM via o port
`tests/integration/rpmvercmp.py`, sempre, com cross-check no `rpmdev-vercmp`
real quando disponível) e roda em `just test` (CTest) e no `build-test` de
todo PR; o smoke Fedora (`install_smoke.sh rpm`) confere as relações cruzadas
com o `rpmdevtools` real do container.

### Como incrementar

1. Editar apenas `VERSION` (ex.: `1.0.0-rc.3` → `1.0.0-rc.4` ou `1.0.0`).
2. Rodar `tools/version.sh` e conferir as conversões.
3. A próxima tag de release será `v$(tools/version.sh --app)`; o CI rejeita
   tag divergente do `VERSION` antes de publicar.

As tags de release seguem `v$(tools/version.sh --app)`; o CI valida que a tag
corresponde ao `VERSION` antes de publicar.

## Como Construir

Todos os comandos rodam da raiz do repositório:

```bash
just pkg-deb        # deb: ubuntu e debian em dist/ubuntu e dist/debian
just pkg-rpm        # rpm: showbox e showbox-studio em dist/
just pkg-appimage   # appimage do Studio em dist/ShowBox-Studio-1.0.0-x86_64.AppImage
```

## Smoke de Instalação

Instala os artefatos em container limpo como usuário da distro e valida o motor
(`--version`, `--help` com "stdin") e o Studio (`--version`), além da separação
dos pacotes (o do motor não contém o binário do Studio):

```bash
just pkg-install-smoke deb dist/ubuntu ubuntu
just pkg-install-smoke deb dist/debian debian
just pkg-install-smoke rpm dist
just pkg-install-smoke appimage dist/ShowBox-Studio-1.0.0-x86_64.AppImage
```

`install_smoke.sh` exige podman (padrão) ou docker em `CONTAINER_ENGINE`.
`INSTALL_SMOKE_DEBUG=1` imprime o script enviado ao container.

## Estrutura

```text
packaging/
├── README.md
├── desktop/           # Desktop entries canônicos (motor e Studio)
├── icon/              # Ícones canônicos
├── deb/
│   ├── build.sh       # Build dentro do container
│   ├── start-pkg-deb.sh   # Orquestra; argumentos: ubuntu | debian | all
│   ├── ubuntu.Dockerfile
│   ├── debian.Dockerfile
│   └── debian/        # control, rules, showbox.install, showbox-studio.install
├── rpm/
│   ├── build.sh
│   ├── start-pkg-rpm.sh
│   ├── fedora.Dockerfile
│   └── showbox.spec   # Subpacotes showbox e showbox-studio
└── appimage/
    ├── AppRun         # Entrypoint do Studio no AppImage
    ├── build.sh
    ├── start-pkg-appimage.sh
    └── appimage.Dockerfile
```

Os artefatos em `dist/` e as ferramentas baixadas pelo linuxdeploy
(`packaging/appimage/tools/`) ficam fora do Git.

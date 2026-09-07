# ShowBox Packaging

Empacotamento do ShowBox em formatos de distribuição. Cada formato produz **dois
pacotes**: `showbox` (motor) e `showbox-studio` (editor visual), evitando
conflito de caminho e refletindo a dependência de execução do Studio pelo motor.

## Formatos Suportados

| Formato                        | Diretório   | Detecção de Dependências             | Saída                     |
| ------------------------------ | ----------- | ------------------------------------ | ------------------------- |
| **DEB** (Debian/Ubuntu)        | `deb/`      | Automática via `dpkg-shlibdeps`      | `dist/ubuntu/`, `dist/debian/` |
| **RPM** (Fedora)               | `rpm/`      | Automática via `AutoReqProv`         | `dist/`                   |
| **AppImage** (Studio)          | `appimage/` | Bundle via `linuxdeploy` + Qt plugin | `dist/ShowBox-Studio-*.AppImage` |
| **Flatpak**                    | `flatpak/`  | Runtime KDE Platform 6.6             | — (não na pipeline)       |

Os debs são separados por distro porque o `dpkg-shlibdeps` registra dependências
diferentes por imagem base (ex.: `libqt6core6t64` no Ubuntu 24.04 vs
`libqt6core6` no Debian Trixie).

## Requisitos

- `just` (receitas `pkg-deb`, `pkg-rpm`, `pkg-appimage`, `pkg-install-smoke`);
- podman ou docker (padrão podman; configure com `CONTAINER_ENGINE=docker`).

As receitas constroem os artefatos **em container**, portanto o host não precisa
de tools do formato. Para validar o ambiente local: `just doctor` e `just setup`.

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

```
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
# ShowBox Packaging

Este diretório contém os scripts e configurações para empacotar o ShowBox em diferentes formatos de distribuição.

## Formatos Suportados

| Formato                        | Diretório   | Detecção de Dependências             |
| ------------------------------ | ----------- | ------------------------------------ |
| **DEB** (Debian/Ubuntu)        | `deb/`      | Automática via `dpkg-shlibdeps`      |
| **RPM** (Fedora/RHEL/openSUSE) | `rpm/`      | Automática via `AutoReqProv`         |
| **Flatpak**                    | `flatpak/`  | Runtime KDE Platform 6.6             |
| **AppImage**                   | `appimage/` | Bundle via `linuxdeploy` + Qt plugin |

## Dependências Runtime

O ShowBox requer as seguintes bibliotecas Qt6:

- `libqt6core6` - Core Qt6
- `libqt6gui6` - GUI Qt6
- `libqt6widgets6` - Widgets Qt6
- `libqt6charts6` - Charts (para gráficos)
- `libqt6svg6` - SVG (para ícones)
- `libqt6opengl6` - OpenGL

## Como Construir

### DEB (Debian/Ubuntu)

```bash
cd packaging/deb
./build.sh
# Pacote gerado em: dist/showbox_1.0.0-1_amd64.deb
```

### RPM (Fedora/RHEL)

```bash
cd packaging/rpm
./build.sh
# Pacote gerado em: dist/showbox-1.0.0-1.x86_64.rpm
```

### Flatpak

```bash
cd packaging/flatpak
./build.sh
# Bundle gerado em: dist/showbox.flatpak
```

### AppImage

```bash
cd packaging/appimage
./build.sh
# AppImage gerado em: dist/ShowBox-1.0.0-x86_64.AppImage
```

## Instalação

### DEB

```bash
sudo dpkg -i dist/showbox_1.0.0-1_amd64.deb
sudo apt-get install -f  # Instala dependências faltantes
```

### RPM

```bash
sudo dnf install dist/showbox-1.0.0-1.x86_64.rpm
```

### Flatpak

```bash
flatpak install dist/showbox.flatpak
flatpak run io.github.showbox
```

### AppImage

```bash
chmod +x dist/ShowBox-1.0.0-x86_64.AppImage
./dist/ShowBox-1.0.0-x86_64.AppImage
```

## Estrutura

```
packaging/
├── README.md           # Este arquivo
├── deb/
│   ├── build.sh        # Script de build DEB
│   └── debian/         # Arquivos de controle Debian
│       ├── control     # Metadados e dependências
│       ├── rules       # Regras de build
│       ├── changelog   # Histórico de versões
│       ├── copyright   # Licença
│       └── compat      # Nível de compatibilidade
├── rpm/
│   ├── build.sh        # Script de build RPM
│   └── showbox.spec    # Spec file
├── flatpak/
│   ├── build.sh        # Script de build Flatpak
│   ├── io.github.showbox.yaml      # Manifest
│   ├── io.github.showbox.desktop   # Desktop entry
│   └── io.github.showbox.metainfo.xml  # AppStream metadata
└── appimage/
    ├── build.sh        # Script de build AppImage
    └── showbox.desktop # Desktop entry
```

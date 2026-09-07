# Interface única; CMake continua responsável pela compilação.
set positional-arguments
set shell := ["bash", "--noprofile", "--norc", "-eo", "pipefail", "-c"]

default:
    @just --list

doctor:
    bash tools/setup/doctor.sh

setup:
    bash tools/setup/debian.sh

configure:
    cmake --preset dev

build: configure
    cmake --build --preset dev

test: build
    ctest --preset dev

check:
    tools/setup/trunk.sh check

run-studio: build
    build/dev/bin/showbox-studio

worktree id topic:
    python3 tools/git/worktree.py "$1" "$2"

# Executa a demonstração contra o motor desta worktree.
demo: build
    SHOWBOX_BIN="$PWD/build/dev/bin/showbox" bash examples/hello-world/run.sh

# Pacotes separados do motor e do Studio (deb ubuntu+debian, rpm fedora,
# AppImage do Studio). Exigem um engine de container (podman/docker).
pkg-deb:
    bash packaging/deb/start-pkg-deb.sh all

pkg-rpm:
    bash packaging/rpm/start-pkg-rpm.sh

pkg-appimage:
    bash packaging/appimage/start-pkg-appimage.sh

# Smoke de instalação: install_smoke.sh deb <deb> ubuntu|debian | rpm <rpm>
pkg-install-smoke args:
    bash tests/installation/install_smoke.sh {{args}}

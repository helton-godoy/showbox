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
    git diff --check
    for script in tools/setup/*.sh; do bash -n "$script"; done

run-studio: build
    build/dev/bin/showbox-studio

worktree id topic:
    python3 tools/git/worktree.py "$1" "$2"

#!/usr/bin/env bash
set -euo pipefail
missing=0
for tool in git cmake ninja c++ bash python3 qmake6; do
    if command -v "$tool" >/dev/null 2>&1; then
        printf 'OK: %s\n' "$tool"
    else
        printf 'Ausente: %s\n' "$tool" >&2
        missing=1
    fi
done
if ((missing)); then
    printf 'Execute just setup em Debian/Ubuntu ou instale as dependências equivalentes.\n' >&2
    exit 1
fi
# A configuração verifica versões e componentes Qt de desenvolvimento.
cmake --preset dev

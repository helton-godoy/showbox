#!/bin/bash
set -e

# Script para compilar e rodar o verificador visual
# Este script deve ser executado de fora do container (host)

echo "=== Compilando e Executando Verificador Visual ==="

./src/start-dev.sh "cd ../showbox && \
    mkdir -p build && cd build && \
    cmake .. && \
    make -j$(nproc) visual_verifier && \
    echo 'Iniciando visual_verifier...' && \
    ./bin/visual_verifier"

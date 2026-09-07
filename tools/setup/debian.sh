#!/usr/bin/env bash
set -euo pipefail
. /etc/os-release
case " ${ID:-} ${ID_LIKE:-} " in
    *debian*|*ubuntu*|*neon*) ;;
    *) printf 'Setup automático disponível apenas para Debian/Ubuntu e derivados.\n' >&2; exit 1 ;;
esac
printf 'Instalando dependências de desenvolvimento pelo apt; sudo pode pedir senha.\n'
sudo apt-get update
sudo apt-get install -y build-essential git cmake ninja-build python3 util-linux \
    qt6-base-dev qt6-charts-dev qt6-svg-dev shellcheck
# Empacotamento e testes de instalação em container (packaging/).
sudo apt-get install -y debhelper dpkg-dev fakeroot podman
printf 'Ambiente preparado. Execute just doctor ou cmake --preset dev.\n'

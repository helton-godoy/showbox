#!/usr/bin/env bash
set -euo pipefail

source_root="${1:?uso: build_legacy_oracle.sh SOURCE_ROOT BUILD_DIR}"
build_dir="${2:?uso: build_legacy_oracle.sh SOURCE_ROOT BUILD_DIR}"

mkdir -p "${build_dir}/bin" "${build_dir}/obj" "${build_dir}/moc"

qmake6 \
    DESTDIR="${build_dir}/bin" \
    OBJECTS_DIR="${build_dir}/obj" \
    MOC_DIR="${build_dir}/moc" \
    "${source_root}/src/code/showbox/showbox.pro" \
    -o "${build_dir}/Makefile"

make -C "${build_dir}" -j"$(getconf _NPROCESSORS_ONLN)"

printf '%s\n' "${build_dir}/bin/showbox-legacy"

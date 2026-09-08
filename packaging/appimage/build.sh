#!/bin/bash
#
# ShowBox Studio AppImage Builder
# Empacota o editor visual (showbox-studio) com as bibliotecas Qt via linuxdeploy.
#
# Execução padrão pelo container (start-pkg-appimage.sh); sem engine de
# container, rode dentro do diretório packaging/appimage a partir da árvore
# com as ferramentas linuxdeploy disponíveis em LINUXDEPLOY_TOOLS_DIR.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"
DIST_DIR="${PROJECT_ROOT}/dist"
APPDIR="${SCRIPT_DIR}/ShowBox.AppDir"
BUILD_DIR="${APPIMAGE_BUILD_DIR:-${PROJECT_ROOT}/build-appimage}"
# Versão derivada da fonte única VERSION (SemVer, ex.: 1.0.0-rc.3).
VERSION="$("${PROJECT_ROOT}/tools/version.sh" --app)"
TOOLS_DIR="${LINUXDEPLOY_TOOLS_DIR:-${SCRIPT_DIR}/tools}"

echo "=== ShowBox Studio AppImage Builder ==="

mkdir -p "${TOOLS_DIR}" "${DIST_DIR}"

for tool in linuxdeploy-x86_64.AppImage linuxdeploy-plugin-qt-x86_64.AppImage; do
	if [[ ! -x "${TOOLS_DIR}/${tool}" ]] && [[ -x "${SCRIPT_DIR}/${tool}" ]]; then
		cp "${SCRIPT_DIR}/${tool}" "${TOOLS_DIR}/${tool}"
	fi
done

# Construção sempre a partir da árvore local, Release, com o Studio habilitado.
cmake -S "${PROJECT_ROOT}" -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE=Release
cmake --build "${BUILD_DIR}" --parallel

# Instala apenas o componente do Studio (binário, desktop e ícone canônicos).
rm -rf "${APPDIR}"
mkdir -p "${APPDIR}"
DESTDIR="${APPDIR}" cmake --install "${BUILD_DIR}" --prefix /usr --strip \
	--component showbox-studio

# Plugins Wayland são carregados dinamicamente e precisam ser semeados.
QT_PLUGIN_DIR="$(qtpaths6 --plugin-dir)"
mkdir -p "${APPDIR}/usr/plugins/platforms"
cp "${QT_PLUGIN_DIR}/platforms/"libqwayland-*.so "${APPDIR}/usr/plugins/platforms/" 2>/dev/null || true
for plugin_group in \
	wayland-decoration-client \
	wayland-graphics-integration-client \
	wayland-shell-integration; do
	if [[ -d "${QT_PLUGIN_DIR}/${plugin_group}" ]]; then
		cp -a "${QT_PLUGIN_DIR}/${plugin_group}" "${APPDIR}/usr/plugins/"
	fi
done

# linuxdeploy detecta e empacota as dependências Qt do AppDir.
export VERSION="${VERSION}"
QMAKE="$(command -v qmake6)"
export QMAKE

cd "${SCRIPT_DIR}"

"${TOOLS_DIR}/linuxdeploy-x86_64.AppImage" \
	--appdir "${APPDIR}" \
	--plugin qt \
	--output appimage \
	--custom-apprun "${SCRIPT_DIR}/AppRun" \
	--desktop-file "${APPDIR}/usr/share/applications/showbox-studio.desktop" \
	--icon-file "${APPDIR}/usr/share/icons/hicolor/scalable/apps/showbox-studio.svg"

# Move para dist com nome determinístico.
shopt -s nullglob
for candidate in ShowBox*.AppImage Showbox_*.AppImage ShowboxStudio*.AppImage; do
	if [[ -f ${candidate} ]]; then
		mv -f "${candidate}" "${DIST_DIR}/ShowBox-Studio-${VERSION}-x86_64.AppImage"
		break
	fi
done

rm -rf "${APPDIR}"

echo ""
echo "=== Build Complete ==="
ls -la "${DIST_DIR}"/ShowBox-Studio-*.AppImage 2>/dev/null || echo "No AppImage found"

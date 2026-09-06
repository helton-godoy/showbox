#!/bin/bash
set -e

# Configurações de Caminhos
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="/workspace/showbox-studio"
SHOWBOX_ROOT="/workspace/showbox"
BUILD_DIR="${PROJECT_ROOT}/build-pkg"
DIST_DIR="${PROJECT_ROOT}/dist"
APPDIR="${BUILD_DIR}/AppDir"

echo "=== Showbox Studio AppImage Builder (Professional Edition) ==="

# 1. Preparar Diretórios
rm -rf "${BUILD_DIR}" "${APPDIR}"
mkdir -p "${BUILD_DIR}" "${DIST_DIR}"

# 2. Compilar com CMake usando GNUInstallDirs e CMAKE_INSTALL_PREFIX=/usr
echo "[1/6] Compiling Showbox Studio..."
cd "${BUILD_DIR}"
cmake "${PROJECT_ROOT}" \
	-DSHOWBOX_ROOT="${SHOWBOX_ROOT}" \
	-DCMAKE_INSTALL_PREFIX=/usr \
	-DCMAKE_BUILD_TYPE=Release

make -j$(nproc)

# 3. Instalação Completa via CMake (Binário, Ícone, Desktop)
echo "[2/6] Installing to AppDir via CMake..."
make install DESTDIR="${APPDIR}"

# 4. Baixar Ferramentas
echo "[3/6] Downloading deployment tools..."
cd "${BUILD_DIR}"
WGET_OPTS="--no-check-certificate -q -nc"
wget ${WGET_OPTS} https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
wget ${WGET_OPTS} https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage
wget ${WGET_OPTS} https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage
wget ${WGET_OPTS} https://github.com/AppImage/type2-runtime/releases/download/continuous/runtime-x86_64
chmod +x *.AppImage

# 5. Configurar Ambiente para Temas e Portais
export EXTRA_QT_PLUGINS="platformthemes,wayland-graphics-integration"
export QMAKE=/usr/bin/qmake6
export LINUXDEPLOY_OUTPUT_VERSION="0.1.0"

# Nota: Não passamos --desktop-file ou --icon-file para o linuxdeploy porque
# eles já estão no lugar certo (usr/share/...) graças ao make install profissional.
# Passamos --custom-apprun apontando para o arquivo fonte original para evitar erro de cópia circular.

echo "[4/6] Running linuxdeploy (bundling dependencies)..."
./linuxdeploy-x86_64.AppImage \
	--appdir "${APPDIR}" \
	--plugin qt \
	--custom-apprun "${PROJECT_ROOT}/packaging/appimage/AppRun" \
	--icon-file "${APPDIR}/usr/share/icons/hicolor/scalable/apps/showbox-studio.svg" \
	--desktop-file "${APPDIR}/usr/share/applications/showbox-studio.desktop"

echo "[5/6] Packaging with appimagetool manually..."
# Forçamos o uso do runtime local
./appimagetool-x86_64.AppImage "${APPDIR}" \
	--runtime-file runtime-x86_64 \
	"Showbox_Studio-${LINUXDEPLOY_OUTPUT_VERSION}-x86_64.AppImage"

# 6. Mover resultado para pasta dist
mv Showbox_Studio*.AppImage "${DIST_DIR}/"

echo "=== Success! Professional Package available in dist/ ==="

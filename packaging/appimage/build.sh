#!/bin/bash
#
# ShowBox AppImage Builder
# Uses linuxdeploy with Qt plugin for automatic dependency bundling
#

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"
DIST_DIR="${PROJECT_ROOT}/dist"
APPDIR="${SCRIPT_DIR}/ShowBox.AppDir"
VERSION="1.0.0"
BUILD_DIR="${APPIMAGE_BUILD_DIR:-${PROJECT_ROOT}/build-appimage}"

echo "=== ShowBox AppImage Builder ==="

# Download linuxdeploy tools if not present
TOOLS_DIR="${LINUXDEPLOY_TOOLS_DIR:-${SCRIPT_DIR}/tools}"
mkdir -p "${TOOLS_DIR}"

if [[ ! -f "${TOOLS_DIR}/linuxdeploy-x86_64.AppImage" ]]; then
	echo "Downloading linuxdeploy..."
	wget -q -O "${TOOLS_DIR}/linuxdeploy-x86_64.AppImage" \
		"https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage"
	chmod +x "${TOOLS_DIR}/linuxdeploy-x86_64.AppImage"
fi

if [[ ! -f "${TOOLS_DIR}/linuxdeploy-plugin-qt-x86_64.AppImage" ]]; then
	echo "Downloading linuxdeploy Qt plugin..."
	wget -q -O "${TOOLS_DIR}/linuxdeploy-plugin-qt-x86_64.AppImage" \
		"https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage"
	chmod +x "${TOOLS_DIR}/linuxdeploy-plugin-qt-x86_64.AppImage"
fi

# Always rebuild from the mounted source tree. Reusing a pre-existing binary
# can silently package a different revision than the one being released.
echo "Building ShowBox..."
cmake -S "${PROJECT_ROOT}" -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE=Release -DSHOWBOX_BUILD_STUDIO=OFF
cmake --build "${BUILD_DIR}" --parallel

# Create AppDir structure
rm -rf "${APPDIR}"
mkdir -p "${APPDIR}/usr/bin"
mkdir -p "${APPDIR}/usr/share/applications"
mkdir -p "${APPDIR}/usr/share/icons/hicolor/scalable/apps"

# Copy files
cp "${BUILD_DIR}/bin/showbox" "${APPDIR}/usr/bin/"
cp "${SCRIPT_DIR}/showbox.desktop" "${APPDIR}/usr/share/applications/"
cp "${SCRIPT_DIR}/showbox.svg" \
	"${APPDIR}/usr/share/icons/hicolor/scalable/apps/showbox.svg"

# linuxdeploy detects the XCB platform from the build environment, but Wayland
# plugins are loaded dynamically and must be seeded into the AppDir explicitly.
QT_PLUGIN_DIR="$(qtpaths6 --plugin-dir)"
mkdir -p "${APPDIR}/usr/plugins/platforms"
cp "${QT_PLUGIN_DIR}/platforms/"libqwayland-*.so \
	"${APPDIR}/usr/plugins/platforms/"
for plugin_group in \
	wayland-decoration-client \
	wayland-graphics-integration-client \
	wayland-shell-integration; do
	if [[ -d "${QT_PLUGIN_DIR}/${plugin_group}" ]]; then
		cp -a "${QT_PLUGIN_DIR}/${plugin_group}" "${APPDIR}/usr/plugins/"
	fi
done

# Create dist directory
mkdir -p "${DIST_DIR}"

# Build AppImage using linuxdeploy with Qt plugin
# The Qt plugin automatically detects and bundles Qt dependencies
export VERSION="${VERSION}"

cd "${SCRIPT_DIR}"

"${TOOLS_DIR}/linuxdeploy-x86_64.AppImage" \
	--appdir "${APPDIR}" \
	--plugin qt \
	--output appimage \
	--desktop-file "${APPDIR}/usr/share/applications/showbox.desktop" \
	--icon-file "${APPDIR}/usr/share/icons/hicolor/scalable/apps/showbox.svg"

# Move to dist
mv ShowBox*.AppImage "${DIST_DIR}/" 2>/dev/null || true

# Cleanup
rm -rf "${APPDIR}"

echo ""
echo "=== Build Complete ==="
echo "AppImage available in: ${DIST_DIR}"
ls -la "${DIST_DIR}"/*.AppImage 2>/dev/null || echo "No AppImage found"

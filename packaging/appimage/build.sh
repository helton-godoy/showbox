#!/bin/bash
#
# ShowBox AppImage Builder
# Uses linuxdeploy with Qt plugin for automatic dependency bundling
#

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
DIST_DIR="$PROJECT_ROOT/dist"
APPDIR="$SCRIPT_DIR/ShowBox.AppDir"
VERSION="1.0.0"

echo "=== ShowBox AppImage Builder ==="

# Download linuxdeploy tools if not present
TOOLS_DIR="$SCRIPT_DIR/tools"
mkdir -p "$TOOLS_DIR"

if [ ! -f "$TOOLS_DIR/linuxdeploy-x86_64.AppImage" ]; then
    echo "Downloading linuxdeploy..."
    wget -q -O "$TOOLS_DIR/linuxdeploy-x86_64.AppImage" \
        "https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage"
    chmod +x "$TOOLS_DIR/linuxdeploy-x86_64.AppImage"
fi

if [ ! -f "$TOOLS_DIR/linuxdeploy-plugin-qt-x86_64.AppImage" ]; then
    echo "Downloading linuxdeploy Qt plugin..."
    wget -q -O "$TOOLS_DIR/linuxdeploy-plugin-qt-x86_64.AppImage" \
        "https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage"
    chmod +x "$TOOLS_DIR/linuxdeploy-plugin-qt-x86_64.AppImage"
fi

# Build ShowBox if not already built
if [ ! -f "$PROJECT_ROOT/src/code/showbox/bin/showbox" ]; then
    echo "Building ShowBox..."
    make -C "$PROJECT_ROOT/src/code/showbox"
fi

# Create AppDir structure
rm -rf "$APPDIR"
mkdir -p "$APPDIR/usr/bin"
mkdir -p "$APPDIR/usr/share/applications"
mkdir -p "$APPDIR/usr/share/icons/hicolor/256x256/apps"

# Copy files
cp "$PROJECT_ROOT/src/code/showbox/bin/showbox" "$APPDIR/usr/bin/"
cp "$SCRIPT_DIR/showbox.desktop" "$APPDIR/usr/share/applications/"

# Create a simple icon (placeholder - replace with actual icon)
if [ ! -f "$SCRIPT_DIR/showbox.png" ]; then
    echo "Warning: No icon found, using placeholder"
    convert -size 256x256 xc:steelblue -fill white -gravity center \
        -pointsize 48 -annotate 0 "SB" "$APPDIR/usr/share/icons/hicolor/256x256/apps/showbox.png" 2>/dev/null || \
    touch "$APPDIR/usr/share/icons/hicolor/256x256/apps/showbox.png"
fi

# Create dist directory
mkdir -p "$DIST_DIR"

# Build AppImage using linuxdeploy with Qt plugin
# The Qt plugin automatically detects and bundles Qt dependencies
export QMAKE=/usr/bin/qmake6
export VERSION="$VERSION"

cd "$SCRIPT_DIR"

"$TOOLS_DIR/linuxdeploy-x86_64.AppImage" \
    --appdir "$APPDIR" \
    --plugin qt \
    --output appimage \
    --desktop-file "$APPDIR/usr/share/applications/showbox.desktop"

# Move to dist
mv ShowBox*.AppImage "$DIST_DIR/" 2>/dev/null || true

# Cleanup
rm -rf "$APPDIR"

echo ""
echo "=== Build Complete ==="
echo "AppImage available in: $DIST_DIR"
ls -la "$DIST_DIR"/*.AppImage 2>/dev/null || echo "No AppImage found"

#!/bin/bash
#
# ShowBox Flatpak Builder
#

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
DIST_DIR="$PROJECT_ROOT/dist"

echo "=== ShowBox Flatpak Builder ==="

# Install Flatpak SDK if needed
flatpak install -y flathub org.kde.Platform//6.6 org.kde.Sdk//6.6 || true

# Create dist directory
mkdir -p "$DIST_DIR"

cd "$SCRIPT_DIR"

# Build Flatpak
flatpak-builder --force-clean --repo=repo build-dir io.github.showbox.yaml

# Create bundle
flatpak build-bundle repo "$DIST_DIR/showbox.flatpak" io.github.showbox

echo ""
echo "=== Build Complete ==="
echo "Flatpak bundle: $DIST_DIR/showbox.flatpak"

#!/bin/bash
#
# ShowBox DEB Package Builder
# Builds a Debian package with automatic dependency detection
#

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
DIST_DIR="$PROJECT_ROOT/dist"

echo "=== ShowBox DEB Package Builder ==="
echo "Project root: $PROJECT_ROOT"

# Create dist directory
mkdir -p "$DIST_DIR"

# Clean any previous debian directory (may have old compat file)
rm -rf "$PROJECT_ROOT/debian"

# Copy debian directory to project root (required by dpkg-buildpackage)
cp -r "$SCRIPT_DIR/debian" "$PROJECT_ROOT/"

cd "$PROJECT_ROOT"

# Make rules executable
chmod +x debian/rules

# Build the package
# -us -uc: don't sign
# -b: binary only
dpkg-buildpackage -us -uc -b

# Move generated packages to dist
mv ../*.deb "$DIST_DIR/" 2>/dev/null || true
mv ../*.changes "$DIST_DIR/" 2>/dev/null || true
mv ../*.buildinfo "$DIST_DIR/" 2>/dev/null || true

# Cleanup
rm -rf "$PROJECT_ROOT/debian"

echo ""
echo "=== Build Complete ==="
echo "Packages available in: $DIST_DIR"
ls -la "$DIST_DIR"/*.deb 2>/dev/null || echo "No .deb files found"

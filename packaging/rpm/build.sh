#!/bin/bash
#
# ShowBox RPM Package Builder
#

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"
DIST_DIR="${PROJECT_ROOT}/dist"
VERSION="1.0.0"

echo "=== ShowBox RPM Package Builder ==="

# Setup rpmbuild directory structure
mkdir -p ~/rpmbuild/{BUILD,RPMS,SOURCES,SPECS,SRPMS}

# Create tarball
cd "${PROJECT_ROOT}/.."
tar czf ~/rpmbuild/SOURCES/showbox-"${VERSION}".tar.gz showbox

# Copy spec file
cp "${SCRIPT_DIR}/showbox.spec" ~/rpmbuild/SPECS/

# Build RPM
rpmbuild -ba ~/rpmbuild/SPECS/showbox.spec

# Copy to dist
mkdir -p "${DIST_DIR}"
cp ~/rpmbuild/RPMS/*/*.rpm "${DIST_DIR}/" 2>/dev/null || true

echo ""
echo "=== Build Complete ==="
echo "Packages available in: ${DIST_DIR}"
ls -la "${DIST_DIR}"/*.rpm 2>/dev/null || echo "No .rpm files found"

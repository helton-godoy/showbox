#!/bin/bash
#
# ShowBox RPM Package Builder
#

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"
DIST_DIR="${PROJECT_ROOT}/dist"
# Versão derivada da fonte única VERSION (o spec é um template renderizado aqui).
RPM_VERSION="$("${PROJECT_ROOT}/tools/version.sh" --rpm-version)"
RPM_RELEASE="$("${PROJECT_ROOT}/tools/version.sh" --rpm-release)"
VERSION="${RPM_VERSION}"
RPMBUILD_ROOT="${RPMBUILD_ROOT:-/tmp/showbox-rpmbuild}"

echo "=== ShowBox RPM Package Builder ==="

# Setup rpmbuild directory structure
mkdir -p "${RPMBUILD_ROOT}"/{BUILD,BUILDROOT,RPMS,SOURCES,SPECS,SRPMS}

# Create tarball
tar \
	--exclude=.git \
	--exclude=build \
	--exclude='build-*' \
	--exclude='obj-*-linux-gnu' \
	--exclude=dist \
	--transform "s,^\./,showbox-${VERSION}/," \
	-czf "${RPMBUILD_ROOT}/SOURCES/showbox-${VERSION}.tar.gz" \
	-C "${PROJECT_ROOT}" .

# Copy spec file
cp "${SCRIPT_DIR}/showbox.spec" "${RPMBUILD_ROOT}/SPECS/showbox.spec"

# Renderiza os placeholders do template com a versão única derivada acima.
sed -i \
	-e "s/@@RPM_VERSION@@/${RPM_VERSION}/g" \
	-e "s/@@RPM_RELEASE@@/${RPM_RELEASE}/g" \
	"${RPMBUILD_ROOT}/SPECS/showbox.spec"

# Build RPM
rpmbuild --define "_topdir ${RPMBUILD_ROOT}" \
	-ba "${RPMBUILD_ROOT}/SPECS/showbox.spec"

# Copy to dist
mkdir -p "${DIST_DIR}"
find "${RPMBUILD_ROOT}/RPMS" -type f -name '*.rpm' \
	-exec cp -t "${DIST_DIR}" {} +

echo ""
echo "=== Build Complete ==="
echo "Packages available in: ${DIST_DIR}"
ls -la "${DIST_DIR}"/*.rpm 2>/dev/null || echo "No .rpm files found"

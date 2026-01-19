#!/bin/bash
#
# ShowBox DEB Package Builder
# Orchestrates Docker build for DEB packages
#
# Usage:
#   ./start-pkg-deb.sh         # Build for Ubuntu 24.04 (default)
#   ./start-pkg-deb.sh ubuntu  # Build for Ubuntu 24.04
#   ./start-pkg-deb.sh debian  # Build for Debian 13 (Trixie)
#   ./start-pkg-deb.sh all     # Build for both
#

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"
DIST_DIR="${PROJECT_ROOT}/dist"
DISTRO="${1:-ubuntu}"

# Colors
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

log_info() { echo -e "${GREEN}[INFO]${NC} $1"; }
log_warn() { echo -e "${YELLOW}[WARN]${NC} $1"; }
log_error() { echo -e "${RED}[ERROR]${NC} $1"; }

build_deb() {
	local distro="$1"
	local dockerfile="${SCRIPT_DIR}/${distro}.Dockerfile"
	local image_name="showbox-deb-${distro}"

	if [[ ! -f ${dockerfile} ]]; then
		log_error "Dockerfile not found: ${dockerfile}"
		exit 1
	fi

	log_info "Building Docker image for ${distro}..."
	docker build -f "${dockerfile}" -t "${image_name}" "${SCRIPT_DIR}"

	log_info "Building .deb package inside Docker container..."
	docker run --rm \
		-v "${PROJECT_ROOT}:/build:rw" \
		-w /build \
		"${image_name}" \
		bash -c "./packaging/deb/build.sh"

	# Rename output with distro suffix
	local version=$(head -1 "${SCRIPT_DIR}/debian/changelog" | grep -oP '\(.*?\)' | tr -d '()')
	local deb_file="${DIST_DIR}/showbox_${version}_amd64.deb"
	if [[ -f ${deb_file} ]]; then
		local new_name="${DIST_DIR}/showbox_${version}_${distro}_amd64.deb"
		mv "${deb_file}" "${new_name}" 2>/dev/null || true
		log_info "Package created: ${new_name}"
	fi
}

mkdir -p "${DIST_DIR}"

case "${DISTRO}" in
ubuntu | debian)
	build_deb "${DISTRO}"
	;;
all)
	build_deb "ubuntu"
	build_deb "debian"
	;;
*)
	log_error "Unknown distro: ${DISTRO}"
	echo "Usage: $0 [ubuntu|debian|all]"
	exit 1
	;;
esac

log_info "=== Build Complete ==="
ls -la "${DIST_DIR}"/*.deb 2>/dev/null || log_warn "No .deb files found"

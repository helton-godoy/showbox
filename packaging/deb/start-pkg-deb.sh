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
# Falhas dentro de command substitutions propagam com set -e (bash >= 4.4).
shopt -s inherit_errexit

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"
DIST_DIR="${PROJECT_ROOT}/dist"
DISTRO="${1:-ubuntu}"
CONTAINER_ENGINE="${CONTAINER_ENGINE:-podman}"

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
	"${CONTAINER_ENGINE}" build -f "${dockerfile}" -t "${image_name}" "${SCRIPT_DIR}"

	log_info "Building .deb package inside Docker container..."
	"${CONTAINER_ENGINE}" run --rm \
		-v "${PROJECT_ROOT}:/build:rw,Z" \
		-w /build \
		"${image_name}" \
		bash -c "./packaging/deb/build.sh"

	# Each distro gets its own artifact directory: the shlibdeps recorded by
	# dpkg-shlibdeps (e.g. libqt6core6t64 vs libqt6gui6) differ per base image.
	mkdir -p "${DIST_DIR}/${distro}"
	local version label
	# Versão derivada da fonte única VERSION (mesma usada de dentro do container).
	# inherit_errexit propaga falha do version.sh; o `||` falha o build explicitamente.
	version="$("${PROJECT_ROOT}/tools/version.sh" --deb)" || {
		log_error "Não foi possível derivar a versão a partir de VERSION."
		exit 1
	}
	label="$(_distro_label "${distro}")"
	for deb_file in "${DIST_DIR}"/showbox*"_${version}_amd64.deb"; do
		if [[ -f ${deb_file} ]]; then
			local new_name
			new_name="$(basename "${deb_file/_amd64.deb/_${label}_amd64.deb}")"
			mv "${deb_file}" "${DIST_DIR}/${distro}/${new_name}"
			log_info "Package created: ${DIST_DIR}/${distro}/${new_name}"
		fi
	done
}

# Rótulo de distribuição embutido no nome do pacote mantém Ubuntu e Debian
# únicos no mesmo draft (showbox_*_amd64.deb colidia entre os dois builds).
_distro_label() {
	case "$1" in
	ubuntu) echo "ubuntu24.04" ;;
	debian) echo "debian13" ;;
	*)
		log_error "Distro sem rótulo: $1"
		exit 1
		;;
	esac
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
ls -la "${DIST_DIR}"/*/*.deb 2>/dev/null || log_warn "No .deb files found"

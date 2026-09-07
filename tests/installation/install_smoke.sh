#!/usr/bin/env bash
#
# Smoke de instalação em container limpo (podman/docker).
#
# Instala os pacotes produzidos como se fosse um usuário da distro e valida:
#   - motor: showbox --version e --help contendo "stdin";
#   - Studio: showbox-studio --version e presença do binário;
#   - separação: o pacote do motor não contém o binário do Studio e há um
#     pacote próprio para o Studio.
#
# Uso:
#   install_smoke.sh deb <dir-com-os-.deb> <ubuntu|debian>
#   install_smoke.sh rpm <dir-com-os-.rpm>
#   install_smoke.sh appimage <artefato.AppImage>
#
# Requer podman (padrão) ou docker em CONTAINER_ENGINE.

set -euo pipefail

if [[ $# -lt 2 || $# -gt 3 ]]; then
	echo "Uso: $0 deb <dir> <ubuntu|debian> | $0 rpm <dir> | $0 appimage <arquivo>" >&2
	exit 1
fi

format="$1"
source_dir="$2"
distro="${3:-}"
engine="${CONTAINER_ENGINE:-podman}"

[[ -d "${source_dir}" ]] || [[ -f "${source_dir}" ]] || {
	echo "Caminho não encontrado: ${source_dir}" >&2
	exit 1
}
command -v "${engine}" >/dev/null 2>&1 || { echo "Engine de container ausente: ${engine}" >&2; exit 1; }

if [[ -d "${source_dir}" ]]; then
	DIR="$(cd "${source_dir}" && pwd)"
else
	DIR="$(cd "$(dirname "${source_dir}")" && pwd)"
fi
mount_dir="/pkg"
APPIMAGE_NAME="$(basename "${source_dir}")"

run() { # run <imagem> <script> <lista de NAME=valor>
	local image="$1" script="$2"
	local -a env=()
	env+=("--env" "QT_QPA_PLATFORM=offscreen")
	for kv in ${3:-}; do
		env+=("--env" "${kv}")
	done
	local script_file
	script_file="$(mktemp)"
	printf '%s' "${script}" > "${script_file}"
	if [[ -n "${INSTALL_SMOKE_DEBUG:-}" ]]; then
		echo "---- script enviado ao container ----"
		cat "${script_file}"
		echo "------------------------------------"
	fi
	"${engine}" run --rm \
		"${env[@]}" \
		-v "${DIR}:${mount_dir}:ro" \
		-v "${script_file}:/smoke.sh:ro" \
		"${image}" \
		bash /smoke.sh
	rm -f "${script_file}"
}

case "${format}" in
deb)
	[[ -n "${distro}" ]] || { echo "Informe a distro dos artefatos deb: ubuntu ou debian" >&2; exit 1; }
	case "${distro}" in
	ubuntu) image="ubuntu:24.04" ;;
	debian) image="debian:trixie-slim" ;;
	*) echo "Distro deb desconhecida: ${distro}" >&2; exit 1 ;;
	esac
	run "${image}" "$(cat <<-'EOF'
	set -euo pipefail
	export DEBIAN_FRONTEND=noninteractive
	apt-get update -qq >/dev/null
	mapfile -t pkgs < <(ls /pkg/showbox*.deb | grep -v -- -dbgsym)
	apt-get install -y -qq --no-install-recommends "${pkgs[@]}" >/dev/null
	showbox --version
	showbox --help | grep -qi stdin
	showbox-studio --version
	dpkg -s showbox-studio >/dev/null
	if dpkg -L showbox | grep -q '/showbox-studio$'; then
		echo "FALHA: showbox-studio dentro do pacote do motor" >&2
		exit 1
	fi
	echo "smoke deb ${SMOKE_DISTRO}: OK"
	EOF
	)" "SMOKE_DISTRO=${distro}"
	;;
rpm)
	run "fedora:latest" "$(cat <<-'EOF'
	set -euo pipefail
	packages=()
	for f in /pkg/showbox-*.x86_64.rpm; do
		case "$(basename "${f}")" in
		*debuginfo*|*debugsource*) continue ;;
		esac
		packages+=("${f}")
	done
	dnf install -y -q "${packages[@]}" >/dev/null
	showbox --version
	showbox --help | grep -qi stdin
	showbox-studio --version
	rpm -q showbox-studio >/dev/null
	if rpm -ql showbox | grep -q '/showbox-studio$'; then
		echo "FALHA: showbox-studio dentro do pacote do motor" >&2
		exit 1
	fi
	echo "smoke rpm: OK"
	EOF
	)" ""
	;;
appimage)
	if [[ ! -f "${source_dir}" ]]; then
		echo "Arquivo AppImage não encontrado: ${source_dir}" >&2
		exit 1
	fi
	run "ubuntu:24.04" "$(cat <<-'EOF'
	set -euo pipefail
	export DEBIAN_FRONTEND=noninteractive
	export APPIMAGE_EXTRACT_AND_RUN=1
	apt-get update -qq >/dev/null
	apt-get install -y -qq --no-install-recommends libegl1 libgl1 libfontconfig1 \
		libxkbcommon0 libxcb-cursor0 libdbus-1-3 libharfbuzz0b libxrender1 \
		libxext6 libxi6 libxcomposite1 libxdamage1 libxrandr2 libxtst6 \
		libx11-xcb1 libopengl0 libxcb-icccm4 libxcb-image0 libxcb-keysyms1 \
		libxcb-randr0 libxcb-render-util0 libxcb-shape0 libxcb-sync1 \
		libxcb-xfixes0 libxcb-xinerama0 libxcb-xkb1 libxkbcommon-x11-0 \
		libsm6 >/dev/null
	mkdir -p /tmp/appimg
	cp "/pkg/${SMOKE_APPIMAGE}" /tmp/appimg/
	chmod +x "/tmp/appimg/${SMOKE_APPIMAGE}"
	"/tmp/appimg/${SMOKE_APPIMAGE}" --version
	"/tmp/appimg/${SMOKE_APPIMAGE}" --help | grep -qi 'editor'
	echo "smoke appimage: OK"
	EOF
	)" "SMOKE_APPIMAGE=${APPIMAGE_NAME}"
	;;
*)
	echo "Formato desconhecido: ${format}" >&2
	exit 1
	;;
esac
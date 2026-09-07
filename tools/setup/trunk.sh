#!/usr/bin/env bash
set -euo pipefail

# Instala somente o launcher oficial do Trunk e valida a versão do CLI fixada
# em .trunk/trunk.yaml. Não exige conta Trunk nem instala o GitHub App.
readonly LAUNCHER_URL="https://trunk.io/releases/trunk"
readonly EXPECTED_CLI="1.25.0"
readonly TRUNK_YAML=".trunk/trunk.yaml"
# SHA-256 do launcher oficial auditado em 2026-09-07.
# Se o upstream publicar um launcher novo, este hash deve ser atualizado
# somente após revisão manual do script baixado.
readonly EXPECTED_LAUNCHER_SHA256="89fbdd8c7b63649eeb1479415757b898903c041e73b49b78028dbd64eca3087a"

resolve_install_dir() {
	if [[ -n ${TRUNK_INSTALL_DIR-} ]]; then
		printf '%s\n' "${TRUNK_INSTALL_DIR}"
		return
	fi
	local dir="${HOME}/.local/bin"
	mkdir -p "${dir}"
	printf '%s\n' "${dir}"
}

verify_platform() {
	local kernel machine
	kernel="$(uname | tr '[:upper:]' '[:lower:]')"
	machine="$(uname -m)"
	case "${kernel}:${machine}" in
	linux:x86_64 | linux:aarch64 | darwin:x86_64 | darwin:arm64) ;;
	*)
		printf 'Plataforma não suportada pelo Trunk: %s %s\n' "${kernel}" "${machine}" >&2
		exit 1
		;;
	esac
}

read_cli_version() {
	if [[ ! -f ${TRUNK_YAML} ]]; then
		printf 'Arquivo %s ausente na raiz do repositório.\n' "${TRUNK_YAML}" >&2
		exit 1
	fi
	awk '
        /^cli:/ { in_cli = 1; next }
        in_cli && /^[[:space:]]+version:/ { print $2; exit }
        /^[a-zA-Z]/ && in_cli { exit }
    ' "${TRUNK_YAML}"
}

install_launcher() {
	local install_dir="$1"
	local launcher="${install_dir}/trunk"
	local cli_version
	cli_version="$(read_cli_version)"
	if [[ ${cli_version} != "${EXPECTED_CLI}" ]]; then
		printf 'Versão do CLI esperada %s, mas %s fixa em %s.\n' \
			"${EXPECTED_CLI}" "${TRUNK_YAML}" "${cli_version}" >&2
		exit 1
	fi
	if [[ -x ${launcher} ]] && "${launcher}" --version >/dev/null 2>&1 &&
		"${launcher}" --version 2>/dev/null | grep -q "${EXPECTED_CLI}"; then
		printf 'Trunk %s já instalado (%s); nada a fazer.\n' "${cli_version}" "${launcher}"
		return 0
	fi
	printf 'Baixando launcher oficial do Trunk para %s ...\n' "${launcher}"
	if ! curl -fsSL --proto https --proto-redir https --max-time 60 --connect-timeout 10 "${LAUNCHER_URL}" -o "${launcher}.tmp"; then # NOSONAR S6506: --proto https impede redirecionamento para HTTP (transporte já fixado)
		printf 'Falha ao baixar o launcher. Verifique a conexão com %s.\n' "${LAUNCHER_URL}" >&2
		rm -f "${launcher}.tmp"
		exit 1
	fi
	local actual_sha256
	actual_sha256="$(sha256sum "${launcher}.tmp" | cut -d' ' -f1)"
	if [[ ${actual_sha256} != "${EXPECTED_LAUNCHER_SHA256}" ]]; then
		printf 'ERRO: checksum do launcher não corresponde ao esperado.\n' >&2
		printf '  Esperado: %s\n' "${EXPECTED_LAUNCHER_SHA256}" >&2
		printf '  Obtido:   %s\n' "${actual_sha256}" >&2
		printf 'O launcher em %s pode ter sido alterado. Não execute sem revisão manual.\n' "${LAUNCHER_URL}" >&2
		rm -f "${launcher}.tmp"
		exit 1
	fi
	chmod +x "${launcher}.tmp"
	mv -f "${launcher}.tmp" "${launcher}"
	printf 'Launcher instalado. Baixando o CLI fixado e validando versão...\n'
	if ! version_output="$("${launcher}" --version 2>&1)"; then
		printf 'Falha ao executar o Trunk. Consulte a saída acima.\n' >&2
		exit 1
	fi
	printf '%s\n' "${version_output}"
	if ! printf '%s\n' "${version_output}" | grep -q "${EXPECTED_CLI}"; then
		printf 'Atenção: o launcher executou um CLI diferente do esperado %s.\n' \
			"${EXPECTED_CLI}" >&2
		exit 1
	fi
}

verify_platform
install_dir="$(resolve_install_dir)"
install_launcher "${install_dir}"
printf 'Trunk pronto: %s\n' "${install_dir}/trunk"

if [[ ${1-} == check ]]; then
	shift
	exec "${install_dir}/trunk" check --cache=false "$@"
fi

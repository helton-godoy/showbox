#!/usr/bin/env bash
#
# Fonte única de versão da suíte Showbox.
#
# Lê o arquivo VERSION (raiz do monorepo), em SemVer estrito, e emite a versão
# na sintaxe de cada consumidor:
#
#   version.sh                 # SemVer da aplicação (ex.: 1.0.0-rc.3)
#   version.sh --app           # idem
#   version.sh --prerelease    # true|false
#   version.sh --deb           # versão Debian (ex.: 1.0.0~rc3-1)
#   version.sh --rpm-version   # parcela de versão do RPM (ex.: 1.0.0)
#   version.sh --rpm-release   # parcela de release do RPM (ex.: 0.3.rc3)
#
# Regras de VERSION:
#   - SemVer sem prefixo "v" (o prefixo pertence à tag Git) e sem metadados
#     "+build";
#   - sem espaços e sem conteúdo vazio;
#   - pré-releases apenas nos estágios alpha.N|beta.N|rc.N com N >= 1.
#
# A tag de release deve ser "v$(version.sh --app)"; os scripts de packaging e o
# CI derivam tudo daqui. Nunca editar versões em múltiplos arquivos.

set -euo pipefail

readonly SEMVER_PATTERN='^(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)(-([0-9A-Za-z-]+(\.[0-9A-Za-z-]+)*))?$'
readonly STAGE_PATTERN='^(alpha|beta|rc)\.([1-9][0-9]*)$'

main() {
	local root version core prerelease suffix stage sequence deb rpm_release

	root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

	if [[ ! -f "${root}/VERSION" ]]; then
		echo "VERSION não encontrado em ${root}" >&2
		exit 1
	fi
	version="$(<"${root}/VERSION")"

	if [[ -z ${version} ]]; then
		echo "VERSION vazio: informe SemVer, por exemplo 1.0.0 ou 1.0.0-rc.3." >&2
		exit 1
	fi
	if [[ ${version} =~ [[:space:]] ]]; then
		echo "VERSION inválido (contém espaços): ${version}. Use SemVer sem espaços." >&2
		exit 1
	fi
	if [[ ! ${version} =~ ${SEMVER_PATTERN} ]]; then
		echo "VERSION inválido: ${version}. Use SemVer, por exemplo 1.0.0 ou 1.0.0-rc.3." >&2
		exit 1
	fi

	core="${version%%-*}"
	if [[ ${version} == *-* ]]; then
		prerelease=true
		suffix="${version#*-}"
	else
		prerelease=false
		suffix=""
	fi

	if [[ ${prerelease} == true ]]; then
		if [[ ! ${suffix} =~ ${STAGE_PATTERN} ]]; then
			echo "Pré-release não suportado: ${suffix}. Suporte: alpha.N, beta.N ou rc.N (N >= 1)." >&2
			exit 1
		fi
		stage="${BASH_REMATCH[1]}"
		sequence="${BASH_REMATCH[2]}"
		# Debian pré-ordena antes do final via "~": rc.3 -> 1.0.0~rc3-1.
		deb="${core}~${stage}${sequence}-1"
		# RPM: 0.<N>.<stage><N> (ex.: rc.3 -> 0.3.rc3).
		rpm_release="0.${sequence}.${stage}${sequence}"
	else
		deb="${core}-1"
		rpm_release="1"
	fi

	case "${1-}" in
	'' | --app)
		printf '%s\n' "${version}"
		;;
	--prerelease)
		printf '%s\n' "${prerelease}"
		;;
	--deb)
		printf '%s\n' "${deb}"
		;;
	--rpm-version)
		printf '%s\n' "${core}"
		;;
	--rpm-release)
		printf '%s\n' "${rpm_release}"
		;;
	*)
		echo "Uso: version.sh [--app|--prerelease|--deb|--rpm-version|--rpm-release]" >&2
		exit 1
		;;
	esac
}

main "$@"

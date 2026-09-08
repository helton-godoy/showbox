#!/usr/bin/env bash
#
# Fonte única de versão da suíte Showbox.
#
# Lê o arquivo VERSION (raiz do monorepo), em SemVer, e emite a versão na
# sintaxe de cada consumidor:
#
#   version.sh                 # SemVer da aplicação (ex.: 1.0.0-rc.3)
#   version.sh --app           # idem
#   version.sh --prerelease    # true|false
#   version.sh --deb           # versão Debian (ex.: 1.0.0~rc3-1)
#   version.sh --rpm-version   # parcela de versão do RPM (ex.: 1.0.0)
#   version.sh --rpm-release   # parcela de release do RPM (ex.: 0.3.rc3)
#
# A tag de release deve ser "v$(version.sh --app)"; os scripts de packaging e o
# CI derivam tudo daqui. Nunca editar versões em múltiplos arquivos.

set -euo pipefail

main() {
	local root version core prerelease suffix deb rpm_release

	root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
	version="$(tr -d '[:space:]' <"${root}/VERSION")"
	version="${version#v}"

	# SemVer sem sufixo de build: core [(-prerelease)].
	if [[ ${version} == *-* ]]; then
		core="${version%%-*}"
		prerelease=true
		suffix="${version#*-}"
	else
		core="${version}"
		prerelease=false
		suffix=""
	fi

	if [[ ${prerelease} == true ]]; then
		# Debian pré-ordena antes do final via "~"; pontos são removidos: rc.3 -> rc3.
		deb="$(printf '%s~%s-1' "${core}" "${suffix//[.:]/}")"
		# RPM: prerelease rc.N vira 0.<N>.rc<N> (ex.: 0.3.rc3); demais: 0.<pre>.
		if [[ ${suffix} =~ ^(rc)\.([0-9]+)$ ]]; then
			rpm_release="0.${BASH_REMATCH[2]}.${BASH_REMATCH[1]}${BASH_REMATCH[2]}"
		else
			rpm_release="0.${suffix//-/.}"
		fi
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

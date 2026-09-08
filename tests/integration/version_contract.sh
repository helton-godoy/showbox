#!/usr/bin/env bash
#
# Contrato da fonte única de versão (SB-009).
#
# Executa tools/version.sh a partir de uma cópia temporária do script e do
# arquivo VERSION, para não modificar a árvore Git. Cobre:
#   - conversões de estável, rc.N, beta.N e alpha.N;
#   - rejeição de versões inválidas (vazio, prefixo v, espaços, +build,
#     estágios fora de alpha/beta/rc e sufixo malformado);
#   - ordenação dos gerenciadores (dpkg sempre; rpmdev-vercmp se instalado),
#     exigindo que o candidato preceda a versão final.
set -euo pipefail

repo="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
source_script="${repo}/tools/version.sh"
work="$(mktemp -d)"
trap 'rm -rf "${work}"' EXIT
mkdir -p "${work}/tools"
cp "${source_script}" "${work}/tools/version.sh"

failures=0
checks=0

fail() {
	printf 'FALHA: %s\n' "$1" >&2
	failures=$((failures + 1))
}

ok() { checks=$((checks + 1)); }

set_version() { printf '%s' "$1" >"${work}/VERSION"; }
set_version_empty() { : >"${work}/VERSION"; }

check_eq() { # título valor1 valor2
	local title="$1" a="$2" b="$3"
	if [[ ${a} == "${b}" ]]; then
		ok
	else
		fail "${title}: obtido ${a}, esperado ${b}"
	fi
}

expect_ok() { # título version app deb rpm_version rpm_release prerelease
	local title="$1" version="$2" app="$3" deb="$4"
	local rpm_version="$5" rpm_release="$6" prerelease="$7"
	set_version "${version}"
	local got
	if ! got="$(bash "${work}/tools/version.sh" --app)"; then
		fail "${title}: --app falhou"
	else
		check_eq "${title}: --app" "${got}" "${app}"
	fi
	if ! got="$(bash "${work}/tools/version.sh" --deb)"; then
		fail "${title}: --deb falhou"
	else
		check_eq "${title}: --deb" "${got}" "${deb}"
	fi
	if ! got="$(bash "${work}/tools/version.sh" --rpm-version)"; then
		fail "${title}: --rpm-version falhou"
	else
		check_eq "${title}: --rpm-version" "${got}" "${rpm_version}"
	fi
	if ! got="$(bash "${work}/tools/version.sh" --rpm-release)"; then
		fail "${title}: --rpm-release falhou"
	else
		check_eq "${title}: --rpm-release" "${got}" "${rpm_release}"
	fi
	if ! got="$(bash "${work}/tools/version.sh" --prerelease)"; then
		fail "${title}: --prerelease falhou"
	else
		check_eq "${title}: --prerelease" "${got}" "${prerelease}"
	fi
	if ! got="$(bash "${work}/tools/version.sh")"; then
		fail "${title}: default (= --app) falhou"
	else
		check_eq "${title}: default (= --app)" "${got}" "${app}"
	fi
}

expect_fail() { # título [conteúdo]; sem conteúdo = arquivo vazio
	local title="$1"
	if (($# > 1)); then
		set_version "$2"
	else
		set_version_empty
	fi
	local out status
	set +e
	out="$(bash "${work}/tools/version.sh" 2>&1)"
	status=$?
	set -e
	if ((status == 0)); then
		fail "${title}: deveria falhar, mas retornou 0"
	elif [[ -z ${out} ]]; then
		fail "${title}: deveria reportar erro em stderr"
	else
		ok
	fi
}

# Casos positivos do parecer e estágios suportados.
expect_ok 'estável 1.0.0' '1.0.0' '1.0.0' '1.0.0-1' '1.0.0' '1' 'false'
expect_ok 'rc.3' '1.0.0-rc.3' '1.0.0-rc.3' '1.0.0~rc3-1' '1.0.0' '0.3.rc3' 'true'
expect_ok 'beta.2' '2.1.0-beta.2' '2.1.0-beta.2' '2.1.0~beta2-1' '2.1.0' '0.2.beta2' 'true'
expect_ok 'alpha.1' '1.2.3-alpha.1' '1.2.3-alpha.1' '1.2.3~alpha1-1' '1.2.3' '0.1.alpha1' 'true'
expect_ok 'patch inicial 0.0.1' '0.0.1' '0.0.1' '0.0.1-1' '0.0.1' '1' 'false'

# Casos negativos: SemVer inválido, prefixo v, build, espaços, estágio errado.
expect_fail 'arquivo vazio'
expect_fail 'apenas quebra de linha' $'\n'
expect_fail 'prefixo v' 'v1.0.0'
expect_fail 'major com zero inicial' '01.0.0'
expect_fail 'minor com zero inicial' '1.01.0'
expect_fail 'patch com zero inicial' '1.0.01'
expect_fail 'versão incompleta' '1.0'
expect_fail 'apenas texto' 'texto'
expect_fail 'espaço interno' '1.0.0 rc.3'
expect_fail 'espaço à direita' '1.0.0 '
expect_fail 'espaço à esquerda' ' 1.0.0'
expect_fail 'build metadata' '1.0.0+build.1'
expect_fail 'sufixo vazio' '1.0.0-'
expect_fail 'estágio sem número' '1.0.0-rc'
expect_fail 'estágio ponto zero' '1.0.0-rc.0'
expect_fail 'estágio simples sem ponto' '1.0.0-rc3'
expect_fail 'pré-release genérico' '1.0.0-preview'
expect_fail 'pré-release composto' '1.0.0-rc.3.1'
expect_fail 'duplo hífen' '1.0.0--rc.1'

# Ordenação: o candidato deve preceder a versão final nos gerenciadores.
if command -v dpkg >/dev/null 2>&1; then
	if dpkg --compare-versions '1.0.0~rc3-1' lt '1.0.0-1'; then
		ok
	else
		fail 'dpkg: rc.3 < estável'
	fi
	if dpkg --compare-versions '1.0.0~beta2-1' lt '1.0.0~rc3-1'; then
		ok
	else
		fail 'dpkg: beta.2 < rc.3'
	fi
	if dpkg --compare-versions '1.0.0~alpha1-1' lt '1.0.0~beta2-1'; then
		ok
	else
		fail 'dpkg: alpha.1 < beta.2'
	fi
else
	echo 'SKIP: dpkg ausente; ordenação Debian não verificada'
fi

if command -v rpmdev-vercmp >/dev/null 2>&1; then
	# rpmdev-vercmp: 0 igual, 11 primeiro mais novo, 12 primeiro mais antigo.
	set +e
	rpmdev-vercmp '1.0.0-0.3.rc3' '1.0.0-1' >/dev/null 2>&1
	status=$?
	set -e
	if ((status == 12)); then
		ok
	else
		fail "rpmdev-vercmp: rc.3 < estável (status ${status})"
	fi
	set +e
	rpmdev-vercmp '1.0.0-0.1.alpha1' '1.0.0-0.3.rc3' >/dev/null 2>&1
	status=$?
	set -e
	if ((status == 12)); then
		ok
	else
		fail "rpmdev-vercmp: alpha.1 < rc.3 (status ${status})"
	fi
else
	echo 'SKIP: rpmdev-vercmp ausente; ordenação RPM não verificada'
fi

printf 'version_contract: %d casos, %d falhas\n' "${checks}" "${failures}"
((failures == 0))

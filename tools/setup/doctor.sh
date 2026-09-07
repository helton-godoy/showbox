#!/usr/bin/env bash
set -euo pipefail
missing=0
for tool in git cmake ninja c++ bash python3 qmake6; do
	if command -v "$tool" >/dev/null 2>&1; then
		printf 'OK: %s\n' "$tool"
	else
		printf 'Ausente: %s\n' "$tool" >&2
		missing=1
	fi
done
# Ferramentas de empacotamento (packaging/) em sistemas Debian/Ubuntu.
. /etc/os-release
case " ${ID:-} ${ID_LIKE:-} " in
	*debian*|*ubuntu*|*neon*)
		for tool in dpkg-buildpackage dpkg-deb fakeroot dh; do
			if command -v "$tool" >/dev/null 2>&1; then
				printf 'OK: %s\n' "$tool"
			else
				printf 'Ausente: %s\n' "$tool" >&2
				missing=1
			fi
		done
		;;
esac
engine="$(command -v podman || command -v docker || true)"
if [[ -n "${engine}" ]]; then
	printf 'OK: container (%s)\n' "$(basename "${engine}")"
else
	printf 'Ausente: podman|docker (necessário para os testes de instalação)\n' >&2
	missing=1
fi
if ((missing)); then
	printf 'Execute just setup em Debian/Ubuntu ou instale as dependências equivalentes.\n' >&2
	exit 1
fi
# A configuração verifica versões e componentes Qt de desenvolvimento.
cmake --preset dev
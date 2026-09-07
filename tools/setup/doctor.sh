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

# --- Repositório remoto (helton-godoy/showbox) ---------------------------------
repo="helton-godoy/showbox"
expected_checks=("build-test" "sanitizers")
check_protection() {
	local branch="$1"
	local payload
	if ! payload="$(gh api "repos/${repo}/branches/${branch}/protection" 2>/dev/null)"; then
		printf 'Proteção ausente: %s\n' "$branch" >&2
		return 1
	fi
	local info ctx count
	info="$(printf '%s' "$payload" | python3 -c '
import json, sys
d = json.load(sys.stdin)
ctx = (d.get("required_status_checks") or {}).get("contexts") or []
count = (d.get("required_pull_request_reviews") or {}).get("required_approving_review_count")
print("contexts=" + ",".join(ctx))
print("approvals=" + str(count if count is not None else "NA"))
print("force_push=" + str(d.get("allows_force_pushes")))
print("deletions=" + str(d.get("allows_deletions")))
print("enforce_admins=" + str(d.get("enforce_admins", {}).get("enabled")))
')"
	ctx="$(printf '%s\n' "$info" | sed -n 's/^contexts=//p')"
	count="$(printf '%s\n' "$info" | sed -n 's/^approvals=//p')"
	for expected in "${expected_checks[@]}"; do
		if ! printf ',%s,' "$ctx" | grep -q ",${expected},"; then
			printf 'Check obrigatório ausente em %s: %s [contextos: %s]\n' \
				"$branch" "$expected" "$ctx" >&2
			return 1
		fi
	done
	printf 'OK: %s — approvals=%s contexts=[%s]\n' "$branch" "$count" "$ctx"
}

if command -v gh >/dev/null 2>&1; then
	printf 'OK: gh\n'
	if gh auth status >/dev/null 2>&1; then
		printf 'OK: autenticação gh (%s)\n' "$(gh api user --jq .login)"
	else
		printf 'Falha: gh não autenticado (gh auth login)\n' >&2
		missing=1
	fi
	if gh repo view "${repo}" >/dev/null 2>&1; then
		printf 'OK: acesso a %s (default branch: %s)\n' "$repo" "$(gh repo view "${repo}" --json defaultBranchRef --jq .defaultBranchRef.name)"
	else
		printf 'Falha: acesso ao repositório %s\n' "$repo" >&2
		missing=1
	fi
	remote_branches="$(git ls-remote --heads origin 2>/dev/null)"
	for branch in main integration/showbox-v1; do
		if printf '%s\n' "${remote_branches}" | grep -q "refs/heads/${branch}$"; then
			printf 'OK: branch remota %s\n' "$branch"
		else
			printf 'Ausente: branch remota %s\n' "$branch" >&2
			missing=1
		fi
	done
	# Proteções e nomes efetivos dos checks.
	for branch in main integration/showbox-v1; do
		if check_protection "$branch"; then
			:
		else
			missing=1
		fi
	done
	if gh api "repos/${repo}/rulesets" 2>/dev/null | grep -q 'tags-v-protection'; then
		printf '%s\n' 'OK: tags protegidas (v*)'
	else
		printf '%s\n' 'Ausente: regra de proteção de tags v* (ruleset "tags-v-protection")' >&2
		missing=1
	fi
else
	printf 'Ausente: gh (necessário para verificação do repositório remoto)\n' >&2
	missing=1
fi

if ((missing)); then
	printf 'Execute just setup em Debian/Ubuntu ou instale as dependências equivalentes.\n' >&2
	exit 1
fi
# A configuração verifica versões e componentes Qt de desenvolvimento.
cmake --preset dev
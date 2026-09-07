#!/usr/bin/env bash
set -euo pipefail

showbox_bin="$1"
output_file="$(mktemp)"
trap 'rm -f "${output_file}"' EXIT

set +e
printf '%s\n' \
	'add checkbox "Option" cb1 checked' \
	'add textbox "Name" txt1 text "Alice"' \
	'query' \
	'show' | timeout 1 "${showbox_bin}" --hidden >"${output_file}"
status=$?
set -e

if [[ ${status} -ne 124 ]]; then
	echo "showbox exited unexpectedly with status ${status}" >&2
	exit 1
fi

diff -u <(printf 'cb1=1\ntxt1=Alice\n') "${output_file}"

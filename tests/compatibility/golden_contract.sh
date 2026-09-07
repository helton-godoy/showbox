#!/usr/bin/env bash
set -euo pipefail

legacy_bin="${1:?uso: golden_contract.sh LEGACY_BIN SHOWBOX_BIN}"
showbox_bin="${2:?uso: golden_contract.sh LEGACY_BIN SHOWBOX_BIN}"
work_dir="$(mktemp -d)"
trap 'rm -rf "${work_dir}"' EXIT

commands_file="${work_dir}/commands"
cat >"${commands_file}" <<'EOF'
add checkbox "Option" z_check checked
add textbox "Name" a_text text "Alice"
add table "Key;Value" report_table
add item "Alpha;1"
end table
add chart "Report" report_chart
set report_chart data "Zebra:2;Alpha:1"
add progressbar ignored_progress value 50
query
show
EOF

run_dialog() {
	local executable="$1"
	local output="$2"
	local status

	set +e
	QT_QPA_PLATFORM=offscreen timeout 1 "${executable}" --hidden \
		<"${commands_file}" >"${output}"
	status=$?
	set -e

	if [[ ${status} -ne 124 ]]; then
		echo "${executable} terminou inesperadamente com status ${status}" >&2
		return 1
	fi
}

run_dialog "${legacy_bin}" "${work_dir}/legacy.out"
run_dialog "${showbox_bin}" "${work_dir}/showbox.out"

diff -u "${work_dir}/legacy.out" "${work_dir}/showbox.out"

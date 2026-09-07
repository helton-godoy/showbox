#!/usr/bin/env bash
set -euo pipefail

showbox_bin="${1:?uso: display_backends.sh SHOWBOX_BIN}"
runtime_dir="$(mktemp -d)"
weston_log="${runtime_dir}/weston.log"
trap 'if [[ -n "${weston_pid:-}" ]]; then kill "${weston_pid}" 2>/dev/null || true; fi; rm -rf "${runtime_dir}"' EXIT
chmod 700 "${runtime_dir}"

commands='add checkbox "Backend" backend checked
query
show'

run_contract() {
	local platform="$1"
	shift
	local output
	local status

	output="$(mktemp "${runtime_dir}/${platform}.XXXXXX")"
	set +e
	printf '%s\n' "${commands}" | "$@" timeout 1 "${showbox_bin}" --hidden >"${output}"
	status=$?
	set -e
	[[ ${status} -eq 124 ]]
	diff -u <(printf 'backend=1\n') "${output}"
}

run_contract xcb env XDG_RUNTIME_DIR="${runtime_dir}" QT_QPA_PLATFORM=xcb xvfb-run -a

XDG_RUNTIME_DIR="${runtime_dir}" weston \
	--backend=headless-backend.so \
	--socket=wayland-showbox \
	--idle-time=0 \
	--log="${weston_log}" &
weston_pid=$!

for _ in $(seq 1 50); do
	[[ -S "${runtime_dir}/wayland-showbox" ]] && break
	kill -0 "${weston_pid}" 2>/dev/null || {
		cat "${weston_log}" >&2
		exit 1
	}
	sleep 0.1
done
[[ -S "${runtime_dir}/wayland-showbox" ]]

run_contract wayland env \
	XDG_RUNTIME_DIR="${runtime_dir}" \
	WAYLAND_DISPLAY=wayland-showbox \
	QT_QPA_PLATFORM=wayland

# shellcheck shell=bash
# Showbox: suporte Bash incorporado aos scripts exportados.
# Requer Bash 4.3+, setsid (util-linux) e o executável showbox.

showbox_send() {
	local _sb_arg _sb_command=''
	for _sb_arg in "$@"; do
		if [[ ${_sb_arg} == *$'\n'* || ${_sb_arg} == *$'\r'* ]]; then
			printf 'Showbox: o protocolo desta versão exige valores em uma linha.\n' >&2
			return 64
		fi
		_sb_arg=${_sb_arg//\\/\\\\}
		_sb_arg=${_sb_arg//\"/\\\"}
		_sb_command+="\"${_sb_arg}\" "
	done
	printf '%s\n' "${_sb_command}" >&"${_sb_input_fd}"
}

showbox_set() {
	[[ $# == 3 ]] || {
		printf 'Uso: showbox_set widget propriedade valor\n' >&2
		return 64
	}
	showbox_send set "$1" "$2" "$3"
}

# A consulta atribui à variável do callback, sem eval nem substituição de comando.
showbox_get() {
	local _sb_var=${1-} _sb_target=${2-} _sb_line _sb_remaining
	[[ ${_sb_var} =~ ^[A-Za-z][A-Za-z0-9_]*$ && ${_sb_target} =~ ^[A-Za-z_][A-Za-z0-9_]*$ ]] || return 64
	showbox_send query "${_sb_target}" || return
	local _sb_deadline=$((SECONDS + 3))
	while ((SECONDS < _sb_deadline)); do
		_sb_remaining=$((_sb_deadline - SECONDS))
		if ! IFS= read -r -t "${_sb_remaining}" -u "${_sb_output_fd}" _sb_line; then
			break
		fi
		if [[ ${_sb_line} == "${_sb_target}="* ]]; then
			printf -v "${_sb_var}" '%s' "${_sb_line#*=}"
			return 0
		fi
		# Cliques de outros componentes durante a consulta ficam para depois.
		printf '%s\n' "${_sb_line}" >>"${_sb_queue}"
	done
	printf 'Showbox: consulta sem resposta para %s.\n' "${_sb_target}" >&2
	return 124
}

_sb_stop_action() {
	if [[ -n ${_sb_action_pid-} ]]; then
		kill -TERM -- "-${_sb_action_pid}" 2>/dev/null || :
		sleep 0.1
		kill -KILL -- "-${_sb_action_pid}" 2>/dev/null || :
		wait "${_sb_action_pid}" 2>/dev/null || :
		_sb_action_pid=''
	fi
}

_sb_cleanup() {
	trap - EXIT INT TERM HUP
	_sb_stop_action
	if [[ -n ${_sb_pid-} ]]; then
		kill -TERM "${_sb_pid}" 2>/dev/null || :
		sleep 0.1
		kill -KILL "${_sb_pid}" 2>/dev/null || :
		wait "${_sb_pid}" 2>/dev/null || :
	fi
	[[ -z ${_sb_tmp-} ]] || rm -rf -- "${_sb_tmp}"
}

_sb_launch() {
	local _sb_callback=$1 _sb_status=0
	kill -0 "${_sb_pid}" 2>/dev/null || return 0
	export WIDGET_NAME=$2 EVENT=$3 WIDGET_VALUE=$4
	TIMESTAMP=$(date -Iseconds)
	export TIMESTAMP
	export -f "${_sb_callback?}" showbox_send showbox_set showbox_get
	export _sb_input_fd _sb_output_fd _sb_queue
	# Uma sessão por ação permite encerrar também subprocessos e netos.
	# O $1 é expandido pelo Bash filho, não por este processo.
	# shellcheck disable=SC2016
	_sb_input_fd=3 _sb_output_fd=4 setsid bash -c 'set -e; "$1"' showbox-action "${_sb_callback}" \
		3>"${_sb_tmp}/input" 4<"${_sb_tmp}/output" &
	_sb_action_pid=$!
	while kill -0 "${_sb_action_pid}" 2>/dev/null; do
		if ! kill -0 "${_sb_pid}" 2>/dev/null; then
			_sb_stop_action
			return 0
		fi
		sleep 0.05
	done
	wait "${_sb_action_pid}" || _sb_status=$?
	_sb_stop_action
	if ((_sb_status)); then
		printf 'Showbox: ação %s/%s terminou com código %s.\n' "${WIDGET_NAME}" "${EVENT}" "${_sb_status}" >&2
	fi
}

_sb_run() {
	local _sb_line _sb_status=0 _sb_item
	local -a _sb_pending=()
	set +m # setsid deve manter o PID monitorado, sem job control do Bash.
	_sb_action_pid=''
	_sb_pid=''
	_sb_tmp=$(mktemp -d) || return
	_sb_queue="${_sb_tmp}/events"
	: >"${_sb_queue}"
	trap _sb_cleanup EXIT
	trap 'exit 130' INT
	trap 'exit 143' TERM HUP
	trap '' PIPE
	command -v setsid >/dev/null || {
		printf 'Showbox: instale util-linux (setsid).\n' >&2
		return 127
	}
	command -v "${SHOWBOX_BIN:-showbox}" >/dev/null || {
		printf 'Showbox: executável não encontrado; configure SHOWBOX_BIN.\n' >&2
		return 127
	}
	mkfifo "${_sb_tmp}/input" "${_sb_tmp}/output" || return
	# Abertura read/write evita bloqueio na conexão inicial. A vida do motor é
	# acompanhada pelo PID, pois estes descritores mantêm as FIFOs abertas.
	exec {_sb_input_fd}<>"${_sb_tmp}/input"
	exec {_sb_output_fd}<>"${_sb_tmp}/output"
	"${SHOWBOX_BIN:-showbox}" --hidden <"${_sb_tmp}/input" >"${_sb_tmp}/output" &
	_sb_pid=$!
	_sb_ui >&"${_sb_input_fd}" || return
	while kill -0 "${_sb_pid}" 2>/dev/null; do
		if [[ -s ${_sb_queue} ]]; then
			mapfile -t _sb_pending <"${_sb_queue}"
			: >"${_sb_queue}"
			for _sb_item in "${_sb_pending[@]}"; do
				_sb_dispatch "${_sb_item}"
			done
		elif IFS= read -r -t 0.1 -u "${_sb_output_fd}" _sb_line; then
			_sb_dispatch "${_sb_line}"
		fi
	done
	wait "${_sb_pid}" || _sb_status=$?
	_sb_pid=''
	return "${_sb_status}"
}

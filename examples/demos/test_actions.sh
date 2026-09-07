#!/bin/bash
# Teste de integração para actions em widgets
#
# Usa FIFO para capturar callbacks continuamente

#SHOWBOX="${SCRIPT_DIR}/../../build/bin/showbox"

# Verificar se o showbox foi compilado
if [[ ! -x ${SHOWBOX} ]]; then
	echo "ERRO: showbox não encontrado em ${SHOWBOX}"
	exit 1
fi

# Criar FIFO para comunicação
FIFO=$(mktemp -u)
mkfifo "${FIFO}"

# Cleanup na saída
cleanup() {
	rm -f "${FIFO}"
	kill %1 2>/dev/null || true
}
trap cleanup EXIT

# === Função principal ===
echo "=== Teste de Actions no Showbox ==="
echo "Clique nos botões para testar os callbacks!"
echo ""

# Processar callbacks em background
(
	while IFS= read -r line; do
		if [[ ${line} == CALLBACK:* ]]; then
			callback_name="${line#CALLBACK:}"
			echo ""
			echo "╔════════════════════════════════════════╗"
			echo "║  [CALLBACK RECEBIDO: ${callback_name}]"
			# shellcheck disable=SC2312  # date no texto do box: falha de date irrelevante aqui
			echo "║  Timestamp: $(date '+%Y-%m-%d %H:%M:%S')"
			echo "╚════════════════════════════════════════╝"

			# Executar ação específica
			case "${callback_name}" in
			btn_test_clicked)
				notify-send "Showbox" "Botão de teste clicado!" 2>/dev/null || true
				;;
			btn_alert_clicked)
				notify-send "Showbox" "Alerta disparado!" 2>/dev/null || true
				;;
			*) ;;
			esac
		else
			echo "${line}"
		fi
	done <"${FIFO}"
) &

# Executar showbox com output redirecionado para o FIFO
# cat <<'EOF' | "${SHOWBOX}" >"${FIFO}" 2>&1
# shellcheck disable=SC2312  # status do cat no pipe propositalmente ignorado
cat <<'EOF' | showbox >"${FIFO}" 2>&1
add window "Teste de Actions" main width=400 height=300
add button "Testar Callback" btn_test action="btn_test_clicked"
add button "Disparar Alerta" btn_alert action="btn_alert_clicked"
add button "Sem Action" btn_none
show
EOF

echo ""
echo "=== Showbox fechado ==="

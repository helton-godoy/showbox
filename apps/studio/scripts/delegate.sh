#!/bin/bash
# delegate.sh - Script para delegar tarefas para agentes CLI (Gemini/Kilocode)

show_help() {
	echo 'Uso: ./delegate.sh --agent [gemini|kilocode] --task "Descrição da tarefa" [opções]'
	echo ""
	echo "Opções:"
	echo "  --agent    Agente a ser usado (gemini ou kilocode)"
	echo "  --task     Descrição detalhada da tarefa a ser executada"
	echo "  --context  Arquivo ou diretório para adicionar ao contexto (pode ser usado múltiplas vezes)"
	echo "  --dry-run  Apenas mostra o comando que seria executado"
	echo "  --help     Mostra esta mensagem"
}

AGENT=""
TASK=""
CONTEXT_FILES=""
DRY_RUN=0

# Parse arguments
while [[ $# -gt 0 ]]; do
	key="$1"
	case $key in
	--agent)
		AGENT="$2"
		shift
		shift
		;;
	--task)
		TASK="$2"
		shift
		shift
		;;
	--context)
		CONTEXT_FILES="$CONTEXT_FILES $2"
		shift
		shift
		;;
	--dry-run)
		DRY_RUN=1
		shift
		;;
	--help)
		show_help
		exit 0
		;;
	*)
		echo "Opção desconhecida: $1"
		show_help
		exit 1
		;;
	esac
done

if [ -z "$AGENT" ] || [ -z "$TASK" ]; then
	echo "Erro: Agente e Tarefa são obrigatórios."
	show_help
	exit 1
fi

CMD=""

# Construct command based on agent
if [ "$AGENT" == "gemini" ]; then
	# Gemini CLI construction
	# Assuming gemini uses positional prompt or --prompt. Using positional for robustness if --prompt is deprecated as per help,
	# but help output said --prompt is deprecated, use positional.
	# Also constructing context. Gemini usually takes context via file reading or specific flags if supported,
	# generally we can just cat files or let the agent read them.
	# However, gemini CLI usually works by just passing the prompt.
	# Providing context files in the prompt is a simple strategy.

	FULL_PROMPT="$TASK"

	if [ -n "$CONTEXT_FILES" ]; then
		FULL_PROMPT="$FULL_PROMPT. Contexto: Os seguintes arquivos são relevantes: $CONTEXT_FILES"
	fi

	# Escape quotes in prompt for bash safety
	SAFE_PROMPT=$(printf '%q' "$FULL_PROMPT")

	CMD="gemini \"$FULL_PROMPT\""

elif [ "$AGENT" == "kilocode" ]; then
	# Kilocode usage
	# kilocode [prompt]

	FULL_PROMPT="$TASK"

	if [ -n "$CONTEXT_FILES" ]; then
		FULL_PROMPT="$FULL_PROMPT. Context files: $CONTEXT_FILES"
	fi

	CMD="kilocode \"$FULL_PROMPT\" --auto"
else
	echo "Erro: Agente não suportado '$AGENT'. Use 'gemini' ou 'kilocode'."
	exit 1
fi

echo "Delegando para $AGENT..."
echo "Tarefa: $TASK"
if [ -n "$CONTEXT_FILES" ]; then
	echo "Contexto: $CONTEXT_FILES"
fi

if [ $DRY_RUN -eq 1 ]; then
	echo "[DRY-RUN] Comando a executar:"
	echo "$CMD"
else
	echo "Executando..."
	eval "$CMD"
fi

#!/bin/bash
# agent-coder (Swarm Toolbelt 2.0)
# Role: Implementation, Refactoring
# Strategy: Gemini CLI (Primary) -> OpenCode (Fallback)
# Usage: ./scripts/swarm/coder.sh "Implementation task" [context_files] [--model <opencode_model>]

TASK=""
CONTEXT=""
MODEL="opencode/minimax-m2.1-free" # Default Fallback Model
ENGINE="gemini"                    # Default Engine

# Parse args
while [[ $# -gt 0 ]]; do
	key="$1"
	case ${key} in
	--model)
		MODEL="$2"
		ENGINE="opencode"
		shift
		shift
		;;
	--engine)
		ENGINE="$2"
		shift
		shift
		;;
	*)
		if [[ -z ${TASK} ]]; then
			TASK="$1"
		else
			CONTEXT="${CONTEXT} $1"
		fi
		shift
		;;
	esac
done

if [[ -z ${TASK} ]]; then
	echo "Usage: $0 <task_description> [context_files...] [--engine gemini|opencode] [--model <model_id>]"
	exit 1
fi

echo "[Coder] Starting Task: ${TASK}"
echo "[Coder] Engine: ${ENGINE}"

START_TIME=$(date +%s)

if [[ ${ENGINE} == "gemini" ]]; then
	# Gemini Mode (Headless/Prompt)
	SYSTEM_PROMPT="ACT AS: Expert C++ Engineer (Qt6/C++17). GOAL: Implement the requested feature. RULES: 1. Use pure C++17 and Qt6. 2. Follow Clean Code principles. 3. Return ONLY code artifacts or diffs."

	# Executing Gemini
	# Note: Assuming gemini cli accepts standard input or prompt argument
	gemini "${SYSTEM_PROMPT} Task: ${TASK}. Context: ${CONTEXT}"
	EXIT_CODE=$?
else
	# OpenCode Mode (Fallback)
	echo "[Coder] Using OpenCode Model: ${MODEL}"
	opencode run "Task: ${TASK}. Context Files: ${CONTEXT}. Implement this in C++17/Qt6." --model "${MODEL}"
	EXIT_CODE=$?
fi

END_TIME=$(date +%s)
DURATION=$((END_TIME - START_TIME))

# Callback to Conductor
if [[ ${EXIT_CODE} -eq 0 ]]; then
	echo "[Coder] Task Completed Successfully."
	# Auto-notify Conductor (Headless)
	gemini --prompt "/conductor:notify 'Coder finished task: ${TASK} in ${DURATION}s using ${ENGINE}'" >/dev/null 2>&1 || true
else
	echo "[Coder] Task Failed."
	gemini --prompt "/conductor:notify 'Coder FAILED task: ${TASK}'" >/dev/null 2>&1 || true
fi

exit "${EXIT_CODE}"

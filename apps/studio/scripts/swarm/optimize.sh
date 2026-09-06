#!/bin/bash
# agent-optimize (Swarm Toolbelt 2.0)
# Role: Code Optimization, Refactoring
# Strategy: OpenCode (Specific Models)
# Usage: ./scripts/swarm/optimize.sh "File to optimize"

FILE="$1"
MODEL="opencode/minimax-m2.1-free" # Optimization-friendly model

if [[ -z ${FILE} ]]; then
	echo "Usage: $0 <file_path>"
	exit 1
fi

echo "[Optimize] Optimizing: ${FILE} using ${MODEL}"

opencode run "Analyze ${FILE}. Improve performance and readability. Apply Qt6 best practices. Return the full optimized code." --model "${MODEL}" >"${FILE}.optimized"
EXIT_CODE=$?

if [[ ${EXIT_CODE} -eq 0 ]]; then
	echo "Optimization saved to ${FILE}.optimized"
	gemini --prompt "/conductor:notify 'Optimize Agent finished: ${FILE}'" >/dev/null 2>&1 || true
else
	gemini --prompt "/conductor:notify 'Optimize Agent FAILED: ${FILE}'" >/dev/null 2>&1 || true
fi

exit "${EXIT_CODE}"

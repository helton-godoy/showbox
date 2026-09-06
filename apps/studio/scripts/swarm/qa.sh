#!/bin/bash
# agent-qa (Swarm Toolbelt 2.0)
# Role: Test Generation, TDD, Bug Analysis
# Strategy: Gemini CLI
# Usage: ./scripts/swarm/qa.sh "Feature to test"

TASK="$1"
if [[ -z ${TASK} ]]; then
	echo "Usage: $0 <feature_to_test>"
	exit 1
fi

echo "[QA] Generating tests for: ${TASK}"

SYSTEM_PROMPT="ACT AS: Senior QA Engineer. GOAL: Write robust QTest (Qt Test) unit tests for the described feature. 
REQUIREMENTS: 
1. Use Qt6 Test module.
2. Cover edge cases.
3. Output full compilable .cpp test file."

gemini "${SYSTEM_PROMPT} Feature: ${TASK}"
EXIT_CODE=$?

# Callback
if [[ ${EXIT_CODE} -eq 0 ]]; then
	gemini --prompt "/conductor:notify 'QA Agent generated tests for: ${TASK}'" >/dev/null 2>&1 || true
fi

exit "${EXIT_CODE}"

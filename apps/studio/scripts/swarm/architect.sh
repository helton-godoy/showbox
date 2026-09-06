#!/bin/bash
# agent-architect (Swarm Toolbelt 2.0)
# Role: Requirements Analysis, System Design, Task Breakdown
# Strategy: Kilocode (Primary) or Cline
# Usage: ./scripts/swarm/architect.sh "Requirement" [--level <0-4>]

TASK=""
LEVEL="2"

while [[ $# -gt 0 ]]; do
	key="$1"
	case ${key} in
	--level)
		LEVEL="$2"
		shift
		shift
		;;
	*)
		if [[ -z ${TASK} ]]; then
			TASK="$1"
			shift
		else
			echo "Unknown argument: $1"
			exit 1
		fi
		;;
	esac
done

if [[ -z ${TASK} ]]; then
	echo "Usage: $0 <task_description> [--level <0-4>]"
	exit 1
fi

echo "[Architect] Analyzing: ${TASK} (Level ${LEVEL})"

# Execute Kilocode
# Kilocode is preferred for reasoning/planning steps
kilocode --mode architect "${TASK} (Level ${LEVEL})" --auto
EXIT_CODE=$?

# Callback
if [[ ${EXIT_CODE} -eq 0 ]]; then
	gemini --prompt "/conductor:notify 'Architect finished planning: ${TASK}'" >/dev/null 2>&1 || true
	# Optional: Update plan.md directly via Conductor if possible
else
	gemini --prompt "/conductor:notify 'Architect failed to plan: ${TASK}'" >/dev/null 2>&1 || true
fi

exit "${EXIT_CODE}"

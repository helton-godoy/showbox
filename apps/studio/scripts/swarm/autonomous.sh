#!/bin/bash
# agent-autonomous (Swarm Toolbelt)
# Role: Self-directed tasks, One-shot complex executions
# Usage: ./scripts/swarm/autonomous.sh "Complex task description"

TASK="$1"

if [[ -z ${TASK} ]]; then
	echo "Usage: $0 <task_description>"
	exit 1
fi

echo "Launching Autonomous Agent (Cline)..."
echo "Task: ${TASK}"

# Use Cline in oneshot/yolo mode for maximum autonomy
# --oneshot: runs until completion
# --yolo: auto-approves commands
cline "${TASK}" --oneshot --yolo

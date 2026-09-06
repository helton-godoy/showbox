#!/bin/bash
# agent-manager (Swarm Toolbelt)
# Role: Status Updates, Conductor Integration
# Usage: ./scripts/swarm/manager.sh "Update message"

MSG="$1"
if [[ -z ${MSG} ]]; then
	echo "Usage: $0 <update_message>"
	exit 1
fi

# Interacts with Conductor via Gemini
gemini --prompt "/conductor:status ${MSG}"

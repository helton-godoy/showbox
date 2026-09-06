#!/bin/bash
# agent-integrator (Swarm Toolbelt)
# Role: GitHub Integration, PR Management, Code Review
# Usage: ./scripts/swarm/integrator.sh "Task or PR command" [pr_number]

COMMAND="$1"
ARG="$2"

if [[ -z ${COMMAND} ]]; then
	echo "Usage: $0 <command> [args]"
	echo "Commands:"
	echo "  pr <number>   - Checkout and review a PR"
	echo "  review        - Review current changes"
	exit 1
fi

if [[ ${COMMAND} == "pr" ]]; then
	if [[ -z ${ARG} ]]; then
		echo "Error: PR number required."
		exit 1
	fi
	# Use opencode to handle PR
	echo "Starting PR Agent for PR #${ARG}..."
	opencode pr "${ARG}"
elif [[ ${COMMAND} == "review" ]]; then
	# General review using opencode
	opencode --prompt "Review the current changes in the workspace for bugs and security issues." --model "openai/gpt-4o"
else
	# Fallback to generic opencode usage
	opencode --prompt "${COMMAND} ${ARG}"
fi

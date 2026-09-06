#!/bin/bash
# agent-research (Swarm Toolbelt)
# Role: Documentation Search, Fact Checking
# Usage: ./scripts/swarm/research.sh "Query"

QUERY="$1"
if [[ -z ${QUERY} ]]; then
	echo "Usage: $0 <query>"
	exit 1
fi

# leverage MCP tools if configured in Gemini
gemini "RESEARCH TASK: ${QUERY}. Use available MCP tools (Exa, Google, Context7) to find accurate information. innovative strategies, best practices."

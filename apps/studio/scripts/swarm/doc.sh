#!/bin/bash
# agent-doc (Swarm Toolbelt 2.0)
# Role: Documentation, Comments, Doxygen
# Strategy: Cline + Context7 (MCP)
# Usage: ./scripts/swarm/doc.sh "File to document"

FILE="$1"
if [[ -z ${FILE} ]]; then
	echo "Usage: $0 <file_path>"
	exit 1
fi

echo "[Doc] Documenting: ${FILE}"

# Cline is used here because of its strong MCP integration capabilities
# We assume 'context7' MCP server is configured in Cline's settings
cline "GOAL:Add Doxygen comments to ${FILE}. Use @ai-coders-context to understand project patterns. Check external docs via Context7 if needed." --oneshot

EXIT_CODE=$?

if [[ ${EXIT_CODE} -eq 0 ]]; then
	gemini --prompt "/conductor:notify 'Doc Agent updated: ${FILE}'" >/dev/null 2>&1 || true
fi

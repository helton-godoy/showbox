#!/bin/bash
# setup_context.sh
# Purpose: Initialize the @ai-coders/context environment for the project.

echo "Initializing AI-Coders Context..."

# Check for npm
if ! command -v npm &>/dev/null; then
	echo "Error: npm is not installed."
	exit 1
fi

# Run the initialization wizard/command
# Using 'yes' to proceed with defaults if possible, or just running init
echo "Running npx @ai-coders/context init..."
npx --yes @ai-coders/context init .

echo "Context initialization complete."
echo "Please configure your MCP settings in your IDE or ~/.config/gemini/settings.json to point to this context if needed."
echo "Command to add to Gemini Config:"
echo "  npx @ai-coders/context mcp"

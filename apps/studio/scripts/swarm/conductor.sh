#!/bin/bash
# conductor.sh (Swarm Toolbelt)
# Role: Conductor Interaction Wrapper
#
# RELEVANCE:
# This script standardizes interactions with the Conductor agent via the Gemini CLI.
# Using a dedicated script ensures:
# 1. Consistent command syntax (e.g., proper quoting of prompts).
# 2. Centralized handling of common tasks like checking status or creating tracks.
# 3. Easy integration into other automation scripts (CI/CD).
#
# USAGE:
# ./scripts/swarm/conductor.sh [command] [args...]
#
# COMMANDS:
#   status [msg]       Check status or update it with a message.
#   new-track <name>   Create a new development track.
#   implement          Execute the 'implement' command for current track.
#   close-track        (Manual step) Reminder to update tracks.md.
#
# EXAMPLES:
#   ./conductor.sh status
#   ./conductor.sh status "Completed initial setup"
#   ./conductor.sh new-track "Refactor Login"

CMD="$1"
shift

case "${CMD}" in
status)
	MSG="$1"
	if [[ -n ${MSG} ]]; then
		gemini --prompt "/conductor:status ${MSG}"
	else
		gemini --prompt "/conductor:status"
	fi
	;;
new-track)
	TRACK_NAME="$1"
	if [[ -z ${TRACK_NAME} ]]; then
		echo "Error: Track name required."
		echo "Usage: $0 new-track \"Track Name\""
		exit 1
	fi
	gemini --prompt "/conductor:newTrack ${TRACK_NAME}"
	;;
implement)
	gemini --prompt "/conductor:implement"
	;;
*)
	echo "Usage: $0 {status|new-track|implement}"
	exit 1
	;;
esac

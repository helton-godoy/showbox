#!/bin/bash

DIALOGBOX=./dialogbox_qt6/dist_qt6/dialogbox

if [[ ! -f ${DIALOGBOX} ]]; then
	echo "Error: Binary not found at ${DIALOGBOX}"
	exit 1
fi

HELP_OUTPUT=$(${DIALOGBOX} --help)

echo "Checking help output for new features..."

MISSING_FEATURES=0

if ! echo "${HELP_OUTPUT}" | grep -q "calendar"; then
	echo "FAIL: 'calendar' widget not found in help text."
	MISSING_FEATURES=$((MISSING_FEATURES + 1))
fi

if ! echo "${HELP_OUTPUT}" | grep -q "table"; then
	echo "FAIL: 'table' widget not found in help text."
	MISSING_FEATURES=$((MISSING_FEATURES + 1))
fi

if ! echo "${HELP_OUTPUT}" | grep -q "chart"; then
	echo "FAIL: 'chart' widget not found in help text."
	MISSING_FEATURES=$((MISSING_FEATURES + 1))
fi

if ! echo "${HELP_OUTPUT}" | grep -q "\-\-new-parser"; then
	echo "FAIL: '--new-parser' option not found in help text."
	MISSING_FEATURES=$((MISSING_FEATURES + 1))
fi

if [[ ${MISSING_FEATURES} -eq 0 ]]; then
	echo "SUCCESS: All new features found in help text."
	exit 0
else
	echo "FAILURE: ${MISSING_FEATURES} missing features in help text."
	exit 1
fi

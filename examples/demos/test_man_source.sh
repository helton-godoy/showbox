#!/bin/bash

MAN_SOURCE=dialogbox_qt6/MAN/dialogbox.md

if [[ ! -f ${MAN_SOURCE} ]]; then
	echo "Error: Man page source not found at ${MAN_SOURCE}"
	exit 1
fi

MISSING_FEATURES=0

if ! grep -q "chart" "${MAN_SOURCE}"; then
	echo "FAIL: 'chart' widget not found in man page source."
	MISSING_FEATURES=$((MISSING_FEATURES + 1))
fi

if grep -q "add textinput" "${MAN_SOURCE}"; then
	echo "FAIL: Found incorrect 'add textinput' in examples (should be 'add textbox')."
	MISSING_FEATURES=$((MISSING_FEATURES + 1))
fi

MAN_GENERATED=dialogbox_qt6/MAN/dialogbox.1

if [[ ! -f ${MAN_GENERATED} ]]; then
	echo "FAIL: Generated man page not found at ${MAN_GENERATED}. Build step failed?"
	MISSING_FEATURES=$((MISSING_FEATURES + 1))
fi

if [[ ${MISSING_FEATURES} -eq 0 ]]; then
	echo "SUCCESS: Man page source is up to date."
	exit 0
else
	echo "FAILURE: ${MISSING_FEATURES} issues in man page source."
	exit 1
fi

#!/bin/bash

DEBIAN_DIR=dialogbox_qt6/debian

if [[ ! -d ${DEBIAN_DIR} ]]; then
	echo "FAIL: Debian directory not found."
	exit 1
fi

MISSING_FILES=0

for FILE in control rules install manpages changelog copyright; do
	if [[ ! -f "${DEBIAN_DIR}/${FILE}" ]]; then
		echo "FAIL: Missing file ${DEBIAN_DIR}/${FILE}"
		MISSING_FILES=$((MISSING_FILES + 1))
	fi
done

if [[ ${MISSING_FILES} -eq 0 ]]; then
	echo "SUCCESS: Debian packaging structure is complete."
	exit 0
else
	echo "FAILURE: ${MISSING_FILES} missing files in debian directory."
	exit 1
fi

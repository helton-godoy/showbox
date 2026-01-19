#!/bin/bash
SHOWBOX_BIN="${SHOWBOX_BIN:-./src/code/showbox/bin/showbox}"

if [[ ! -f ${SHOWBOX_BIN} ]]; then
	echo "Error: Binary not found at ${SHOWBOX_BIN}"
	exit 1
fi

echo "Running manual integration test for calendar..."

# Test 1: Create calendar and check initial output (report)
# We use --new-parser and --hidden to run non-interactively
# We send 'query' command to get current state
echo 'add calendar "Birth Day" my_cal date "2026-01-10"
query' | timeout 2s "${SHOWBOX_BIN}" --new-parser --hidden >test_out.txt || true

if grep -q "my_cal=2026-01-10" test_out.txt; then
	echo "Test 1 Passed: Initial date reported correctly."
else
	echo "Test 1 Failed: Initial date NOT reported correctly."
	cat test_out.txt
	exit 1
fi

# Test 2: Update calendar date via 'set' command
echo 'add calendar "Birth Day" my_cal date "2026-01-10"
set my_cal date "2026-12-25"
query' | timeout 2s "${SHOWBOX_BIN}" --new-parser --hidden >test_out.txt || true

if grep -q "my_cal=2026-12-25" test_out.txt; then
	echo "Test 2 Passed: Date updated via 'set' reported correctly."
else
	echo "Test 2 Failed: Date updated via 'set' NOT reported correctly."
	cat test_out.txt
	exit 1
fi

# Cleanup
rm -f test_out.txt
echo "Integration tests finished successfully."

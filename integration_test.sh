#!/bin/bash
set -e

echo "=== Running Integration Test (V1 Parser -> Modern Architecture) ==="

# Run the CLI tool with input redirection inside Docker
# We use 'timeout' to ensure it doesn't hang if 'show' blocks (though it runs in a thread)
# However, poc_modern_cli is a GUI app.
# If we run it, it will try to open a window.
# We can use xvfb-run if available, or just rely on the fact that we are in a container with X11 forwarding if set.
# But for automated testing without visual verification, we rely on 'query' output.

# We need to capture stdout
# WORKDIR is /workspace/showbox-studio
# So we need to go to ../showbox
OUTPUT=$(./src/start-dev.sh "cd ../showbox && ./build/bin/poc_modern_cli < test_script.sbx")

echo "--- CLI Output ---"
echo "$OUTPUT"
echo "------------------"

# Verification
if echo "$OUTPUT" | grep -q "John Doe"; then
    echo "[PASS] Query username matches"
else
    echo "[FAIL] Query username failed"
    exit 1
fi

if echo "$OUTPUT" | grep -q "40"; then
    echo "[PASS] Query age matches (set value worked)"
else
    echo "[FAIL] Query age failed"
    exit 1
fi

if echo "$OUTPUT" | grep -q "0"; then
    echo "[PASS] Query newsletter matches (unset checked worked)"
else
    echo "[FAIL] Query newsletter failed"
    exit 1
fi

echo "Integration Test Passed!"

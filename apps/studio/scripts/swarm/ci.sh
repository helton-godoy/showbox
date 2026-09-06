#!/bin/bash
# agent-ci (Swarm Toolbelt 2.0)
# Role: Continuous Integration Check
# Strategy: Gemini CLI (Analysis) + Local Build
# Usage: ./scripts/swarm/ci.sh

echo "[CI] Starting Local Integration Check..."

# 1. Build
echo "[CI] Running Build..."
mkdir -p build && cd build || exit
if cmake .. && make -j$(nproc); then
	echo "[CI] Build Success"
else
	echo "[CI] Build Failed"
	gemini --prompt "/conductor:notify 'CI Failed: Build Error'" >/dev/null 2>&1 || true
	exit 1
fi

# 2. Test
echo "[CI] Running Tests..."
# ctest output
if ctest; then
	echo "[CI] Tests Passed"
else
	echo "[CI] Tests Failed"
	gemini --prompt "/conductor:notify 'CI Failed: Tests Error'" >/dev/null 2>&1 || true
	exit 1
fi

# 3. Gemini Analysis
echo "[CI] Running AI Analysis..."
gemini analyze --diff HEAD~1
EXIT_CODE=$?

if [[ ${EXIT_CODE} -eq 0 ]]; then
	gemini --prompt "/conductor:notify 'CI Pipeline Passed Successfully'" >/dev/null 2>&1 || true
fi

exit "${EXIT_CODE}"

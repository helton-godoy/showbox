#!/bin/bash
set -e

# Ensure we are in project root
if [[ ! -f "CMakeLists.txt" ]]; then
	echo "Please run from showbox project root"
	exit 1
fi

# Build Legacy (QMake)
echo "=== Building Legacy Showbox ==="
pushd apps/runtime
if command -v qmake6 &>/dev/null; then
	qmake6 showbox.pro
else
	echo "qmake6 not found, skipping legacy build (assuming it is built or environment issue)"
fi
if [[ -f Makefile ]]; then
	jobs_nproc="$(nproc)"
	make -j"${jobs_nproc}"
fi
popd

# Build Target (CMake)
echo "=== Building Target Showbox ==="
mkdir -p build
pushd build
if command -v cmake &>/dev/null; then
	cmake ..
	jobs_nproc="$(nproc)"
	make -j"${jobs_nproc}"
else
	echo "cmake not found, skipping target build"
fi
popd

echo "=== Launching Comparison ==="
echo "Legacy (expecting 'Legacy Button'):"
if [[ -f apps/runtime/bin/showbox-legacy ]]; then
	echo 'add pushbutton "Legacy Button" btn1' | ./apps/runtime/bin/showbox-legacy &
	LEGACY_PID=$!
else
	echo "Legacy binary not found."
fi

echo "Target (expecting 'Target Button'):"
if [[ -f build/bin/showbox ]]; then
	echo 'add pushbutton "Target Button" btn2' | ./build/bin/showbox &
	TARGET_PID=$!
else
	echo "Target binary not found."
fi

echo "Press Enter to close applications..."
read -r
if [[ -n ${LEGACY_PID} ]]; then kill "${LEGACY_PID}" 2>/dev/null; fi
if [[ -n ${TARGET_PID} ]]; then kill "${TARGET_PID}" 2>/dev/null; fi

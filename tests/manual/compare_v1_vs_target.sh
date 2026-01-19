#!/bin/bash
set -e

# Ensure we are in project root
if [[ ! -f "CMakeLists.txt" ]]; then
	echo "Please run from showbox project root"
	exit 1
fi

# Build Legacy (QMake)
echo "=== Building Legacy Showbox ==="
pushd src/code/showbox
if command -v qmake6 &>/dev/null; then
	qmake6 showbox.pro
else
	echo "qmake6 not found, skipping legacy build (assuming it is built or environment issue)"
fi
if [[ -f Makefile ]]; then
	make -j$(nproc)
fi
popd

# Build Target (CMake)
echo "=== Building Target Showbox ==="
mkdir -p build
pushd build
if command -v cmake &>/dev/null; then
	cmake ..
	make -j$(nproc)
else
	echo "cmake not found, skipping target build"
fi
popd

echo "=== Launching Comparison ==="
echo "Legacy (expecting 'Legacy Button'):"
if [[ -f src/code/showbox/bin/showbox-legacy ]]; then
	echo 'add pushbutton "Legacy Button" btn1' | ./src/code/showbox/bin/showbox-legacy &
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
read
if [[ -n ${LEGACY_PID} ]]; then kill "${LEGACY_PID}" 2>/dev/null; fi
if [[ -n ${TARGET_PID} ]]; then kill "${TARGET_PID}" 2>/dev/null; fi

#!/bin/bash
# scripts/evaluation/run_benchmark.sh
# Automated Benchmark for OpenCode Free Models
# Authored by Antigravity

OUTPUT_DIR="scripts/evaluation/opencode"
mkdir -p "${OUTPUT_DIR}"

PROMPT_FILE="${OUTPUT_DIR}/prompt.txt"
echo "Create a C++17 class 'StringUtils' using Qt6. Include static methods for 'toCamelCase' and 'toSnakeCase'. Provide a main function running basic unit tests with QTest." >"${PROMPT_FILE}"
PROMPT_CONTENT=$(cat "${PROMPT_FILE}")

echo "============================================="
echo "Starting OpenCode Benchmark: The Free 4"
echo "============================================="
echo "Task: ${PROMPT_CONTENT}"
echo "============================================="

# 1. MiniMax
echo "Testing MiniMax M2.1..."
opencode run "${PROMPT_CONTENT}" --model opencode/minimax-m2.1-free >"${OUTPUT_DIR}/result_minimax.cpp" || echo "Failed MiniMax"

# 2. GLM
echo "Testing GLM-4.7..."
opencode run "${PROMPT_CONTENT}" --model opencode/glm-4.7-free >"${OUTPUT_DIR}/result_glm.cpp" || echo "Failed GLM"

# 3. Grok
echo "Testing Grok Code..."
opencode run "${PROMPT_CONTENT}" --model opencode/grok-code >"${OUTPUT_DIR}/result_grok.cpp" || echo "Failed Grok"

# 4. Big Pickle
echo "Testing Big Pickle..."
opencode run "${PROMPT_CONTENT}" --model opencode/big-pickle >"${OUTPUT_DIR}/result_bigpickle.cpp" || echo "Failed Big Pickle"

echo "============================================="
echo "Benchmark Complete."
echo "Check results in ${OUTPUT_DIR}"

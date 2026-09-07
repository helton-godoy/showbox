#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
STUDIO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
WORKSPACE_ROOT="$(cd "${STUDIO_ROOT}/.." && pwd)"

IMAGE_NAME="showbox-studio-dev:latest"

echo "=== Showbox Studio Packaging Orchestrator ==="
echo "Workspace: ${WORKSPACE_ROOT}"

# Garantir que a imagem existe (constrói se não existir)
"${STUDIO_ROOT}/src/start-dev.sh" bash -c "true"

# Rodar o build
docker run --rm \
	-v "${WORKSPACE_ROOT}:/workspace" \
	-e HOME=/tmp \
	-e APPIMAGE_EXTRACT_AND_RUN=1 \
	-w "/workspace/showbox-studio" \
	"${IMAGE_NAME}" \
	bash packaging/appimage/build.sh

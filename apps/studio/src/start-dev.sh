#!/bin/bash
set -e

# Configurações da Imagem
IMAGE_NAME="showbox-studio-dev:latest"
DOCKERFILE_PATH="src/dev.Dockerfile"

# Diretórios
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
# IMPORTANTE: Montamos o diretório PAI (onde estão 'showbox' e 'showbox-studio')
# para que o CMake consiga resolver '../showbox' dentro do container.
WORKSPACE_ROOT="$(cd "${PROJECT_ROOT}/.." && pwd)"
PROJECT_DIR_NAME="$(basename "${PROJECT_ROOT}")"

echo "=== Showbox Studio Dev Environment ==="
echo "Building Docker image..."
docker build -t "${IMAGE_NAME}" -f "${DOCKERFILE_PATH}" "${PROJECT_ROOT}"

# Configuração de Usuário (para evitar arquivos root)
USER_ID=$(id -u)
GROUP_ID=$(id -g)

# Configuração de GUI (X11)
DOCKER_ARGS=()
if [[ -n ${DISPLAY} ]]; then
	echo "Enabling GUI support (X11)..."
	DOCKER_ARGS+=(-e "DISPLAY=${DISPLAY}")
	DOCKER_ARGS+=(-v "/tmp/.X11-unix:/tmp/.X11-unix")

	# Tenta configurar cookies XAuth se necessário (opcional, mas ajuda em alguns sistemas)
	if [[ -f "${HOME}/.Xauthority" ]]; then
		DOCKER_ARGS+=(-v "${HOME}/.Xauthority:/tmp/.Xauthority")
		DOCKER_ARGS+=(-e "XAUTHORITY=/tmp/.Xauthority")
	fi
fi

# Modo Interativo
INTERACTIVE_FLAGS="-i"
if [[ -t 0 ]]; then
	INTERACTIVE_FLAGS="-it"
fi

echo "Starting container..."
echo "Mounting Workspace: ${WORKSPACE_ROOT} -> /workspace"
echo "Working Directory: /workspace/${PROJECT_DIR_NAME}"

# Se nenhum comando for passado, abre o bash. Caso contrário, executa o comando.
CMD="${*:-bash}"

docker run --rm "${INTERACTIVE_FLAGS}" \
	-v "${WORKSPACE_ROOT}:/workspace" \
	-u "${USER_ID}:${GROUP_ID}" \
	-e HOME=/tmp \
	-w "/workspace/${PROJECT_DIR_NAME}" \
	"${DOCKER_ARGS[@]}" \
	"${IMAGE_NAME}" \
	bash -c "${CMD}"

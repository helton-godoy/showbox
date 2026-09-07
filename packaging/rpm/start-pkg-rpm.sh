#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd "$(dirname "$0")" && pwd)"
project_root="$(cd "${script_dir}/../.." && pwd)"
container_engine="${CONTAINER_ENGINE:-podman}"
image_name="showbox-rpm"

"${container_engine}" build \
	-f "${script_dir}/fedora.Dockerfile" \
	-t "${image_name}" \
	"${script_dir}"

"${container_engine}" run --rm \
	-v "${project_root}:/build/showbox:rw,Z" \
	-w /build/showbox \
	"${image_name}" \
	bash ./packaging/rpm/build.sh

ls -la "${project_root}/dist/"*.rpm

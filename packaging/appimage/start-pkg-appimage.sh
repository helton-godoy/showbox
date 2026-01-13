#!/bin/bash
#
# ShowBox AppImage Builder
# Orchestrates Docker build for AppImage
#

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
DIST_DIR="$PROJECT_ROOT/dist"

GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

log_info() { echo -e "${GREEN}[INFO]${NC} $1"; }
log_warn() { echo -e "${YELLOW}[WARN]${NC} $1"; }

mkdir -p "$DIST_DIR"

log_info "Building Docker image for AppImage..."
docker build -f "$SCRIPT_DIR/appimage.Dockerfile" -t "showbox-appimage" "$SCRIPT_DIR"

log_info "Building AppImage inside Docker container..."
docker run --rm \
    --privileged \
    -v "$PROJECT_ROOT:/build:rw" \
    -e APPIMAGE_EXTRACT_AND_RUN=1 \
    -w /build \
    "showbox-appimage" \
    bash -c "./packaging/appimage/build.sh"

log_info "=== Build Complete ==="
ls -la "$DIST_DIR"/*.AppImage 2>/dev/null || log_warn "No AppImage found"

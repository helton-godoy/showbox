FROM ubuntu:24.04

# Imagem de build/CI efemera, sem processo em background: healthcheck desabilitado de proposito.
HEALTHCHECK NONE

# Avoid interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install build and runtime dependencies
# Build tools, Qt6 deps and FUSE come from the pinned base (ubuntu:24.04);
# version-pinning every distro package would make this image brittle to maintain.
# hadolint ignore=DL3008
RUN apt-get update -qq && apt-get install -qqy --no-install-recommends \
    # Build tools
    build-essential \
    cmake \
    git \
    python3 \
    wget \
    ca-certificates \
    file \
    xvfb \
    xauth \
    weston \
    # FUSE for AppImage
    libfuse2 \
    # Qt6 development packages
    qt6-base-dev \
    qt6-wayland \
    qt6-charts-dev \
    libqt6charts6-dev \
    libqt6svg6-dev \
    libqt6opengl6-dev \
    libgl-dev \
    libopengl-dev \
    # Qt6 runtime libraries
    libqt6charts6 \
    libqt6svg6 \
    libqt6opengl6 \
    libqt6widgets6 \
    libqt6gui6 \
    libqt6core6t64 \
    # Icon themes
    adwaita-icon-theme \
    hicolor-icon-theme \
    && rm -rf /var/lib/apt/lists/*

# Download linuxdeploy and Qt plugin
WORKDIR /tools
RUN wget -q https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage \
    && chmod +x linuxdeploy-x86_64.AppImage \
    && wget -q https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage \
    && chmod +x linuxdeploy-plugin-qt-x86_64.AppImage

# Set environment for Qt
WORKDIR /build

# checkov:skip=CKV_DOCKER_3:builder CI efemero roda como root para gerar artefatos e montar o volume do host
# Default command: build AppImage
CMD ["bash", "-c", "./packaging/appimage/build.sh"]
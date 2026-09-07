FROM debian:trixie-slim

# Imagem de build/CI efemera, sem processo em background: healthcheck desabilitado de proposito.
HEALTHCHECK NONE

# Avoid interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Update and install dependencies
# Build dependencies (dev packages)
# Pacotes vindos do repositorio da distro fixado pelo tag da base (debian:trixie-slim);
# pinagem por pacote e inviavel de manter.
# hadolint ignore=DL3008
RUN apt-get update -qq && apt-get install -qqy --no-install-recommends \
    build-essential \
    cmake \
    git \
    python3 \
    qmake6 \
    qt6-base-dev \
    qt6-charts-dev \
    libqt6charts6-dev \
    libqt6svg6-dev \
    libqt6opengl6-dev \
    libgl-dev \
    libopengl-dev \
    # Runtime libraries
    libqt6charts6 \
    libqt6svg6 \
    libqt6opengl6 \
    libqt6widgets6 \
    libqt6gui6 \
    libqt6core6t64 \
    # XCB dependencies for GUI rendering
    libxcb-cursor0 \
    libxcb-xinerama0 \
    libxcb-icccm4 \
    libxcb-image0 \
    libxcb-keysyms1 \
    libxcb-randr0 \
    libxcb-render-util0 \
    libxcb-shape0 \
    libxkbcommon-x11-0 \
    # Icon themes for GUI icon support
    adwaita-icon-theme \
    hicolor-icon-theme \
    # Utilities
    git \
    locales \
    curl \
    wget \
    pandoc \
    clang-format \
    doxygen \
    debhelper \
    && rm -rf /var/lib/apt/lists/*

# Generate locale
RUN sed -i '/en_US.UTF-8/s/^# //g' /etc/locale.gen && locale-gen
ENV LANG=en_US.UTF-8
ENV LANGUAGE=en_US:en
ENV LC_ALL=en_US.UTF-8

# Set working directory
WORKDIR /app

# checkov:skip=CKV_DOCKER_3:container de dev interativo roda como root dentro do workspace montado do host
# Default command
CMD ["/bin/bash"]
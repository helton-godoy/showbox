FROM debian:trixie-slim

# Avoid interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install build and runtime dependencies for ShowBox
RUN apt-get update -qq && apt-get install -qqy \
    # Build tools
    build-essential \
    qmake6 \
    debhelper \
    devscripts \
    fakeroot \
    lintian \
    # Qt6 development packages
    qt6-base-dev \
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

# Set working directory
WORKDIR /build

# Default command: build the package
CMD ["bash", "-c", "cd /build && ./packaging/deb/build.sh"]
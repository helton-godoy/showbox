FROM debian:bookworm-slim

# Evitar prompts interativos
ENV DEBIAN_FRONTEND=noninteractive

# Instalar dependências de build e runtime
# Inclui Qt6 (Base, Charts, SVG) e ferramentas (CMake, Ninja, GDB)
RUN apt-get update -qq && apt-get install -qqy --no-install-recommends \
    build-essential \
    cmake \
    ninja-build \
    gdb \
    git \
    qt6-base-dev \
    qt6-charts-dev \
    libqt6charts6-dev \
    libqt6svg6-dev \
    libqt6opengl6-dev \
    libgl-dev \
    libopengl-dev \
    # Dependências de Runtime Qt
    libqt6charts6 \
    libqt6svg6 \
    libqt6opengl6 \
    libqt6widgets6 \
    libqt6gui6 \
    libqt6core6 \
    # Suporte a X11/GUI
    libxcb-cursor0 \
    libxcb-xinerama0 \
    libxcb-icccm4 \
    libxcb-image0 \
    libxcb-keysyms1 \
    libxcb-randr0 \
    libxcb-render-util0 \
    libxcb-shape0 \
    libxkbcommon-x11-0 \
    libx11-xcb1 \
    # Ícones e utilitários
    adwaita-icon-theme \
    hicolor-icon-theme \
    locales \
    curl \
    wget \
    file \
    clang-format \
    doxygen \
    && rm -rf /var/lib/apt/lists/*

# Gerar locale
RUN sed -i '/en_US.UTF-8/s/^# //g' /etc/locale.gen && locale-gen
ENV LANG=en_US.UTF-8
ENV LANGUAGE=en_US:en
ENV LC_ALL=en_US.UTF-8

# Diretório de trabalho padrão (será sobrescrito pelo script)
WORKDIR /workspace/showbox-studio

# Comando padrão
CMD ["/bin/bash"]

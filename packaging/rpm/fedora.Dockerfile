FROM fedora:latest

# Instala apenas o estritamente necessário para o build
# rpm-build: Ferramentas de empacotamento
# make, gcc-c++: Compilação
# qt6-qtbase-devel: Dependência de build do projeto
RUN dnf install -y --setopt=install_weak_deps=False \
    rpm-build \
    cmake \
    git \
    python3 \
    gcc-c++ \
    qt6-qtbase-devel \
    qt6-qtcharts-devel \
    qt6-qtsvg-devel \
    && dnf clean all

WORKDIR /build

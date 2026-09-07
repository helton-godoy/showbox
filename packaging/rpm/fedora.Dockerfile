FROM fedora:46

# Imagem de build/CI efemera, sem processo em background: healthcheck desabilitado de proposito.
HEALTHCHECK NONE

# Instala apenas o estritamente necessário para o build
# rpm-build: Ferramentas de empacotamento
# make, gcc-c++: Compilação
# qt6-qtbase-devel: Dependência de build do projeto
# Todos os pacotes vêm do repositório do Fedora fixado pelo tag da base (46);
# pinagem por pacote (DL3041) é inviável de manter à medida que o distro evolui.
# hadolint ignore=DL3041
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

# checkov:skip=CKV_DOCKER_3:imagem de build/CI roda como root para gerar artefatos e montar o volume do host
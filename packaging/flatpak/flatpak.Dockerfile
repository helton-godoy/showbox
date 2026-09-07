# Dockerfile otimizado para construção de pacotes Flatpak
# Base: Debian Stable Slim
FROM debian:stable-slim

# Imagem de build/CI efemera, sem processo em background: healthcheck desabilitado de proposito.
HEALTHCHECK NONE

ENV DEBIAN_FRONTEND=noninteractive

# Instala apenas flatpak-builder e dependências básicas de rede
# Pacotes vindos do repositorio da distro fixado pelo tag da base (debian:stable-slim);
# pinagem por pacote e inviavel de manter.
# hadolint ignore=DL3008
RUN apt-get update && apt-get install -y --no-install-recommends \
    flatpak-builder \
    ca-certificates \
    git \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /build

# checkov:skip=CKV_DOCKER_3:imagem de build/CI roda como root para gerar artefatos e montar o volume do host
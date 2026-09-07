# ADR 0005 — Empacotamento e pipeline de release

Estado: implementado na SB-005, sujeito aos testes de instalação desta tarefa.

A release distribui dois pacotes por formato: showbox (motor) e showbox-studio
(editor visual), sem conflitos de caminho. O pacote do Studio depende do do motor
(deb: Depends: showbox; rpm: Requires: showbox), refletindo a dependência de
execução. Cada formato tem um subdiretório próprio em packaging/ e segue o
convencionado pelo sistema (deb via dh/dpkg-buildpackage, rpm via spec,
AppImage via linuxdeploy).

Desktop files e ícones são canônicos em packaging/desktop e packaging/icon e
instalados via CMake (install com COMPONENT: showbox-engine e showbox-studio),
evitando duplicação por formato. dpkg-shlibdeps registra dependências de Qt
diferentes por distro (libqt6core6t64 no Ubuntu vs libqt6core6 no Debian), então
os debs são gerados por imagem base e depositados em dist/ubuntu e dist/debian.

O AppImage empacota só o Studio. O fluxo roda em container porque o linuxdeploy
é baixado e executado dentro da imagem base, com acoplamento ao runtime Qt;
comandos de instalação do build (cmake --install --component) vencem dentro do
container porque o build dir embute caminhos absolutos /build.

Os artefatos são totalmente construídos em container, mantendo a integração
multiplataforma e sem exigir as ferramentas de empacotamento no host. Ferramentas
locais de verificação são instaladas por tools/setup e conferidas por just doctor.

Testes de instalação em container limpo (podman/docker, via
tests/installation/install_smoke.sh) validam a instalação real do usuário por
formato e distro: motor com --version/--help e Studio com --version, além de
garantir que o pacote do motor não introduz o binário do Studio. Entradas de
.gitignore mantêm dist/ e as ferramentas baixadas fora do repositório.

A pipeline de release (.github/workflows/release.yml) constrói deb ubuntu+debian,
rpm e appimage, roda o smoke de instalação de cada artefato e publica um draft de
release com os arquivos em tags v\*. O CI diário valida build e árvore de
instalação via CMake.

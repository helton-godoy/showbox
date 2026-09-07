# Integração Bash

lib/runtime.sh é a implementação única do transporte da SB-002. O gerador a
incorpora por recurso Qt; a demonstração de desenvolvimento a carrega com source.
Não copie e edite versões independentes dessa biblioteca.

O protocolo de execução está documentado em docs/architecture/adr/0002-shell-dispatch.md.
Os testes tst_ShellFlow exercitam uma entrada compilada do motor real com harness
Qt que clica em seus componentes. Nenhuma opção de automação foi adicionada ao
binário distribuído showbox.

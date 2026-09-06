# Showbox

Showbox é um ambiente Qt6 para criar ferramentas visuais com shell script como
back-end principal, sucessor espiritual do Kommander.

- **showbox**: motor que recebe comandos pela entrada e emite eventos pela saída.
- **showbox-studio**: editor visual experimental, agora no mesmo repositório.

## Desenvolvimento

```sh
just doctor
just setup       # Debian/Ubuntu e derivados; instala dependências com sudo
just build
just test
just run-studio
```

Em uma máquina sem just, comece com `bash tools/setup/debian.sh`.
Just é opcional e não é instalado pelo setup.

Sem just: `cmake --preset dev`, `cmake --build --preset dev` e
`ctest --preset dev`. O build fica em `build/dev` de cada worktree.

## Estado

A consolidação é incremental. O motor vem de `integration/showbox-v1` e o Studio
vem da `develop` do antigo showbox-studio. Geração shell, preview e persistência
completa de ações ainda precisam de validação e correções; o editor é experimental.

- [Roadmap oficial](docs/project/ROADMAP.md)
- [Desenvolvimento e testes](docs/development/README.md)
- [Usuário](docs/user/README.md)
- [Manual CLI](docs/cli/README.md)
- [Origem e migração](docs/migration/README.md)
- [Trabalho com agentes e worktrees](docs/development/worktrees.md)

O histórico e os créditos GPLv3+ dos projetos de origem são preservados.

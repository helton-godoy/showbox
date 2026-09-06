# Desenvolvimento

Requisitos: compilador C++17, CMake 3.21+, Ninja, Qt6 6.4+ (Widgets, Charts, Svg,
Test), Bash e Python3 3.9+ para ferramentas de repositório. Just é recomendado.

```sh
just doctor
just setup
just build
just test
```

O setup suporta Debian/Ubuntu e derivados, é repetível e instala pacotes por apt.
Não altera configuração Git pessoal, credenciais ou serviços. Em outras
plataformas, instalar as dependências equivalentes e executar doctor.

CMakePresets.json é compartilhado; CMakeUserPresets.json é pessoal e ignorado.
Cada worktree usa seu build/dev. Não reutilizar o cache CMake de outra árvore.

## Compatibilidade legado

```sh
bash tests/compatibility/build_legacy_oracle.sh "$PWD" "$PWD/build/oracle"
cmake --preset dev -DSHOWBOX_LEGACY_ORACLE="$PWD/build/oracle/bin/showbox-legacy"
cmake --build --preset dev
ctest --preset dev
```

Qt Test executa com plataforma offscreen. X11/Wayland e testes reais de pacotes
são verificações adicionais de release, não substituídos por offscreen.

Para compilar somente o motor: `cmake --preset dev -DSHOWBOX_BUILD_STUDIO=OFF`.
Reativar o Studio antes da validação de integração.

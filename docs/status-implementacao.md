# Status da implementação

**Atualizado em:** 16/08/2026
**Linha de integração:** `integration/showbox-v1`

| Área          | Estado atual                                                                |
| ------------- | --------------------------------------------------------------------------- |
| Build moderno | CMake explícito; compila biblioteca, core, CLI e testes                     |
| Parser        | Moderno ativo; quoting, aliases e comandos principais cobertos              |
| Widgets       | Catálogo documentado criado por `showbox-ui`                                |
| Runtime       | stdin no event loop Qt; widgets manipulados na thread GUI                   |
| Testes        | 9 suítes unitárias, contrato CLI e comparação dourada opcional com o legado |
| Backends      | Smoke tests aprovados em X11/Xvfb e Wayland/Weston                          |
| Pacotes       | DEB, RPM e AppImage gerados; plugins Wayland incluídos no AppImage          |

Os diretórios `legacy/v1_monolith` e `legacy/v2_incomplete` permanecem somente
como referência histórica e não participam do alvo moderno. O status detalhado,
gates restantes e regras para múltiplos agentes ficam em
[`project/v1/STATUS.md`](project/v1/STATUS.md).

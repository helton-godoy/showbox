# Status da implementação

**Atualizado em:** 11/09/2026
**Linha de integração:** `main` via pull request

| Área          | Estado atual                                                                |
| ------------- | --------------------------------------------------------------------------- |
| Build moderno | CMake explícito; compila biblioteca, core, CLI e testes                     |
| Parser        | Moderno ativo; quoting, aliases e comandos principais cobertos              |
| Widgets       | Catálogo documentado criado por `showbox-ui`                                |
| Runtime       | stdin no event loop Qt; widgets manipulados na thread GUI                   |
| Testes        | 9 suítes unitárias, contrato CLI e comparação dourada opcional com o legado |
| Backends      | Smoke tests aprovados em X11/Xvfb e Wayland/Weston                          |
| Pacotes       | RC.4 publicado; DEB, RPM e AppImage com smokes de instalação aprovados      |

Os diretórios `legacy/v1_monolith` e `legacy/v2_incomplete` permanecem somente
como referência histórica e não participam do alvo moderno. O status detalhado,
gates restantes e regras para múltiplos agentes ficam em
[`project/v1/STATUS.md`](project/v1/STATUS.md).

A SB-011 encerrou a branch intermediária `integration/showbox-v1`. A `main` é a
única branch permanente de desenvolvimento e recebe mudanças somente por pull
request com `build-test`, `sanitizers` e `trunk-check` obrigatórios.

O primeiro candidato público é o prerelease `v1.0.0-rc.4`, produzido a partir
da `main` consolidada até a SB-011. O RC.3 permanece apenas como draft histórico.

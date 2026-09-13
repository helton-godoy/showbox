# Status da implementação

**Atualizado em:** 13/09/2026
**Linha de integração:** `main` via pull request

| Área          | Estado atual                                                                |
| ------------- | --------------------------------------------------------------------------- |
| Build moderno | CMake explícito; compila biblioteca, core, CLI e testes                     |
| Parser        | Moderno ativo; quoting, aliases e comandos principais cobertos              |
| Widgets       | Catálogo documentado criado por `showbox-ui`                                |
| Runtime       | stdin no event loop Qt; widgets manipulados na thread GUI                   |
| Testes        | 9 suítes unitárias, contrato CLI e comparação dourada opcional com o legado |
| Backends      | Smoke tests aprovados em X11/Xvfb e Wayland/Weston                          |
| Pacotes       | RC.6 publicado (prerelease, com SB-017); smokes aprovados; RC.5 histórico   |
| Automação     | SB-017 integrada: socket JSON-RPC local, `showbox-studioctl` e MCP          |

Os diretórios `legacy/v1_monolith` e `legacy/v2_incomplete` permanecem somente
como referência histórica e não participam do alvo moderno. O status detalhado,
gates restantes e regras para múltiplos agentes ficam em
[`project/v1/STATUS.md`](project/v1/STATUS.md).

A SB-011 encerrou a branch intermediária `integration/showbox-v1`. A `main` é a
única branch permanente de desenvolvimento e recebe mudanças somente por pull
request com `build-test`, `sanitizers` e `trunk-check` obrigatórios.

O candidato público avaliado foi o prerelease `v1.0.0-rc.6`, produzido a
partir da `main` com a SB-017 integrada (socket JSON-RPC, `showbox-studioctl`,
MCP). A SB-019 concluiu os gates A–E, mas encontrou bloqueio no fluxo gráfico
de fechamento com alterações: o Studio encerrou sem confirmação de descarte
observável. A recomendação operacional é corrigir e produzir `rc.7`; nenhuma
tag estável foi criada. O RC.5 permanece publicado como histórico, o RC.4
como histórico e o RC.3 como draft.

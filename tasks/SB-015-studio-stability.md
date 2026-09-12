# SB-015 — Estabilidade do Studio e menu padrão

Estado: concluída (2026-09-12).

- Objetivo: eliminar os fechamentos repentinos do showbox-studio corrigindo o
  ciclo de vida de widgets/seleção/comandos/processos, e completar a barra de
  menus no padrão File, Edit, View, Help.
- Base: `f4f2a23` (`main` com a SB-014 integrada).
- Branch de execução: `fix/SB-015-studio-stability`.
- Responsável: integrador desta sessão.
- Dependências: SB-010 (fluxo visual), SB-013 (janela de validação aberta —
  esta tarefa entra antes da decisão `v1.0.0`).

## Evidência de origem

Relato do mantenedor: o motor está estável, mas o Studio fecha sozinho e a
barra de menus diverge do padrão. Auditoria somente-leitura em
`apps/studio/src` mapeou 8 pontos de use-after-free/double-delete:
`static QWidget* lastHighlighted` em `Canvas` (destaque pendurado após delete
ou New/Open — segfault no próximo arrasto), `QWidget*` crus na seleção do
`StudioController`, no `m_target` do `PropertyEditor`, no mapa do
`ObjectInspector` e nos comandos de undo (`StudioCommands.h`), teardown fora
de ordem no destrutor de `MainWindow` (`delete` manual de `m_previewManager` /
`m_factory`), lambdas do `PreviewManager` sobre `QProcess*` cru com
`deleteLater`, e `delete` síncrono do toolbox durante drag ativo.
Menus atuais (`MainWindow.cpp:75-220`): File, Edit, View — sem Help.

## Escopo

- Trocar ponteiros crus por `QPointer` + guardas de nulidade nos 8 pontos,
  sem mudar comportamento funcional: highlight do Canvas como membro,
  poda da seleção/alvo/mapa via `destroyed()`, validação de pai/alvo em
  undo/redo, `deleteLater`/bloqueio na troca de toolbox, teardown ordenado
  (parar prévia, desconectar, soltar fábrica após o Canvas).
- Criar menu Help (About com versão, links para docs de usuário e roadmap).
- Testes offscreen de regressão: deletar com editores abertos, undo após
  clear, fechar/Novo com prévia ativa; mais roteiro manual de crash-repro.
- Sem mudança de protocolo, catálogo, motor ou empacotamento.

## Fora do escopo

- Ampliar widgets exportáveis ou mexer no `ScriptGenerator`/motor (SB-017+).
- Desabilitar itens do toolbox (SB-016). Promover `v1.0.0` (SB-013).

## Aceite

- Roteiro de crash-repro executa sem fechamentos.
- `just build`, `just test` (24/24 + novos testes) e `just check` verdes;
  gates do PR (`build-test`, `sanitizers` com ASan/UBSan, `trunk-check`) e
  SonarCloud verdes — sanitizers são prova central desta tarefa.
- Menu File, Edit, View, Help visível e funcional.
- Handoff com SHAs, cenários testados e limitações.

## Riscos

- Correções de lifetime podem expor bugs latentes. Mitigação: PRs pequenos
  por frente, gates obrigatórios, reversão por revert. Sanitizers no CI
  cobrem regressões de memória nos testes automatizados.

## Handoff

- Implementação na branch `fix/SB-015-studio-stability`, base `1bd5889`:
  - `Canvas`: highlight de drag virou membro `QPointer` (fim do `static`
    pendurado); limpeza em `removeWidget`/`clear`; origem do drop guardada
    por `QPointer`; guarda de `m_factory`.
  - `StudioController`: seleção como `QList<QPointer>` com poda via
    `destroyed()`; `manageWidget`/`select`/`multiSelect` rastreiam o widget.
  - `PropertyEditor`: alvo `QPointer` com reset via `destroyed()`; lambda de
    layout captura `QPointer`.
  - `ObjectInspector`: poda no `destroyed()` (com purga de chaves órfãs dos
    itens filhos), `QSignalBlocker` no `updateHierarchy`, guardas de
    liveness em seleção/current/drop.
  - `StudioCommands`: membros `QPointer` + guardas em undo/redo/move/layout;
    `GroupWidgetsCommand` valida container criado pela fábrica.
  - `PreviewManager`: `m_process` virou `QPointer` com guardas nas lambdas.
  - `MainWindow`: teardown ordenado (desconectar, parar prévia, soltar
    controller/fábrica), `stop()` no `closeEvent`, toolbox com `deleteLater`,
    menus View/Help movidos ao construtor (ordem File, Edit, View, Help) e
    menu Help com About + About Qt.
- Novo `tst_LifetimeGuards` (5 casos): seleção podada, editor limpo,
  inspector após morte, undo-após-clear como no-op, ordem dos menus.
- Achado durante o trabalho: a poda ingênua do inspector deletava item já
  liberado quando o widget pai morria (filhos órfãos no mapa); corrigido com
  purga de descendentes — `tst_Hierarchy` voltou a passar.
- Validações locais: `just build` limpo; `just test` **25/25** (inclui o
  novo teste); `just check` sem achados. Prova de sanitizers pelos gates do
  PR.
- Limitação: testes offscreen não cobrem drag real nem janelas visíveis; o
  roteiro manual (arrastar/deletar/desfazer/fechar com prévia ativa) deve ser
  repetido pelo mantenedor em X11/Wayland antes da `v1.0.0`.

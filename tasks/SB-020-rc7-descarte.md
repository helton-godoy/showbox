# SB-020 — Corretivo rc.7: proteção contra perda no descarte (Studio)

Estado: em execução.

- Objetivo: corrigir o bloqueador da SB-019 (fechamento sem diálogo de
  descarte observável) e publicar `v1.0.0-rc.7`, com repetição da validação.
- Base: `9ef3d677` (`main` com a SB-019 integrada).
- Branch: `fix/SB-020-descarte`.
- Dependências: SB-019 (achado do bloco F), SB-015 (ciclo de vida), SB-017
  (automação/snapshot).
- Causa raiz: `MainWindow::hasUnsavedChanges()` considera apenas
  `m_actionsModified || !undoStack()->isClean()`. Edições diretas no canvas
  (célula de tabela, texto, spin, combo, toggle) alteram o QWidget vivo sem
  empilhar undo e sem marcar `m_actionsModified`; após `Undo` do agrupamento,
  a pilha volta a limpa com o modelo diferente do salvo → `closeEvent`,
  `onNewClicked` e `onOpenClicked` fecham sem perguntar e a alteração é
  perdida. Evidência SB-019: tabela `1→42`, `Group in Frame`, `Undo`,
  `close` sem diálogo (`61-f-discard-dialog.png` mostra só o preview).

## Escopo

- Detecção de sujeira por impressão digital do modelo (`ProjectWidgetMapper`)
  sem geometria volátil (`x/y/width/height`), com `OU` à lógica atual, em
  `hasUnsavedChanges()`; atualizar a impressão em `markDocumentSaved()` e em
  todos os pontos que limpam/salvam (new/open/save/demo/automação/construtor).
- Diálogo de descarte padrão `Save/Discard/Cancel`: `Save` abre salvar e só
  prossegue se salvar; `Discard` prossegue; `Cancel` aborta. Aplicar em
  `closeEvent`, `onNewClicked`, `onOpenClicked` e `onDemoClicked`. Manter o
  comportamento `force` da automação sem GUI.
- Testes offscreen de regressão sem modal: projeto limpo; push marca sujo;
  save limpa; edição direta de tabela/texto marca sujo; cenário SB-019
  (load→edita tabela→agrupa→desagrupa/undo→ainda sujo); save volta a limpo.
- `just doctor`, `just build`, `just test`, `just check` verdes; PR com
  `build-test`, `sanitizers`, `trunk-check` e SonarCloud.

## Fora do escopo

- Desabilitar edição direta no canvas (fica como backlog/UX; a impressão
  digital já impede perda silenciosa).
- Novos widgets, mudanças de protocolo/catálogo/motor/empacotamento.
- Publicar `v1.0.0` nesta tarefa (só `rc.7` + repetir SB-019).

## Aceite

- Fechar/Novo/Abrir com edição direta (incluindo cenário SB-019) exige
  confirmação `Save/Discard/Cancel`; `Cancel` não perde dados.
- `just test` com os novos casos verdes; sem regressão nos 28 existentes.
- `rc.7` publicado como prerelease e SB-019 repetida sobre ele.

## Handoff

- Implementação na branch `fix/SB-020-descarte`, base `9ef3d677`:
  - `MainWindow.h/cpp`: impressão digital do modelo (`ProjectWidgetMapper`,
    sem `x/y/width/height`) em `OU` com `m_actionsModified`/pilha;
    `updateSavedFingerprint()` no construtor, `markDocumentSaved()` e
    abertura GUI (que não a chamava); diálogo elevado a
    `Save/Discard/Cancel` com `saveProjectInteractive()` em
    `closeEvent`/`onNewClicked`/`onOpenClicked`/`onDemoClicked`; automação
    `force` inalterada (sem GUI).
  - `tst_DiscardProtection` (9 casos): janela limpa, push suja, save limpa,
    edição direta de tabela/texto suja, cenário SB-019 segue sujo após undo,
    `Discard` fecha, `Cancel` mantém, close limpo sem diálogo.
- Commit `1b0814f` (fix) + `8dff7c8` (teste sem orfao p/ LSan). Validações
  locais: `ctest` **29/29** (28 existentes + nova suíte), `trunk check` sem
  issues, ASan/LSan locais limpos nas suítes com `MainWindow`.
- Limitação: edição direta no canvas continua permitida (vira sujeira e
  pede confirmação, sem perda silenciosa); desabilitá-la fica como
  backlog/UX. `rc.7` + repetição da SB-019 pendentes após integrar.

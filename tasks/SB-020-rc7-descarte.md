# SB-020 — Corretivo rc.7: proteção contra perda no descarte (Studio)

Estado: concluída (rc.7 publicado como prerelease; SB-019 repetida com
recomendação de promover `v1.0.0`).

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
  save limpa; edição direta de tabela/texto marca sujo; regressão
  equivalente ao SB-019 (load→edita tabela→adiciona→undo→ainda sujo;
  `Add/Undo` em vez de `Group/Undo`, mesma condição causal: pilha limpa com
  modelo diferente do salvo); save volta a limpo.
- `just doctor`, `just build`, `just test`, `just check` verdes; PR com
  `build-test`, `sanitizers`, `trunk-check` e SonarCloud.

## Fora do escopo

- Desabilitar edição direta no canvas (fica como backlog/UX; a impressão
  digital já impede perda silenciosa).
- Novos widgets, mudanças de protocolo/catálogo/motor/empacotamento.
- Publicar `v1.0.0` nesta tarefa (só `rc.7` + repetir SB-019).

## Aceite

- Fechar/Novo/Abrir com edição direta (incluindo a regressão equivalente
  ao SB-019) exige confirmação `Save/Discard/Cancel`; `Cancel` não perde
  dados. Os testes dos três botões exercitam o fechamento (função
  `confirmDiscardIfModified` compartilhada com Novo/Abrir/Demo); diálogos
  ponta a ponta próprios de Novo/Abrir/Demo ficam como dívida de cobertura.
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
  - `tst_DiscardProtection` (10 casos): janela limpa, push suja, save limpa,
    edição direta de tabela/texto suja, regressão equivalente ao SB-019 segue
    suja após undo, `Discard`/`Cancel`/`Save` de ponta a ponta, close limpo
    sem diálogo.
- Commit `1b0814f` (fix) + `8dff7c8` (teste sem orfao p/ LSan). Validações
  locais: `ctest` **29/29** (28 existentes + nova suíte), `trunk check` sem
  issues, ASan/LSan locais limpos nas suítes com `MainWindow`.
- Integração: PR #46 (merge `e3359f7`; `build-test`, `sanitizers`,
  `trunk-check` e SonarCloud verdes). Achado no caminho: a regressão
  equivalente ao SB-019 no teste deixava widget órfão no undo de Add (vazamento só de teste);
  corrigido com `redo` no próprio teste; registrado como backlog investigar
  o mesmo padrão no produto (`AddWidgetCommand` guarda `QPointer` sem posse).
- Cobertura do caminho Save: PR #49 (`test(studio): cobre caminho Save`,
  merge `830b9fb`; 10º caso `closeWithSavePersistsAndCloses`, ctest 29/29,
  gates verdes). Detalhe: `selectFile()` programático não seleciona o nome
  (Save desabilitado); o teste digita no `QLineEdit` como o usuário.
- Limitação: edição direta no canvas continua permitida (vira sujeira e
  pede confirmação, sem perda silenciosa); desabilitá-la fica como
  backlog/UX.

## Handoff — rc.7 publicado (2026-09-13)

- Preparação: PR #47 (`chore(release): prepara v1.0.0-rc.7 com SB-020`),
  integrado em `927ae6c`; gates verdes. Conversões: app `1.0.0-rc.7`, deb
  `1.0.0~rc7-1`, rpm `1.0.0`/`0.3.rc7`, prerelease `true`
  (`version_contract` 88/88 local).
- Tag `v1.0.0-rc.7` sobre `927ae6c`; workflow run `34769385241`: `deb-ubuntu`,
  `deb-debian`, `rpm`, `appimage`, `install-smoke` e `release` com sucesso;
  draft com os sete assets criado e publicado como prerelease:
  `https://github.com/helton-godoy/showbox/releases/tag/v1.0.0-rc.7`.
- Assets (SHA-256, em `build/sb019r/SHA256SUMS`, ignorado):
  - `b2066176193ea2661de67408b1b7854a60c273033e900ed58fd028f3f29b9830`
    `showbox_1.0.0.rc7-1_ubuntu24.04_amd64.deb`;
  - `8028301eac14bc3cdd6497253d1a8fe51c166771c27a24c17c1d8cb340058553`
    `showbox_1.0.0.rc7-1_debian13_amd64.deb`;
  - `c1a3451923224e09a654b464445e26d842d755c5a734d9eb529e89ef8685d292`
    `showbox-1.0.0-0.3.rc7.fc46.x86_64.rpm`;
  - `2120536c041c8d3c7cd0cf5fa0baa866a8307244395ae44109faed196fabc44c`
    `showbox-studio_1.0.0.rc7-1_ubuntu24.04_amd64.deb`;
  - `fc40fad6a1f52ad4c44e907717fcfd2a7aa4fc44ac85993fd95c781826595664`
    `showbox-studio_1.0.0.rc7-1_debian13_amd64.deb`;
  - `9e0d93daf2ebfa638bc4746bedecbf8220edecf10dd1edeb1d777164f2ad9d48`
    `showbox-studio-1.0.0-0.3.rc7.fc46.x86_64.rpm`;
  - `f8ec2d723a86c017d5883cb0397c116e8b66326fc2bb60ebd0a4de1188bda967`
    `ShowBox-Studio-1.0.0-rc.7-x86_64.AppImage`.
- Smokes locais (podman, contêineres limpos, todos `1.0.0-rc.7`): deb Ubuntu
  OK, deb Debian OK, RPM Fedora OK, AppImage OK (cobrem `showbox-studioctl`
  e MCP). Notas: o smoke deb exige um diretório por distro (ubuntu+debian
  juntos quebram dependências); o smoke rpm lê `VERSION` do checkout local
  (rodar da árvore `rc.7`).
- Limitações: sem assinatura/notarização; GUI inferida por cadeia
  reprodutível em contêiner.

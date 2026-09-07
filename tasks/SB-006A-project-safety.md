# SB-006A — Correção de segurança de projeto: abertura, salvamento e validação

Estado: concluída.

- Objetivo: corrigir os problemas de segurança de dados e validação apontados
  na revisão da linha integrada antes de a SB-006 prosseguir com push/proteção
  de branches/arquivamento: carregar validando antes de tocar o canvas,
  validação recursiva de ações/consultas, salvamento atômico com QSaveFile e
  validação estrutural do formato v2. Não altera protocolo nem runtime.
- Base: integration/showbox-v1 (22f0417, abertura da SB-006).
- Branch prevista: feat/SB-006A-project-safety.
- Responsável: integrador desta sessão.
- Dependências: SB-005. Bloqueia a continuação da SB-006 (só prosseguir com
  push/proteção remota após esta correção integrada).
- Problemas confirmados na revisão (arquivo:linha):
  - P1 — `apps/studio/src/gui/MainWindow.cpp:370` (onOpenClicked): `m_canvas
    ->clear()` ocorre antes de `ProjectSerializer::load` validar; JSON
    corrompido, versão desconhecida ou erro elimina o trabalho aberto sem
    possibilidade de desfazer.
  - P1 — `libs/project/src/ProjectModel.cpp:320`/`323` (validate): chamadas a
    `validateActions` e `validateQueryTargets` apenas nos nós raiz; ações
    inválidas em groupbox/tabs/pages/layouts aninhados escapam da validação
    (contrato SB-003).
  - P1 — `apps/studio/src/core/ProjectSerializer.cpp:18` (save): `QFile` abre
    o destino direto para escrita; falta de espaço/erro parcial truncam o
    arquivo anterior. Usar `QSaveFile` e só commitar após gravar tudo.
  - P2 — `libs/project/src/ProjectModel.cpp:262`/`288` (fromJson v2):
    `format` ausente é aceito e `widgets` ausente ou de tipo incorreto vira
    lista vazia; `{"version":2,"widgets":"inválido"}` passa como projeto
    válido. Validar presença e tipos dos campos obrigatórios.
  - Usabilidade — `apps/studio/src/gui/MainWindow.cpp:491` (onDemoClicked):
    apaga o projeto e limpa o histórico de desfazer sem verificar alterações
    pendentes. Hoje não existe fluxo de confirmação de documento modificado
    (nem para Open/Exit); a tarefa institui um helper único e o aplica aos
    pontos que descartam o trabalho.
- Escopo:
  1. **Abertura segura** (MainWindow::onOpenClicked): carregar em widget
     temporário/estrutura auxiliar, validar tudo e só então `m_canvas->clear()`
     + inserir nós; em falha, preservar o projeto e histórico atuais e exibir
     os erros.
  2. **Validação recursiva** (ProjectModel::validate): `validateActions` e
     `validateQueryTargets` passam a percorrer `children` em todos os níveis
     (mesmo padrão de `collectNames`/`validateStructure`). Só `validate` é
     chamado nos nós raiz; a recursão fica dentro das funções.
  3. **Salvamento atômico** (ProjectSerializer::save): `QSaveFile` escreve e
     `commit()`; em qualquer erro, o arquivo anterior permanece íntegro.
  4. **Validação estrutural v2** (ProjectModel::fromJson): exigir
     `format == "showbox"` presente e `widgets` presente e do tipo array;
     rejeitar nós não-objeto do array; mantém migração v1 intacta.
  5. **Confirmação de documento modificado**: helper único (ex. no
     MainWindow/StudioController) baseado no undo stack limpo + canvas vazio,
     perguntando antes de descartar; aplicado em Nova demonstração/Open/Exit.
  6. **Testes** (executam sem servidor gráfico): casos para os quatro pontos —
     load válido/inválido não toca o canvas (testável via serializer/modelo),
     ações inválidas em dois ou mais níveis aninhados, save preserva anterior
     se erro/escrita falha, e rejeição de `{"version":2,"widgets":"inválido"}`,
     `format` ausente e `widgets` ausente.
- Fora do escopo: mudanças de protocolo/runtime (motor), migração de dados,
  refactor de arquitetura do Studio (extração de modelo), modificações nos
  repositórios antigos ou qualquer ação remota da SB-006.
- Aceite:
  - `just build` e `just test` verdes (todos os testes novos e 23 existentes);
    `just check` verde (formatação/Bash/ShellCheck).
  - Oráculo legado inalterado e verde (sem mudança de protocolo).
  - Testes novos cobrindo: abertura falha preserva documento/histórico;
    validação recursiva (ações inválidas em ≥2 níveis); save atômico preserva
    arquivo anterior; fromJson v2 rejeita estrutura inválida.
  - Sem mudanças fora dos arquivos de projeto/abertura/salvamento e seus
    testes; sem ações remotas (push/proteção) nesta tarefa.
- Handoff ao final com SHA base/final, arquivos alterados, testes executados e
  limitações (fluxo de confirmação GUI validado por testes de modelo/estado,
  não por automação de UI).

## Handoff

- Base: `a966cb5`. Final da branch: `394522d`.
- Commits:
  - `889ed81` — fix(project): validação recursiva, abertura segura e save
    atômico (SB-006A) [código + testes].
  - `6b9bba0` — docs(tasks): encerra SB-006A com handoff, testes e limitações.
  - `a94c43d` — fix(project): rastreia alterações pelo estado limpo real da
    pilha (SB-006A) [commit corretivo pós-revisão].
  - `5db0d06` — docs(tasks): registra commit corretivo do rastreio de alterações
    (SB-006A).
  - `394522d` — fix(project): demonstração nasce como documento modificado
    (SB-006A) [commit corretivo pós-revisão].
- Arquivos alterados:
  - `libs/project/src/ProjectModel.cpp` — `validateActions`/`validateQueryTargets`
    recursam `children` no início (os `return` internos não pulam a descida);
    `fromJson` v2 exige `format == "showbox"`, `widgets` sendo array de objetos.
  - `libs/project/tests/tst_ProjectModel.cpp` — 3 casos novos
    (`rejectMissingFormatInV2`, `rejectMissingOrInvalidWidgetsInV2`,
    `validateDescendsIntoChildren` — ações inválidas em tabs→page→button).
  - `apps/studio/src/core/ProjectSerializer.cpp` — `QSaveFile` + `commit()`;
    retorna falso em falha de open/escrita sem tocar o arquivo anterior.
  - `apps/studio/tests/tst_ProjectSerializer.cpp` — 2 casos novos
    (`rejectsInvalidV2Structure`, `saveToUnwritablePathFails`).
  - `apps/studio/src/gui/MainWindow.{h,cpp}` — abertura segura (load antes de
    `m_canvas->clear()`; em falha preserva documento, histórico e seleção;
    novo documento zera undo stack e seleção), helper
    `confirmDiscardIfModified()` aplicado em Nova demonstração/Open/`closeEvent`,
    rastreio de documento modificado e reset em open/save/demo.
- Implementação da confirmação: em vez de inferir pelo canvas vazio (o estado
  inicial traz o rótulo de boas-vindas criado sem comandos de undo), usei um
  flag explícito `m_documentModified`, marcado quando `QUndoStack::cleanChanged`
  diz que há comandos e quando `ActionEditor::actionsChanged` dispara (ações não
  geram comandos de undo); reset em save/open/demo. Critério mais preciso que o
  descrito inicialmente e sem modal espúrio na janela recém-aberta.
- Revisão [#comentário P1]: o flag único zerado em save deixava a pilha ainda
  não limpa, então uma nova edição não emitia `cleanChanged(false)` e o descarte
  seguinte perdia a alteração. Corrigido em `a94c43d`:
  - `hasUnsavedChanges()` = `m_actionsModified || !undoStack()->isClean()`.
  - `markDocumentSaved()` = `undoStack()->setClean()` + zera `m_actionsModified`.
  - Confirmação consulta as duas fontes reais; `closeEvent`/Open/Demo usam
    `confirmDiscardIfModified()` (que chama `hasUnsavedChanges`).
  - Teste novo `modifiedTrackingAcrossSaveAndEdit` cobre a sequência
    editar→salvar→editar→fechar/abrir (e Undo de volta ao ponto salvo).
- Revisão [#P1 demonstração]: `onDemoClicked()` deixava pilha limpa e
  `m_actionsModified=false`, então a demo nunca salva era descartável sem
  confirmação ao fechar/abrir. Corrigido em `394522d`: mantém o estado limpo
  durante a construção (sem sinais intermediários de `cleanChanged`) e define
  `m_actionsModified = true` ao final. Teste `demoIsAvailableInStudio` agora
  exige `QVERIFY(window.hasUnsavedChanges())`.
- Testes executados (worktree SB-006A-project-safety):
  - `just build` — verde.
  - `just test` — 23/23 verdes (incluindo 5 casos novos de modelo/serializer, o
    `modifiedTrackingAcrossSaveAndEdit` da sequência editar→salvar→editar e o
    `demoIsAvailableInStudio` que agora exige documento modificado).
  - `just check` — verde (`git diff --check`, `bash -n`, ShellCheck).
  - Oráculo legado: não executado — não houve mudança de protocolo/runtime;
    `fromJson` v2 valida tipos porém preserva a forma do JSON emitido.
- Limitações:
  - A falha de escrita do `QSaveFile` não é simulável de forma confiável em
    ambiente de teste; a integridade vem da garantia do Qt (abrir + commit) e o
    teste cobre open em diretório inexistente + round-trip normal. A atomicidade
    do arquivo anterior em erro é o comportamento documentado do `QSaveFile`.
  - Fluxo de confirmação GUI validado por testes de modelo/estado, não por
    automação de UI (sem servidor gráfico) — teste do demo confirma que a
    janela não trava no caminho limpo.
- Fora do escopo (não tocado): protocolo/runtime do motor, `AppImage`/CI,
  migração de dados, repositórios antigos e qualquer ação remota da SB-006.
- Próximo: integrar esta branch (fast-forward) em `integration/showbox-v1` e
  atualizar o ROADMAP conforme decisão do integrador; depois prosseguir a SB-006.
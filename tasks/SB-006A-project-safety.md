# SB-006A — Correção de segurança de projeto: abertura, salvamento e validação

Estado: em execução.

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
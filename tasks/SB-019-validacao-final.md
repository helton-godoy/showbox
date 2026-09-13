# SB-019 — Plano de testes para promoção a v1.0.0

Estado: validação concluída em 13/09/2026; decisão: recomendar `rc.7`.

- Objetivo: avaliar o status do Showbox (motor) e do Showbox Studio e
  decidir, com evidências, entre publicar `v1.0.0` ou abrir corretivos.
- Candidato: prerelease `v1.0.0-rc.6` (SB-018), `main` em `5bfe080`.
- Branch de trabalho: usar worktree/branch própria ao registrar resultados
  (ex. `chore/SB-019-validacao`); não desenvolver funcionalidades aqui.
- Dependências: SB-010 (fluxo visual), SB-012/SB-018 (roteiros de RC),
  SB-013 (critérios de promoção), SB-017 (automação).

## Escopo

- Executar os blocos A–G abaixo, sempre sobre o RC.6 instalado a partir dos
  assets publicados (não sobre builds de desenvolvimento), exceto onde
  indicado.
- Registrar cada resultado com SHA/versão, comando e evidência (log, saída).
- Classificar todo achado em: bloqueador de `v1.0.0`, corretivo de `rc.7`
  (se necessário) ou backlog pós-`v1.0.0`.
- Não alterar código do produto durante a janela; só registrar.

## Fora do escopo

- Novas funcionalidades, novos widgets ou mudanças de protocolo.
- Publicar em repositórios de terceiros, assinar ou notarizar artefatos.
- Reabrir debate arquitetural das tarefas já integradas.

## Bloco A — Gates automatizados (base)

1. `just doctor`, `just build`, `just test` (28/28), `just check` sem issues.
2. CI em `origin/main`: `build-test`, `sanitizers`, `trunk-check` verdes.
3. SonarCloud sem BUGs novos no diff avaliado.
4. Transporte com socket local executado de verdade (não apenas skips):
   `tst_StudioAutomationTransport` 7/7 fora de sandbox restrito.
5. Registrar SHAs e runs. Falha aqui = bloqueador.

## Bloco B — Pacotes RC.6 instalados (revalidar)

1. Baixar os 7 assets do release e conferir SHA-256 contra o handoff SB-018
   (atenção ao AppImage: valor correto tem 64 hex, ver SB-018).
2. `install_smoke.sh` nos 4 formatos (deb Ubuntu/Debian, rpm Fedora,
   AppImage), todos reportando `1.0.0-rc.6`, incluindo presença e `--help`
   de `showbox-studioctl` e `showbox-studio-mcp`.
3. Falha aqui = bloqueador.

## Bloco C — Fluxo visual completo (marco de produto)

Sobre o Studio empacotado (ou AppImage), interface normal, sem automação:

1. Criar campo de texto, botão e rótulo.
2. Associar ao botão ação Bash que lê o campo e atualiza o rótulo.
3. Executar a aplicação (F5/Executar) e confirmar a atualização do rótulo.
4. Salvar o projeto (`.sbxproj`), fechar, reabrir e confirmar fidelidade.
5. Exportar o Bash e executar o mesmo cenário **sem o Studio**, contra o
   motor real instalado.
6. Referência automatizada: `tst_ShellFlow` 15/15 + `examples/hello-world`.

## Bloco D — Salvar, reabrir, preview e exportação

1. Salvar → indicador limpo; reabrir → sem perdas nem ressalvas.
2. Preview executa e o log reflete stdout/stderr e o código de saída.
3. Exportação gera `.sh` executável com o cenário do bloco C.
4. Projeto com widget não-exportável (SB-016): toolbox o marca e
   preview/export recusam com mensagem clara.

## Bloco E — Automação do Studio (foco da janela)

Sobre o produto empacotado, offscreen onde aplicável:

1. **Sem `--automation`**: inicia normal, nenhum socket criado, cliente
   falha ao conectar.
2. **Socket JSON-RPC**: com `--automation`, `system.describe` (protocolo 1),
   `widget.add`/`setProperty`, `project.snapshot` coerente, `ui.tree`,
   `diagnostics.list`.
3. **`showbox-studioctl`**: describe/snapshot legíveis por máquina; erro de
   transporte e erro RPC com exit não zero e mensagem em stderr.
4. **MCP**: `initialize`/`tools/list`/`tools/call` traduzidos; `tools/list`
   sem `events.subscribe`; `tools/call` para ele retorna erro explícito.
5. **Execução**: sem `--automation-allow-execution`, `preview.start`
   recusado (`-32011`); com a flag, preview inicia/para e `preview.status`
   reflete o estado; `--automation-read-only` recusa mutações (`-32010`).
6. **Eventos**: `events.subscribe` entrega `project.changed`,
   `selection.changed`, `dirty.changed` e `preview.finished` exatamente 1×
   cada, sem `project.changed` inventado para preview/export.
7. **Regressões SB-017 dirigidas**: undo atômico de tabela/combobox; nomes
   inválidos/reservados recusados; ações condicionais validadas; abas
   (mover/desfazer preserva ordem e título); transações new/open com trio
   final coerente após sujeira do editor de ações.

## Bloco F — Uso real exploratório (janela curta)

1. Sessão interativa de pelo menos 30 minutos construindo um diálogo
   diferente do cenário de referência (abas, tabela, combobox, spin).
2. Exercitar undo/redo, agrupar/desagrupar, trocar estilo do toolbox,
   ajuda/sobre, fechar com alterações (diálogo de descarte).
3. Anotar qualquer travamento, perda de dado, texto truncado ou
   comportamento surpreendente, com passos para reproduzir.

## Bloco G — Decisão

Promover `v1.0.0` somente se, cumulativamente:

1. Blocos A–E 100% verdes sobre o RC.6 instalado.
2. Bloco F sem bloqueadores (só cosméticos documentados como backlog ou
   limitações já aceitas: sem assinatura/notarização, GUI inferida por
   cadeia reprodutível).
3. Zero issues/PRs abertos bloqueadores; janela externa sem relato crítico.

Se qualquer item falhar como bloqueador: documentar, abrir tarefa corretiva
e avaliar `rc.7` em vez de `v1.0.0`.

## Handoff

### Execução (13/09/2026)

- Branch: `chore/SB-019-validacao`, baseada em `main`/`5bfe080e7e6765e9af39412fc182d8bec2404bc8`.
- Versões: motor e Studio `1.0.0-rc.6`; Trunk `1.25.0`.
- Bloco A: **verde** — `just doctor`, `just build`, `just test` (28/28),
  `just check`; transporte `tst_StudioAutomationTransport` executado de
  verdade (7/7); CI `build-test`, `sanitizers` e `trunk-check` verdes no run
  [34750650804](https://github.com/helton-godoy/showbox/actions/runs/34750650804);
  SonarCloud sem novos bugs (conclusão informativa `neutral`).
- Bloco B: **verde** — sete assets baixados novamente e SHA-256 conferidos;
  `install_smoke.sh` passou em Ubuntu/Debian, Fedora RPM e AppImage, todos em
  contêineres limpos (versão `1.0.0-rc.6`, `showbox-studioctl` e MCP presentes).
  Os hashes conferidos foram: AppImage
  `9222ad0827c230ad9802dc80a5bb9955df67eb41be55faa1750b968f6c2ebd3a`;
  engine RPM `24b336c35e8fdbc9fd65a809c73819fd876249cd9e411efa811ce0ff2574b672`;
  Studio RPM `3d5dc164ee02697e3f7b7386fc2c3435a668fb54676681abf3e1fcc0d8434da6`;
  Studio Debian `80c7420718b721b7733e490db3da217e5a9b803ed0544df2b2cfde9d4f856b2d`;
  Studio Ubuntu `d0e0e58450d6d910b7e0f9d1f7e5f96fa82a41785f417d173f15940c8a6df99a`;
  engine Debian `aaa8bbf270d60e8da292ec0eadbf3ed8b5195543b521d4b4bacd6c758ab8d9ed`;
  engine Ubuntu `53fbfa2aa859c22037f500aa399554f4bc46df0b1fbd0f4c8462915bba6c9446`.
- Blocos C/D: **verde** — no Studio normal em Xvfb/D-Bus foi criado o trio
  campo/botão/rótulo, a ação Bash foi executada e atualizou o rótulo; projeto
  salvo/reaberto; Bash exportado e executado contra o motor instalado. O
  projeto com tabela não exportável foi recusado pelo preview com mensagem
  explícita. Evidências: `build/sb019/gui/31-reopened.png`,
  `build/sb019/gui/37-preview-action.png` e
  `build/sb019/gui/42-exported-action.png` (artefatos ignorados).
- Bloco E: **verde** — sem automação não houve socket; JSON-RPC, CLI e MCP
  validaram describe/snapshot/tree/diagnostics, erros `-32011`, `-32010`,
  `-32020` e `-32602`, permissões, undo atômico, abas e eventos exatamente
  uma vez (`project.changed`, `selection.changed`, `dirty.changed`,
  `preview.finished`).
- Bloco F: **bloqueado** — a sessão gráfica carregou o segundo diálogo com
  abas, tabela, combobox e spinbox; exercitou edição, undo/redo, agrupamento
  em Frame, estilo Tree e About (`build/sb019/gui/50-f-undo-redo.png`,
  `58-f-grouped.png`, `59-f-ungroup-redo.png`, `60-f-about.png`). Ao fechar
  com alterações, porém, o Studio encerrou sem apresentar o diálogo de
  descarte esperado; a rotina subsequente perdeu a janela e registrou
  `BadWindow` em `f-session-run.log`. Não houve alteração no repositório, mas
  a proteção contra perda de alterações não pôde ser comprovada.
- Consulta imediatamente antes da decisão: nenhuma issue ou PR aberto no
  repositório.

### Julgamento

`v1.0.0` **não deve ser promovida**. O encerramento sem confirmação no fluxo
de alterações é uma falha reproduzível/contratual potencialmente destrutiva,
portanto bloqueia a promoção e requer `rc.7` com correção e nova execução de
SB-019. Nenhuma tag ou release estável foi criada nesta validação.

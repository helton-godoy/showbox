# SB-017 — Showbox Studio Automation Interface

Estado: em execução (2026-09-12).

- Objetivo: oferecer uma interface local, versionada e observável para
  automação, testes funcionais, CI e diagnóstico do Showbox Studio.
- Base: `b5779e3` (main, RC.5 integrado).
- Branch: `feat/SB-017-studio-automation`.
- Dependências: SB-003/SB-004 (modelo e catálogo), SB-010 (fluxo visual),
  SB-015/SB-016 (estabilidade e exportação honesta).
- Escopo desta tarefa: protocolo JSON-RPC 2.0 delimitado por linhas, servidor
  QLocalServer/QLocalSocket desativado por padrão, cliente `showbox-studioctl`,
  diagnósticos estruturados e adaptador `showbox-studio-mcp` consumidor do CLI
  público; depois, mutações controladas reutilizando os serviços do Studio.
- Fora do escopo: servidor TCP, execução implícita de scripts ao abrir ou
  inspecionar projetos, acesso do MCP a widgets Qt, e alteração do roadmap
  global sem integração.

## Arquitetura e contrato público

- Protocolo versão 1, JSON-RPC 2.0, um objeto JSON por linha.
- Endpoint local com `QLocalServer::UserAccessOption`; o caminho pode ser
  informado por `--automation-socket`.
- A interface só é criada com `--automation`; `--automation-read-only`
  restringe mutações; `--automation-allow-execution` é exigido para iniciar
  preview/execução Bash pela automação.
- Métodos iniciais: `system.describe`, `system.capabilities`,
  `project.snapshot`, `ui.tree`, `diagnostics.list`, `export.validate`,
  `preview.status`, `preview.logs` e `events.subscribe`.
- Erros possuem forma estável com `code`, `severity`, `component`, `message`,
  `context`, `location` e `suggestion`, quando aplicável.
- O MCP é um adaptador separado e conversa somente com a interface local.

## Critérios de aceite

- Studio inicia normalmente sem socket de automação.
- Studio com `--automation` responde aos métodos públicos por socket local,
  sem expor ponteiros, classes Qt ou nomes acidentais da árvore.
- Cliente `showbox-studioctl` realiza descoberta e consultas JSON legíveis por
  máquinas, com falhas de transporte em stderr e código não zero.
- Base somente leitura é testada sem servidor gráfico e sem executar Bash.
- `--automation-read-only` recusa mutações com erro estruturado; execução só
  ocorre após `--automation-allow-execution`.
- `showbox-studio-mcp` traduz `initialize`, `tools/list` e `tools/call` para
  o protocolo público, sem acessar o Studio internamente.
- `just build` e `just test` passam; os comandos e limitações reais ficam no
  checkpoint e no handoff.

## Riscos

- Qt local socket varia por plataforma; o primeiro incremento cobre Unix/Qt6
  e mantém o nome do endpoint configurável.
- A árvore QWidget contém objetos internos do Qt; a resposta deve ser gerada
  pelo modelo sanitizado, nunca por `QObject::children()` cru.
- Preview é assíncrono; o contrato expõe estado e logs acumulados, não promete
  sincronismo da execução.


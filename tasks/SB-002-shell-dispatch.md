# SB-002 — Interface → Bash → interface

Estado: concluída (commit d3291dd, branch feat/SB-002-shell-dispatch).

- Objetivo: criar campo, botão e rótulo no Studio; associar uma ação Bash;
  executar e atualizar o rótulo na mesma janela; exportar para execução sem IDE.
- Base: ec1c7afc4929d6639d649d1df0f7f6245eca6c42.
- Branch prevista: feat/SB-002-shell-dispatch.
- Responsável: integrador desta sessão.
- Dependência: SB-001 aprovada.
- Escopo: apps/studio/src/core, integração na MainWindow, shell/lib,
  examples/hello-world e testes de fluxo completo.
- Fora do escopo: reescrever widgets, protocolo v1, modelo completo ou pacotes.

## Contratos a definir antes de paralelizar

Despacho seguro de eventos, transporte de comandos à instância ativa, isolamento
stdout/stderr, ciclo de processos e cancelamento, caminhos e quoting dos scripts.
O valor de widgets não pode ser avaliado como código. Abrir projeto não executa ação.

## Aceite

Teste contra showbox real, com clique por harness Qt: valores com espaços e aspas,
ação executada e rótulo atualizado, erro de script visível e processos encerrados.
O script exportado deve executar fora da pasta do repositório. Prévia visual sem
shell deve permanecer distinta de executar aplicação. Caracterizar falhas atuais
antes de alterar o gerador e PreviewManager.

Persistência completa do modelo e catálogo ficam em SB-003/SB-004; documentar
explicitamente os tipos e propriedades suportados nesta primeira demonstração.

## Handoff

Commit final d3291dd sobre a base ec1c7af, branch feat/SB-002-shell-dispatch.
Fluxo campo → Bash → rótulo fechado na mesma janela via shell/lib/runtime.sh,
prévia visual sem ações, execução com logs e Parar, exportação independente do
repositório e salvar/reabrir preservando o JSON das ações.
Validação local: ctest 21/21 e ASan 2/2 aprovados (incluindo demo com clique real,
aspas, espaços e `$(...)` neutro; erro de script visível; subprocessos encerrados);
oráculo legado compilado. Limites: exportação restrita ao conjunto linear v1
(sem grid/form), modelo completo fica na SB-003, catálogo na SB-004. Sem push.

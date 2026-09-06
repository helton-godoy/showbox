# SB-002 — Interface → Bash → interface

Estado: planejada, ainda não iniciada.

- Objetivo: criar campo, botão e rótulo no Studio; associar uma ação Bash;
  executar e atualizar o rótulo na mesma janela; exportar para execução sem IDE.
- Base: fixar SHA após revisão de SB-001, antes de criar a worktree.
- Branch prevista: feat/SB-002-shell-dispatch.
- Responsável: a designar pelo integrador.
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

# SB-010 — Fluxo visual executável de ponta a ponta

Estado: concluída (2026-09-10).

- Objetivo: comprovar e tornar confiável o principal fluxo de produto do
  Showbox: criar uma interface no Studio, programar uma ação Bash, executá-la
  na mesma janela, persistir o projeto e exportar uma aplicação executável sem
  depender do Studio.
- Base: `0ed5f48` (main com o contrato integrado pelo PR #23).
- Branch: `feat/SB-010-vertical-slice`.
- Responsável: integrador desta sessão.
- Dependências: SB-002 (ações e exportação), SB-003 (modelo persistente),
  SB-004 (catálogo compartilhado) e SB-006A (abertura/salvamento seguros).

## Cenário de referência

1. Criar um projeto com campo de texto, botão e rótulo.
2. Associar ao clique do botão uma ação Bash que leia o campo e atualize o
   rótulo.
3. Executar a prévia pelo Studio e observar a atualização na mesma janela.
4. Salvar, fechar e reabrir o projeto sem perder widgets, propriedades ou ação.
5. Exportar uma aplicação Bash e executar o mesmo cenário usando o motor real,
   sem o Studio.

## Escopo

- Auditar o caminho já implementado entre `ActionEditor`, `ScriptGenerator`,
  `PreviewManager`, serialização do projeto e runtime; corrigir as rupturas que
  impedirem o cenário de referência.
- Tornar o cenário acessível pela interface normal do Studio, sem depender de
  edição manual do arquivo `.sbxproj`.
- Transportar valores do widget como dados, preservando espaços, aspas e
  conteúdo que se pareça com código; não usar `eval` para interpretar saídas.
- Garantir que abrir ou selecionar um projeto nunca execute scripts. A execução
  ocorre somente por comando explícito de prévia ou na aplicação exportada.
- Criar um teste integrado determinístico que cubra persistência, geração e
  execução contra o binário real do motor.
- Manter uma demonstração revisável em `examples/` e atualizar o guia do usuário
  quando o comportamento final divergir da documentação atual.

## Fora do escopo

- Novos tipos de widget, layouts grid/form, sistema de plugins ou mudança no
  protocolo estável do motor.
- Polimento visual amplo do Studio, suporte a outros back-ends ou plataformas
  além de Linux/Bash.
- Nova tag ou promoção do draft `v1.0.0-rc.3`; uma nova release será decidida
  após a entrega funcional.

## Aceite

- O cenário de referência pode ser construído pela interface normal do Studio.
- Salvar e reabrir conserva a hierarquia, os nomes, os textos e a ação Bash.
- A prévia atualiza o rótulo na instância em execução e encerra sem processos
  ou arquivos temporários abandonados.
- A aplicação exportada funciona fora do diretório do projeto com Bash,
  util-linux e o motor Showbox instalado ou indicado por `SHOWBOX_BIN`.
- Entradas com espaços, aspas e sintaxe shell são tratadas como dados, com
  cobertura automatizada.
- `just build`, `just test` e `just check` verdes. Se o protocolo precisar ser
  alterado, a mudança será retirada desta tarefa ou formalizada em contrato
  próprio com execução do oráculo legado.
- Handoff registra SHA base/final, roteiro manual executado, testes, limitações
  e evidência da execução contra o motor real.

## Handoff

- Implementação: `855fb49`.
- O Studio ganhou **File → New Project** (`Ctrl+N`) e ação equivalente na
  toolbar. O comando respeita a confirmação de descarte, encerra uma prévia em
  execução e reinicia canvas, seleção, editores, diretório e estado de
  modificação.
- O teste `studioAuthoredProjectSurvivesAndRuns` reproduz o fluxo normal:
  cria TextBox, Button e Label por drag-and-drop; altera `objectName` pelo
  PropertyEditor; adiciona e edita Bash pelo ActionEditor; salva e reabre o
  modelo; gera a aplicação; e a executa contra o motor real.
- O valor de teste contém espaços, aspas, barra invertida, substituição de
  comando e crases. O rótulo recebe o texto literal e os arquivos-sentinela que
  denunciariam execução indevida não são criados.
- Guia `docs/user/shell-actions.md` atualizado com o caminho de autoria manual e
  com o estado atual da persistência versionada.
- Validações: `just build`; `just test` 24/24; `just check`; teste focal
  `tst_ShellFlow` contra `showbox_test_driver` que recompila a entrada real do
  motor. Nenhuma mudança de protocolo; oráculo legado não necessário.
- Limitação: a automação cobre o fluxo funcional em plataforma Qt offscreen;
  avaliação visual e ergonomia ampla continuam fora do aceite desta tarefa.

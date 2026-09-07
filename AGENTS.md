# Showbox — instruções para contribuição

## Missão e fontes oficiais

Showbox é o nome canônico: motor Qt6 e Studio para ferramentas visuais com Bash.
Interaja em português brasileiro. Leia `docs/project/ROADMAP.md`,
`docs/architecture/adr/0001-monorepo.md` e o contrato em `tasks/` antes de editar.
`docs/project/v1/` registra o ciclo histórico do motor, não o roadmap da suíte.

## Trabalho isolado

- Uma tarefa, branch e worktree por responsável. Não trocar a branch de outro agente.
- Definir objetivo, SHA base, escopo, dependências e aceite antes da implementação.
- Não alterar contratos compartilhados sem coordenar seus consumidores.
- Somente o integrador atualiza roadmap e status global.
- Não compartilhar diretórios de build, sockets, portas ou arquivos temporários.
- Não usar stash compartilhado, force-push em branches compartilhadas ou limpeza global.
- Não publicar ou configurar serviços externos sem autorização do mantenedor.

## Código e verificação

C++17, Qt6, CMake e quatro espaços para código novo. Preserve formatação de código
importado em mudanças mecânicas. O motor não depende do Studio. Widgets compartilhados
ficam em `libs/ui`; não copiar suas implementações para o editor.
Preserve o protocolo e a matriz em `docs/project/v1/COMPATIBILITY_MATRIX.md`.
Nunca executar scripts ao abrir projetos ou ao selecionar widgets.
Não interpretar saída do motor com eval. Nenhum alvo estável compila `legacy/`.

Use `just doctor`, `just build` e `just test`. Para alterações no protocolo,
execute também o oráculo legado conforme `docs/development/README.md`.
Registre testes realmente executados e falhas conhecidas. Não confunda compilação
com funcionamento de geração, preview ou ações.

## Entrega

Conventional Commits com escopo; PRs pequenos. Separar movimentações e mudanças de
comportamento. Informar tarefa, SHA base/final, testes e limitações no handoff.
Não incluir binários, caches, segredos ou documentação gerada no Git.

# Trabalho paralelo e Git

Uma tarefa, branch, worktree e responsável. Worktrees compartilham objetos e refs
Git; não são sandbox de segurança. O integrador mantém a main.

1. Criar contrato em tasks com objetivo, base SHA, escopo, dependências e aceite.
2. Integrar contratos compartilhados antes de iniciar consumidores em paralelo.
3. Executar `just worktree SB-014 shell-dispatch` em uma árvore limpa e atualizada.
4. Trabalhar apenas na worktree indicada. Usar build e temporários exclusivos.
5. Entregar commit final, testes e lacunas; integrador incorpora sequencialmente.
6. Remover com git worktree remove somente após verificar árvore limpa e entrega.

Branches feat/SB-NNN-assunto, fix/SB-NNN-assunto ou chore/SB-NNN-assunto.
Não usar stash compartilhado, force-push na main ou checkout de branch alheia.
Rebase só em branch exclusiva. PRs focados; movimentação separada de refatoração.
Roadmap global é atualizado pelo integrador; agentes atualizam seu handoff.

## Configuração remota pendente
Exigir PR e checks build-test/sanitizers, bloquear exclusão e force-push na main,
usar revisão CODEOWNERS e fila de merge se disponível. Sem fila, integrar uma
mudança por vez. Estas regras documentadas não equivalem à proteção no GitHub.

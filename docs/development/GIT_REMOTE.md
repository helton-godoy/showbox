# Git remoto — governança do repositório canônico

Estas regras valem para o repositório canônico `helton-godoy/showbox`. Elas são
operacionalizadas por proteções configuradas no GitHub (ver ADR 0006 e
`tools/setup/doctor.sh`), não apenas por convenção. O repositório é de conta
única; as proteções não exigem aprovação humana, mas exigem PR e checks verdes.

## Função das branches

- `main`: linha de entrega. Recebe somente merges de `integration/showbox-v1`
  (via PR, preservando a cadeia de commits). É a branch padrão e origem das
  tags de release `v*`.
- `integration/showbox-v1`: linha de integração contínua. Nela as tarefas
  (`feat/SB-*`) são integradas de forma sequencial pelo responsável da
  integração, via PR ou pela política de `just worktree`. Não há push direto de
  agentes. Quando a linha está estável, é consolidada em `main` por PR.
- `feat/SB-*`, `fix/SB-*`, `chore/SB-*`: branches de tarefa, uma por worktree,
  curtas e descartáveis após a integração. Não são protegidas, mas nunca devem
  receber push direto de `main`/`integration` por terceiros.

## Pull Requests obrigatórios

- Toda mudança que altera `main` e `integration/showbox-v1` entra por PR.
- O PR deve estar atualizado com a base (`Update branch`/up-to-date) antes do
  merge.
- Checks obrigatórios (configurados como required status checks de PR):
  - `build-test` (app_id 15368 / GitHub Actions) — build, testes (23 suítes),
    ShellCheck, validação da árvore de instalação e smoke de CLI contratual.
  - `sanitizers` (app_id 15368 / GitHub Actions) — build e testes com
    ASan/UBSan.
  Os nomes acima correspondem aos nomes efetivos dos check-runs produzidos pelo
  workflow `Showbox CI`; a proteção vincula cada contexto ao app_id do GitHub
  Actions para evitar colisões (`tools/setup/doctor.sh` valida).
- Exigência de aprovação humana: **zero**. A garantia é técnica (PR + checks),
  conforme decisão para repositório de conta única.

## Tags `v*`

- Tags de release são criadas somente pelo job de release do `release.yml`
  (fluxo autorizado), nunca manualmente por push direto.
- Regra de proteção de tags com padrão `v*`: proíbe sobrescrita
  (`non_fast_forward`) e exclusão (`deletion`), aplicada por ruleset
  `tags-v-protection` (validação em `just doctor`).
- O primeiro release é um **draft** (nada é publicado publicamente) até a SB-007
  decidir a primeira versão pública com tag real.

## Proibição de force-push

- Force-push é proibido em `main`, `integration/showbox-v1` e em tags `v*`
  (bloqueio configurado na proteção).
- Rebase somente em branch exclusiva da tarefa, e ainda assim sem sobrescrever
  refs remotas de outras pessoas. Divergência com o remoto resolve por PR,
  nunca por force-push.

## Procedimento de emergência

1. Parar a operação imediatamente (pipeline de tarefa ou CI).
2. Avaliar o impacto e não alterar mais referências remotas até o diagnóstico.
3. Restaurar a partir das refs locais de auditoria (`refs/audit/` ou branches
   `audit/remote-*` criadas na SB-006), nunca corrigindo por force-push.
4. Se o remoto tiver commits inesperados, abrir análise com o mantenedor antes
   de qualquer push.
5. Registrar o incidente no contrato/handoff da tarefa em andamento.

## Política para agentes LLM e worktrees

- Trabalho isolado conforme `docs/development/worktrees.md`: uma tarefa, branch,
  worktree e responsável; integrador atualiza o ROADMAP e mantém a integração.
- Agentes não fazem push direto para `main`/`integration/showbox-v1`: a entrega
  é a branch da tarefa + handoff. A integração é feita pelo responsável da
  integração via `just worktree` + PR (ou merge seguindo a política).
- Fora de tarefa autorizada, nenhuma alteração em proteções, tags, arquivamento
  de repositórios ou configuração externa.
- Sempre confirmar com `git status`, `git diff --check`, `just check` e validar
  as refs remotas com `git ls-remote` antes de operações externas.

## Verificação

`just doctor` valida a presença/autenticação do `gh`, o acesso ao repositório,
a existência das branches remotas, as proteções aplicadas e os nomes reais dos
checks obrigatórios (passo a passo em `tools/setup/doctor.sh`).

## Integrações de GitHub Apps

- Apenas o GitHub Actions e o SonarCloud estão instalados. Apps de terceiros
  que foram instalados anteriormente sem worker ativo (`codegen-sh`,
  `prefect-horizon`, `cursor`, `continue`, `kilo-code-bot`, `trunk-io`) foram
  desinstalados por deixarem check-suites `queued` para sempre e bloquearem
  merges.
- Não reinstalar apps que possam registrar check-suites sem um worker real
  ativo; qualquer avaliação passa antes por um contrato (ex.: SB-007 para o
  Trunk).
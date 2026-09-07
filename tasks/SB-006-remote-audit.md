# SB-006 — Auditoria do estado remoto inicial (2026-09-07)

Captura de pré-condições antes do primeiro push. Coletado via `gh api` e
`git ls-remote`. Nenhuma referência remota foi alterada nesta etapa; backup
local das referências em `refs/heads/audit/remote-showbox/2026-09-07/*`.

## Repositório canônico `helton-godoy/showbox`

- URL: `https://github.com/helton-godoy/showbox.git` (origin).
- Proprietário e conta usada: `helton-godoy` (única; `gh auth status` OK,
  escopos: gist, read:org, repo, workflow; protocolo https).
- Visibilidade: public. `archived`: false. `pushed_at`: 2026-08-29T21:51:32Z.
- Branch padrão: `main` (estrutura pré-monorepo ainda presente).
- Permissões da conta: admin (read/push/admin todos true).

### Branches remotas (SHA no GitHub)

| branch | SHA remoto | protegida |
|---|---|---|
| `main` | `eab3300348671b1d2c2784595bb3b4294a18e78f` | não |
| `develop` | `2848cb37d7137ba49bd6b608171937f8c2c60b02` | não |
| `bugfix/quick-fixes` | `2848cb37d7137ba49bd6b608171937f8c2c60b02` | não |
| `integration/showbox-v1` | `a5dd820e55c0e690f471451b95ddd6dade7de0c6` | não |

`origin/HEAD` aponta para `origin/main`.

### PRs, workflows e proteções

- PRs: nenhum aberto ou fechado em qualquer estado.
- Workflows: histórico de runs de um workflow chamado **ShowBox CI** (todas com
  falha, pushes antigos de 2026-01 e 2026-08-29). O `ci.yml` do monorepo define
  o workflow **Showbox CI**; o próximo push cria runs deste nome.
- Proteção de branches: **nenhuma** (`GET /branches/main/protection` retornou
  404). Nenhuma regra de branch ruleset nem tag protection.

### Repositórios antigos (em `helton-godoy/*`, antes da transição)

| repositório | default branch | SHA | archived |
|---|---|---|---|
| `dialogbox` | `master` | `6989740746f376becc989ab2698e77d14186a0f9` | false |
| `SHantilly` | `main` | `33c4b0644cc695f2bbf3d64d8e2c6c7b318aa85d` | false |
| `SHantilly-Studio` | `main` | `eda620c13b3b91238ba5b538b390c0420e108dec` | false |
| `showbox-studio` | `main` | `84786852552e163fff10ffb00660dbf8b628cae1` | false |

Pushed_at: dialogbox 2020-11-16, SHantilly 2026-08-31, SHantilly-Studio
2026-07-20, showbox-studio 2026-01-19.

## Backup local das referências

Branches locais de auditoria (apenas leitura, não publicadas):

- `audit/remote-showbox/2026-09-07/main` → eab3300
- `audit/remote-showbox/2026-09-07/develop` → 2848cb3
- `audit/remote-showbox/2026-09-07/bugfix-quick-fixes` → 2848cb3
- `audit/remote-showbox/2026-09-07/integration-showbox-v1` → a5dd820

Estas refs ficam disponíveis durante o período de transição e podem ser
removidas após a SB-006 ser encerrada (decisão do integrador).
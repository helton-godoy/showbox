# SB-013 — Validação externa do RC.4 e decisão v1.0.0

Estado: concluída (2026-09-12, decidido: produzir e publicar `v1.0.0-rc.5`).

- Objetivo: validar o prerelease `v1.0.0-rc.4` em uso externo e decidir, com evidências, entre promover `v1.0.0` ou produzir `v1.0.0-rc.5`.
- Base: `8bc0757` (`main` com o contrato SB-013 integrado via PR #30).
- Branch: `chore/SB-013-validacao`.
- Responsável: integrador desta sessão.
- Dependências: SB-010 (fluxo visual), SB-011 (fluxo trunk-based), SB-012 (RC.4 publicado como prerelease).

## Escopo

- Definir janela de validação externa do RC.4 e o roteiro mínimo (instalação DEB Ubuntu/Debian, RPM e AppImage + cenário de referência da SB-010).
- Coletar feedback externo e classificar achados em bloqueadores de `v1.0.0`, corretivos de `rc.5` ou backlog pós-`v1.0.0`.
- Verificar pendências herdadas da SB-012: aviso de depreciação Node.js 20 no Actions, ausência de assinatura/notarização, validação GUI empacotada inferida por cadeia reprodutível.
- Atualizar `docs/project/ROADMAP.md` com a nova linha SB-013 somente após aprovação do contrato por PR (somente o integrador altera o roadmap global).
- Registrar a decisão final (`v1.0.0` ou `rc.5`) com SHAs, testes e limitações no handoff.

## Fora do escopo

- Novas features, novos widgets, paridade completa de propriedades do Studio ou mudança de protocolo (exigem contrato próprio).
- Promover `v1.0.0` diretamente sem validação ou publicar em repositórios de terceiros.
- Alterar os assets ou a tag `v1.0.0-rc.4` já publicados.

## Aceite

- Contrato revisado e integrado por PR com base, escopo e aceite explícitos.
- Critérios de promoção para `v1.0.0` documentados (smokes, fluxo visual, janela externa sem bloqueadores).
- Decisão `v1.0.0` vs `rc.5` registrada no handoff com evidências, ou bloqueio documentado.
- `just doctor` e `just check` verdes na branch do contrato.

## Handoff parcial — validação local (2026-09-12)

- Revalidação local sobre `8bc0757` na worktree `chore/SB-013-validacao`:
  `just doctor` aprovado (proteção `main`, tags `v*`, Trunk 1.25.0);
  `just build` completo (137/137 alvos); `just test` **24/24 aprovados**,
  incluindo `tst_ShellFlow` (2.78s); `just check` sem achados.
- Sinal externo: zero issues abertas; cada um dos sete assets do RC.4 registra
  2 downloads, correspondentes aos downloads próprios da validação SB-012.
  O RC.4 foi publicado há menos de 24h — ainda **sem feedback externo**.
- Aviso Node.js 20: confirmado no run `34665687619` (merge #30). Origem nos
  pins `actions/checkout@11d5960a` (v4.2.2) e
  `actions/upload-artifact@ea165f8d` (v4.6.2), executados à força em Node 24.
  Cosmético — todos os jobs passam. Classificação: **não-bloqueador**;
  correção proposta como tarefa própria (SB-014: migrar pins para versões
  baseadas em Node 24 — checkout v5, upload/download-artifact v5, revisar
  trunk-action e softprops — com validação pelos gates do PR).
- Assinatura/notarização ausente e validação GUI empacotada inferida por cadeia
  reprodutível: limitações herdadas e aceitas da SB-012, **não-bloqueadoras**
  para `v1.0.0`, a documentar nas notas do release final.
- Critérios de promoção para `v1.0.0`: (1) janela externa sem bloqueadores;
  (2) smokes DEB/RPM/AppImage + fluxo visual SB-010 revalidados no candidato;
  (3) CI (`build-test`, `sanitizers`, `trunk-check`) e SonarCloud verdes.
- Decisão: **não promover `v1.0.0` ainda**. A janela externa segue aberta;
  falta definir prazo e testadores com o mantenedor. Estado permanece
  `em andamento` até o registro de feedback externo ou o fim da janela.

## Decisão — produzir `v1.0.0-rc.5` (2026-09-12)

- O mantenedor solicitou testar as correções do Studio (SB-015: crashes e
  menus; SB-016: toolbox honesto) via release publicada. Essas mudanças de
  produto justificam novo candidato nos termos da SB-012.
- A `main` em `87b9929` (SB-014/015/016 integradas) está 6 merges à frente
  da tag `v1.0.0-rc.4` (`30e72c1`); nenhum asset publicado contém as
  correções. `VERSION` sobe para `1.0.0-rc.5`; tag `v1.0.0-rc.5` sobre o
  merge de preparação; validação (checksums + 4 smokes + fluxo visual) e
  publicação como prerelease seguem o mesmo roteiro da SB-012.
- A janela de validação externa recomeça no RC.5; o RC.4 permanece publicado
  como histórico.

## Handoff — RC.5 publicado (2026-09-12)

- Preparação: PR #38 (`chore(release): prepara v1.0.0-rc.5`) integrado em
  `e45d61f`; gates `build-test`, `sanitizers`, `trunk-check` e SonarCloud
  verdes. Conversões validadas: app `1.0.0-rc.5`, deb `1.0.0~rc5-1`,
  rpm `1.0.0`/`0.3.rc5`, prerelease `true`.
- Tag anotada `v1.0.0-rc.5` sobre `e45d61f`. Workflow run `34708066203`:
  `deb-ubuntu`, `deb-debian`, `rpm`, `appimage`, `install-smoke` e `release`
  com sucesso; draft com os sete assets criado.
- Assets baixados e conferidos (nomes/versões rc.5). Checksums SHA-256:
  - `f7ed29e089e96419b85def4eca70a722e438e3614d795e420cdb1ee3e06f1d97`
    `showbox_1.0.0.rc5-1_ubuntu24.04_amd64.deb`;
  - `dac2faecf2f48695f7f594f2187586b1fdbaadd4de337203c6801df736caa76f`
    `showbox_1.0.0.rc5-1_debian13_amd64.deb`;
  - `2a9d0a7c8d82f7eb94a8be6db380b2a412d70863d7d5dfd707b0e41ae86270fd`
    `showbox-1.0.0-0.3.rc5.fc46.x86_64.rpm`;
  - `c704e1b983ef63917f6e3cb2dd6fb926b9dca51efc9eac52a2b6c693242fcbf9`
    `showbox-studio_1.0.0.rc5-1_ubuntu24.04_amd64.deb`;
  - `15694fb18cc9d94278992bca38b4e10a6a17782b2304deab6841f18ca45282e2`
    `showbox-studio_1.0.0.rc5-1_debian13_amd64.deb`;
  - `4c13027a030520f108527d3bc14bf3e6ee12e345511f8713b45cde4fffe4dfaf`
    `showbox-studio-1.0.0-0.3.rc5.fc46.x86_64.rpm`;
  - `d25240a4f80894f8c81fe5383e3f31e5b5ee2d1926261fbc0783d38c511d51ae`
    `ShowBox-Studio-1.0.0-rc.5-x86_64.AppImage`.
- Smokes locais (podman) aprovados, todos reportando `1.0.0-rc.5`: deb
  Ubuntu, deb Debian, RPM Fedora e AppImage.
- Fluxo visual: `tst_ShellFlow` 15/15 sobre a `main` == SHA da tag
  (`e45d61f`), cobrindo prévia, exportação, salvar/reabrir e ações Bash.
- Release publicado como prerelease em 2026-09-12:
  `https://github.com/helton-godoy/showbox/releases/tag/v1.0.0-rc.5`,
  com `draft: false` e `prerelease: true`.
- Limitações: sem assinatura/notarização; GUI empacotada inferida por cadeia
  reprodutível; validação externa do RC.5 ainda pendente (janela reaberta).

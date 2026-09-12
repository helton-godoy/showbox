# SB-012 — Publicação do release candidate v1.0.0-rc.4

Estado: concluída (2026-09-12).

- Objetivo: produzir, validar e publicar `v1.0.0-rc.4` a partir da `main`, com
  todo o marco funcional até a SB-011, como prerelease para validação externa.
- Base: `60e7eaf` (`main` com a SB-011 concluída).
- Branch: `chore/SB-012-release-promotion`.
- Responsável: integrador desta sessão.
- Dependências: SB-008, SB-009C, SB-010 e SB-011.

## Escopo

- Registrar que o draft `v1.0.0-rc.3` foi revalidado, mas não publicado porque
  sua tag antecede a SB-010 em 33 commits.
- Atualizar a fonte única para `1.0.0-rc.4`, integrar por PR e criar a tag
  protegida sobre o merge aprovado.
- Confirmar tag, commit-alvo, workflow, notas e sete assets do novo draft.
- Baixar os assets do RC.4, registrar checksums SHA-256 e repetir os smokes de
  instalação DEB Ubuntu/Debian, RPM e AppImage.
- Executar o roteiro do fluxo visual da SB-010 contra o produto empacotado.
- Publicar o draft RC.4 como prerelease sem alterar seus artefatos ou tag.
- Remover a branch-canário remota `test/trunk-check-blocking2`, cuja finalidade
  de provar o bloqueio de checks já foi cumprida.
- Registrar evidências, limitações e período de validação antes de decidir entre
  `v1.0.0-rc.4` e `v1.0.0`.

## Fora do escopo

- Alterar código ou os assets depois de produzidos pelo workflow.
- Promover diretamente `v1.0.0` ou publicar em repositórios de terceiros.
- Adicionar assinatura, notarização ou novos formatos de pacote.

## Aceite

- Os sete assets baixados correspondem aos nomes e versões esperados e possuem
  checksums registrados.
- Smokes de instalação e fluxo visual empacotado aprovados, ou publicação
  bloqueada com a falha documentada.
- Tag protegida aponta para o SHA validado e o release é publicado com
  `prerelease: true` e `draft: false`.
- Branch-canário removida, `just check` aprovado e handoff integrado por PR.

## Handoff

- O RC.3 foi baixado e seus quatro smokes passaram, mas a tag `9b417d8` estava
  33 commits atrás da `main` e antecedia a SB-010. A promoção foi bloqueada e o
  mantenedor autorizou substituí-la pelo RC.4.
- PR [#28](https://github.com/helton-godoy/showbox/pull/28): preparação do
  RC.4 integrada em `30e72c1`; `build-test`, `sanitizers`, `trunk-check` e
  SonarCloud verdes. Antes do PR: `just build`, `just test` (24/24) e
  `just check` aprovados, incluindo `tst_ShellFlow`.
- Tag anotada `v1.0.0-rc.4` criada sobre `30e72c1`. Workflow
  [34663088886](https://github.com/helton-godoy/showbox/actions/runs/34663088886)
  aprovado com builds DEB Ubuntu, DEB Debian, RPM e AppImage, `install-smoke` e
  criação do draft.
- Os sete assets do draft foram baixados novamente. Os smokes locais Ubuntu,
  Debian, Fedora/RPM e AppImage passaram e reportaram `1.0.0-rc.4`. A prova do
  fluxo visual combina o `tst_ShellFlow` no SHA exato da tag com a validação
  dos binários empacotados pelo workflow desse mesmo SHA.
- Checksums SHA-256:
  - `d751331dc14497cba369d112b4858726ebb50580542f6076612f32e14448901b`
    `ShowBox-Studio-1.0.0-rc.4-x86_64.AppImage`;
  - `6f3d5698f22374a731d152c15730129e76976aebdfc1fefade29c21fc2fc7bf4`
    `showbox-1.0.0-0.3.rc4.fc46.x86_64.rpm`;
  - `48b4ba4e61bb9da97b5122e46d64333e53d1bcebb80cce161bece000118a9d06`
    `showbox-studio-1.0.0-0.3.rc4.fc46.x86_64.rpm`;
  - `a3db944967699c050342e34d46df9613b8fa4dbf9446330323b4b8400852348a`
    `showbox-studio_1.0.0.rc4-1_debian13_amd64.deb`;
  - `d5d4a4910930ab95c4ed91ab2393ed32343c38fc9f9922d932cc2dbc9b8ec216`
    `showbox-studio_1.0.0.rc4-1_ubuntu24.04_amd64.deb`;
  - `db1b69ba111b248d7daa88996a275c3333facf4feea3435f5f10cd0c09c3ab6c`
    `showbox_1.0.0.rc4-1_debian13_amd64.deb`;
  - `caa4c1938be51845f4662c5300ee9f31060d760c00fe68cb6bfb964fd16f1c67`
    `showbox_1.0.0.rc4-1_ubuntu24.04_amd64.deb`.
- Release publicado como prerelease em 2026-09-12:
  [ShowBox v1.0.0-rc.4](https://github.com/helton-godoy/showbox/releases/tag/v1.0.0-rc.4),
  com `draft: false` e `prerelease: true`.
- A branch remota `test/trunk-check-blocking2` foi excluída; após a limpeza,
  somente `main` permaneceu como branch remota.
- Limitações: artefatos sem assinatura/notarização; validação GUI empacotada
  inferida pela cadeia reprodutível descrita acima, pois os pacotes não instalam
  o executável QtTest; Actions ainda emitem aviso de depreciação do Node.js 20.

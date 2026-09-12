# SB-012 — Publicação do release candidate v1.0.0-rc.4

Estado: em andamento (2026-09-12).

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

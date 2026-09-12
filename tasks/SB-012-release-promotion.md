# SB-012 — Publicação do release candidate v1.0.0-rc.3

Estado: em andamento (2026-09-12).

- Objetivo: revalidar os sete artefatos do draft `v1.0.0-rc.3` e publicar o
  candidato como prerelease para validação externa.
- Base: `60e7eaf` (`main` com a SB-011 concluída).
- Branch: `chore/SB-012-release-promotion`.
- Responsável: integrador desta sessão.
- Dependências: SB-008, SB-009C, SB-010 e SB-011.

## Escopo

- Confirmar tag, commit-alvo, workflow de release, notas e sete assets do draft.
- Baixar os assets, registrar checksums SHA-256 e repetir os smokes de instalação
  DEB Ubuntu/Debian, RPM e AppImage.
- Executar o roteiro do fluxo visual da SB-010 contra o produto empacotado.
- Publicar o draft como prerelease sem alterar artefatos, tag ou versão.
- Remover a branch-canário remota `test/trunk-check-blocking2`, cuja finalidade
  de provar o bloqueio de checks já foi cumprida.
- Registrar evidências, limitações e período de validação antes de decidir entre
  `v1.0.0-rc.4` e `v1.0.0`.

## Fora do escopo

- Alterar código, pacotes ou os assets já produzidos.
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

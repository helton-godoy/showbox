# SB-013 — Validação externa do RC.4 e decisão v1.0.0

Estado: em andamento (2026-09-12).

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

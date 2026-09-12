# SB-013 — Validação externa do RC.4 e decisão v1.0.0

Estado: proposta (2026-09-12).

- Objetivo: validar o prerelease `v1.0.0-rc.4` em uso externo e decidir, com evidências, entre promover `v1.0.0` ou produzir `v1.0.0-rc.5`.
- Base: `6c3a348` (`main` com a SB-012 concluída via PR #29).
- Branch: `docs/SB-013-validacao-rc4`.
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

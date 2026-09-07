# SB-001 — Fundação do monorepo

- Objetivo: conciliar histórico, importar Studio e compilar/testar pela raiz.
- Base: showbox a5dd820; main eab3300; Studio 38602a5.
- Branch: chore/SB-001-monorepo.
- Responsável: integrador desta sessão.
- Escopo: estrutura, CMake, automação, instruções, testes de integração da importação.
- Fora do escopo: reescrever modelo, corrigir todo o exportador, publicar releases.
- Dependências: Qt6 de desenvolvimento.
- Aceite: build dos dois binários, testes existentes e compatibilidade do motor;
  limitações do Studio explicitamente registradas.

## Handoff

Build conjunto aprovado em KDE neon 24.04, GCC 13.3 e Qt 6.11.1.
Primeira execução: 16/17 testes aprovados; persistência esperava pushbutton,
mas a factory importada já normaliza para button. O teste foi corrigido para
verificar preservação do tipo e classe QPushButton após salvar/reabrir.
Instalação CMake em build/stage aprovada, incluindo ambos os executáveis e
os recursos desktop do Studio. Validação final: 19/19 testes CTest aprovados, incluindo comparação byte a byte
com o oráculo legado e teste de isolamento de worktrees (três cenários).
Studio instalado permaneceu ativo durante smoke offscreen de dois segundos;
isso não valida interação humana nem o fluxo de ações.
`just check` e parsing YAML aprovados. Histórico Studio importado em 337f3fe;
main conciliada em a5602c9. Consulte git log da branch para SHA final desta entrega.

## Pendências explícitas

- CI remoto e sanitizers não executados nesta sessão.
- Pacotes DEB/RPM/AppImage não reconstruídos nesta sessão; receitas antigas são
  explicitamente apenas do motor até SB-005, para não incorporar Studio experimental.
- Geração/preview/ações permanecem SB-002; testes antigos não cobrem esse ciclo.
- Proteção remota, publicação e arquivamento não realizados.

## Próximo passo

Revisar SB-001 e iniciar o contrato SB-002-shell-dispatch em worktree própria,
a partir do commit aprovado desta fundação.

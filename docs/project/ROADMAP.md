# Roadmap da suíte Showbox

Fonte oficial de prioridades. O diretório v1 registra o ciclo anterior do motor.
Somente o integrador altera esta tabela.

| Tarefa | Entrega | Dependências | Estado |
| --- | --- | --- | --- |
| SB-001 | Histórico conciliado, monorepo, build e instruções únicos | — | Concluída localmente |
| SB-002 | Corrigir exportação/preview e fechar interface → Bash → interface | SB-001 | Planejada |
| SB-003 | Modelo versionado e salvar/reabrir ações sem perdas | SB-002 | Planejada |
| SB-004 | Catálogo compartilhado de propriedades/eventos suportados | SB-003 | Planejada |
| SB-005 | Pacotes separados, pipeline de release e testes de instalação | SB-001, SB-002 | Planejada |
| SB-006 | Proteção Git remota e transição dos repositórios antigos | SB-005 | Planejada |

## Marco de produto
Criar campo, botão e rótulo; associar ação Bash; clicar e atualizar o rótulo;
salvar, reabrir, exportar e executar sem Studio. Testar contra o motor real.

## Limites conhecidos na origem
O gerador coleta callbacks depois de tentar emiti-los. O preview envia Bash
como se fosse protocolo. Há widgets/opções experimentais fora do contrato v1.
Não declarar a suíte pronta com base apenas em testes unitários antigos.

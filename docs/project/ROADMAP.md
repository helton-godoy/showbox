# Roadmap da suíte Showbox

Fonte oficial de prioridades. O diretório v1 registra o ciclo anterior do motor.
Somente o integrador altera esta tabela.

| Tarefa  | Entrega                                                                 | Dependências   | Estado                         |
| ------- | ----------------------------------------------------------------------- | -------------- | ------------------------------ |
| SB-001  | Histórico conciliado, monorepo, build e instruções únicos               | —              | Concluída localmente           |
| SB-002  | Corrigir exportação/preview e fechar interface → Bash → interface       | SB-001         | Concluída localmente           |
| SB-003  | Modelo versionado e salvar/reabrir ações sem perdas                     | SB-002         | Concluída localmente           |
| SB-004  | Catálogo compartilhado de propriedades/eventos suportados               | SB-003         | Concluída localmente           |
| SB-005  | Pacotes separados, pipeline de release e testes de instalação           | SB-001, SB-002 | Concluída localmente           |
| SB-006  | Proteção Git remota e transição dos repositórios antigos                | SB-005         | Concluída                      |
| SB-006A | Correção: segurança de abertura/salvamento e validação de projeto       | SB-005         | Concluída e integrada          |
| SB-007  | Trunk como ferramenta central de validação (lint, formatação, segredos) | SB-006         | Concluída                      |
| SB-008  | Primeiro release candidato e validação dos artefatos                    | SB-005, SB-007 | Concluída (rc.3, corretivo P1) |
| SB-009  | Contrato e validação da versão única (testes do conversor)              | SB-008         | Concluída                      |
| SB-009A | Correção: ordenação RPM da versão única entre estágios                  | SB-009         | Concluída                      |
| SB-009B | Testes de ordenação ligados à saída real do conversor                   | SB-009A        | Concluída                      |
| SB-009C | Smoke RPM valida Version/Release realmente instalados                   | SB-009B        | Concluída                      |
| SB-010  | Fluxo visual executável de ponta a ponta                                | SB-004         | Concluída                      |
| SB-011  | Simplificar integração Git para branches de tarefa → main               | SB-006, SB-007 | Concluída                      |
| SB-012  | Revalidar e publicar o release candidate v1.0.0-rc.3                    | SB-008–SB-011  | Em andamento                   |

## Marco de produto

Criar campo, botão e rótulo; associar ação Bash; clicar e atualizar o rótulo;
salvar, reabrir, exportar e executar sem Studio. Testar contra o motor real.

## Limites conhecidos na origem

Salvar/reabrir persistia conforme a árvore QWidget e ignorava a versão; a SB-003
extrai o modelo versionado e a validação. A SB-004 centraliza tipos, aliases,
containers, exportáveis e eventos em libs/catalog sem novas exportações nem
mudanças no protocolo; políticas de UI específicas (checkable, layout rows do
PropertyEditor e alvos de drop/árvore do ObjectInspector) permanecem explícitas,
documentadas na ADR 0004. Há widgets/opções experimentais fora do contrato v1
(grid/form não exportáveis). Não declarar a suíte pronta com base apenas em
testes unitários antigos.

# Consolidação de origem

Data: 2026-09-06. Implementação na branch chore/SB-001-monorepo.

| Origem | Revisão | Destino |
| --- | --- | --- |
| showbox integration/showbox-v1 | a5dd820 | Motor da suíte |
| showbox main | eab3300 | Histórico conciliado |
| showbox-studio origin/develop | 38602a5 | apps/studio, importação subtree sem squash |
| SHantilly main | 33c4b06 | Referência, sem importação nesta etapa |
| SHantilly-Studio main | eda620c | Referência, sem importação nesta etapa |

A main antiga removia documentação e adicionava binário legado. A conciliação
mantém a documentação útil da integração e não reintroduz o binário gerado.
O histórico do Studio é preservado no commit de importação. Automação duplicada,
playbooks genéricos e instruções obsoletas foram retirados da árvore ativa;
continuam acessíveis no histórico de origem.

Movimentos principais: src/code/showbox → apps/runtime; libs/showbox-ui → libs/ui.
Os nomes de alvos CMake permanecem compatíveis. O oráculo legado é compilado à
parte, nunca vinculado aos alvos estáveis. As cópias originais não foram movidas.

Não houve push, publicação, arquivamento de repositórios ou proteção remota.
A validação da fundação consta em tasks/SB-001-monorepo.md.

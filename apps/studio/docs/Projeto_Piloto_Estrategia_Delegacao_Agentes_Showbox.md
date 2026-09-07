# Projeto Piloto: Estratégia de Delegação de Agentes (Showbox)

## 1. Visão Executiva

Este documento define a estratégia revisada para o workflow de desenvolvimento do Showbox Studio, priorizando a eficiência e o custo-benefício. A premissa central é alocar Agentes de Inteligência Artificial de acordo com suas forças específicas:

- **Codificação (High-Perf):** Gemini CLI (Prioridade 1) e OpenCode (Prioridade 2).
- **Gestão e Planejamento:** Kilocode e Cline (via scripts administrativos).
- **Contexto:** `ai-coders-context` (MCP Server).

## 2. Matriz de Responsabilidades (RACI)

| Atividade                | Responsável (Agente) | Ferramenta           | Justificativa                                                  |
| :----------------------- | :------------------- | :------------------- | :------------------------------------------------------------- |
| **Arquitetura**          | Architect            | **Kilocode / Cline** | Capacidade de raciocínio lógico e quebra de tarefas complexas. |
| **Codificação**          | Coder                | **Gemini CLI**       | Otimizado para contexto largo (1M tokens) e velocidade.        |
| **Codificação (Backup)** | Coder (Alt)          | **OpenCode**         | Versatilidade de modelos (MiniMax, GLM, etc.) gratuitos.       |
| **QA / Testes**          | QA Agent             | **Gemini CLI**       | Excelente para gerar casos de teste baseados em specs.         |
| **Documentação**         | Doc Agent            | **Cline + Context7** | Integração com MCP permite acesso a docs atualizados.          |
| **Gestão (PM)**          | Manager              | **Gemini CLI**       | Automação headless leve para atualizar o Conductor.            |

## 3. Avaliação OpenCode (Benchmark)

Para validar a qualidade dos modelos gratuitos disponíveis no OpenCode, definimos o seguinte protocolo de teste.

### Modelos Alvo

1. **MiniMax M2.1** (`opencode/minimax-m2.1-free`)
2. **GLM-4.7** (`opencode/glm-4.7-free`)
3. **Grok Code Fast** (`opencode/grok-code`)
4. **Big Pickle** (`opencode/big-pickle`)

### Tarefa Padronizada

Desenvolvimento de uma classe C++ `StringUtils`:

- C++17 padrão.
- Qt6 (`QString`).
- Testes unitários incluídos.
- Documentação Doxygen.

### Execução

Utilizar o script `scripts/evaluation/run_benchmark.sh` para executar a tarefa em todos os modelos sequencialmente e gerar os artefatos de código para comparação humana.

### Resultados da Execução (17/01/2026)

| Modelo           | Status      | Análise                                                                                                                                                       |
| :--------------- | :---------- | :------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| **MiniMax M2.1** | **SUCESSO** | Gerou `.h`, `.cpp`, testes `QTest`, configurou `CMakeLists.txt`, compilou e rodou os testes. 24 Passaram, 3 Falharam (Edge cases de acrônimos). **Vencedor.** |
| **GLM-4.7**      | FALHA       | Não interpretou corretamente o comando `run`.                                                                                                                 |
| **Grok Code**    | FALHA       | Não interpretou corretamente o comando `run`.                                                                                                                 |
| **Big Pickle**   | FALHA       | Não interpretou corretamente o comando `run`.                                                                                                                 |

### Conclusão do Benchmark

O modelo **MiniMax M2.1** demonstrou capacidade autônoma superior, não apenas gerando código, mas interagindo com o sistema de build e execução. Ele será o modelo padrão para fallback no `coder.sh`.

## 4. Otimização do Workflow (Toolbelt 2.0)

Os scripts em `scripts/swarm/` serão refatorados para refletir esta estratégia.

### Estrutura de Scripts Proposta

#### `scripts/swarm/architect.sh` (Planning)

- **Engine:** Kilocode (Prioritário) ou Cline.
- **Função:** Recebe requisitos macro, gera plano em Markdown e tasks no Conductor.

#### `scripts/swarm/coder.sh` (Execution)

- **Engine:** Gemini CLI.
- **Fallback:** OpenCode (Buscando o melhor modelo do benchmark).
- **Sintaxe:** `coder.sh "Tarefa" [arquivos]`
- **Automação:** Notifica o Conductor ao finalizar (via Callback).

#### `scripts/swarm/qa.sh` (Verification)

- **Engine:** Gemini CLI.
- **Função:** Analisa o código gerado pelo Coder e cria testes.

#### `scripts/swarm/manager.sh` (Management)

- **Engine:** Gemini CLI (Headless).
- **Função:** Interface via CLI para o sistema Conductor.

## 5. Integração MCP (Contexto e Documentação)

A ferramenta `ai-coders-context` ([GitHub](https://github.com/helton-godoy/ai-coders-context)) será utilizada como servidor MCP para fornecer:

1. **Contexto do Projeto:** indexação de arquivos.
2. **Documentação Externa:** via Context7 (se disponível).

**Configuração:**

- Cline e Kilocode devem ser configurados para conectar a este servidor MCP.
- Script `scripts/setup_context.sh` gerencia esta inicialização.

## 6. Próximos Passos

1. Executar Benchmark (Usuário): `./scripts/evaluation/run_benchmark.sh`
2. Analisar resultados e eleger o modelo "OpenCode de Ouro".
3. Atualizar scripts do Toolbelt com a lógica de delegação definida.

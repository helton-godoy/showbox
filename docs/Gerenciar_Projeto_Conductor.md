Para gerenciar o Conductor na linha de comando sem a TUI do Gemini CLI, utilize o modo "headless". Este modo permite executar comandos, scripts e automações diretamente, retornando ao terminal após a conclusão da tarefa.

Como fazer:

1. **Usando a flag `--prompt` (Modo Headless)**

   Use o comando `gemini --prompt "..."` seguido de um comando do Conductor.
   - Configurar o Conductor:

   ```bash
   gemini --prompt "/conductor:setup"
   ```

   - Criar uma nova "Track":

   ```bash
   gemini --prompt "/conductor:newTrack Adicionar login com Google"
   ```

   - Implementar o plano da Track:

   ```bash
   gemini --prompt "/conductor:implement"
   ```

2. **Usando Stdin (Entrada de comandos)**

   Passe o comando via `echo` ou `printf` para o `gemini`, útil em scripts Bash:

   ```bash
   echo "/conductor:status" | gemini
   ```

3. **Exemplo em Script**

   Para planejar e implementar sem a TUI:

   ```bash
   # Define o contexto do projeto
   gemini --prompt "/conductor:setup"

   # Cria a tarefa
   gemini --prompt "/conductor:newTrack Criar estrutura de pastas"

   # Executa a implementação
   gemini --prompt "/conductor:implement"
   ```

**Vantagens do Modo Headless com Conductor:**

- **Automação:** Ideal para CI/CD ou scripts (cron jobs).
- **Velocidade:** Evita a TUI.
- **Contexto Persistente:** O Conductor salva "tracks" e "specs" em arquivos Markdown, mantendo o contexto do projeto.

**Dicas adicionais:**

- Verifique as versões mais recentes do CLI para evitar travamentos, pois o Conductor foi lançado no final de 2025 e recebeu ajustes em seu modo headless.
- Use `gemini --help` para mais opções de automação.

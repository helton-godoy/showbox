# ADR 0002 — Execução shell da primeira aplicação exportável

Estado: implementado na SB-002, sujeito aos testes de integração desta tarefa.

O exportador incorpora shell/lib/runtime.sh no arquivo Bash. Não depende da árvore
fonte em execução. SHOWBOX_BIN escolhe o motor (default: showbox no PATH).
Uma instância do motor recebe comandos em stdin e emite eventos em stdout por duas FIFOs privadas em diretório temporário. Logs das ações usam stdout/stderr do Bash e nunca entram no protocolo.

Callbacks executam sequencialmente, cada um em uma sessão de processos própria.
A janela continua responsiva; fechar a janela interrompe a ação. O Studio também
oferece Parar e encerra sua sessão ao fechar. Processos de ações não são serviços:
subprocessos restantes são encerrados quando o callback termina.

showbox_set widget propriedade valor envia tokens escapados. showbox_get VAR widget
consulta um valor escalar com prazo de três segundos e atribuição sem eval. Outros
eventos lidos durante a consulta são enfileirados. O protocolo v1 não tem IDs de
requisição: não há consultas concorrentes nem suporte a respostas multilinha.
Uma resposta de mudança do mesmo widget pode atender a consulta; o valor é sempre
um valor publicado pelo motor, não uma transação com isolamento.

Nomes são identificadores ASCII únicos. Shell é código explícito do programador;
texto e valores dos componentes são dados escapados. Tipos/eventos não suportados
causam erro de exportação. A lista inicial é documentada em docs/user/shell-actions.md.
Prévia visual envia somente o protocolo para o motor, sem iniciar shell.

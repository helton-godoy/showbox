# Aplicações visuais com Bash

## Experimente no Studio

1. Execute `just run-studio` na worktree.
2. No menu File, escolha **Nova demonstração campo/botão/rótulo**. Isso substitui
   o canvas atual; salve seu trabalho antes. Também é possível abrir
   `examples/hello-world/hello-world.sbxproj`.
3. Selecione o botão Saudar e abra a aba Ações. Selecione a ação para editar o código.
4. Use **Prévia visual** para conferir a interface sem executar nenhuma ação.
5. Use **Executar aplicação** (F5), preencha o campo e clique em Saudar.
6. O rótulo recebe o valor e os logs aparecem no painel inferior. **Parar** encerra
   a aplicação e seus subprocessos. Fechar o Studio também encerra a execução.
7. Use **Exportar aplicação Bash…**. Execute o arquivo com `bash /caminho/app.sh`.

O arquivo exportado incorpora a camada de comunicação: não precisa do Studio nem
da árvore do repositório. Precisa de Bash 4.3+, `setsid` (util-linux) e `showbox`
instalado no PATH, ou `SHOWBOX_BIN=/caminho/absoluto/showbox`. Dependências externas
citadas pelo código do programador continuam sob sua responsabilidade.

## Escrever ações

````bash
showbox_get VALUE entry
showbox_set result text "Olá: $VALUE"
printf 'Ação concluída.\n'
```text

- `showbox_get VAR componente`: consulta um valor escalar na janela atual, em até
  três segundos. A variável não leva `$` e deve começar por uma letra.
- `showbox_set componente propriedade valor`: envia um comando `set` escapado.
  A semântica da propriedade é a do protocolo v1; por exemplo, checked é uma flag,
  não um booleano textual. Para removê-la use `showbox_send unset nome checked`.
- `showbox_send comando argumentos...`: envia tokens escapados para a mesma janela.
- `$WIDGET_NAME`, `$WIDGET_VALUE`, `$EVENT` e `$TIMESTAMP`: contexto do evento.
- stdout e stderr da ação são logs. Não use echo para enviar comandos ao motor.

As ações de um evento são executadas na ordem da lista, no mesmo callback Bash.
Um comando que retorna erro interrompe esse callback e o código é registrado no
log; um próximo evento ainda pode executar. Variáveis comuns não persistem entre
callbacks. Não coloque daemons em segundo plano: ao terminar uma ação, seus
subprocessos restantes são encerrados.

Os tipos visuais **Modificar componente** e **Consultar componente** também são
suportados. O campo Valor literal não expande `$VAR`; use Comando Bash quando
precisar de um valor dinâmico. A consulta aceita textbox, checkbox, radiobutton e
slider. Prévia visual ignora o código das ações, inclusive código com erro.

## Escopo da primeira versão

Componentes exportáveis: label, pushbutton, textbox, checkbox, radiobutton, slider,
progressbar, groupbox, frame, tabs e page. A ordenação é a dos layouts do canvas.
Containers lineares são suportados; grade e formulário ainda não são exportáveis.
Propriedades básicas: texto/título, estado inicial de seleção, valor/faixa dos
controles numéricos, orientação, campo password/readonly e enabled. Fontes, cores,
geometria livre, estilos e todas as propriedades avançadas ainda não têm paridade.

Eventos: clicked em botão simples, pressed/released em botão alternável e changed
em slider. Textbox e checkboxes são consultáveis, mas não oferecem callbacks de
mudança nesta etapa. Tipos, nomes ou eventos não suportados são recusados na exportação.
Nomes precisam ser identificadores ASCII únicos, sem espaços. main e showbox são
reservados. Os valores transportados nesta etapa precisam caber em uma linha;
respostas multilinha e consultas simultâneas exigem um contrato posterior.

Salvar/reabrir agora preserva o JSON das ações. Isso não substitui a SB-003:
modelo independente dos widgets, validação do formato, layouts e persistência
completa continuam sendo a próxima etapa.
````

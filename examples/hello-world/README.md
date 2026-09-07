# Campo → Bash → rótulo

No Studio, abra hello-world.sbxproj ou use File → Nova demonstração
campo/botão/rótulo. Selecione a ação do botão para editar o código.

Na árvore de desenvolvimento, `just demo` executa run.sh com o motor compilado.
Também é possível executar de qualquer diretório:

```sh
SHOWBOX_BIN=/caminho/absoluto/showbox bash /caminho/do/repo/examples/hello-world/run.sh
```

run.sh reutiliza shell/lib/runtime.sh do repositório. Para obter um arquivo
independente, use Exportar aplicação Bash no Studio: a biblioteca é incorporada
no arquivo exportado. O arquivo precisa apenas de Bash, util-linux e Showbox,
além de eventuais ferramentas usadas no código de suas próprias ações.

Saudar consulta entry e altera result na mesma janela. Valores com aspas, espaços
e expressões shell são transportados como dados. A demonstração não altera
arquivos do usuário. Consulte docs/user/shell-actions.md na raiz para limitações.

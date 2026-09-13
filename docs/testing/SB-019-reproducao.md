# SB-019 — roteiro de reprodução da validação do RC.6

Roteiro operacional para outro LLM repetir a validação do motor Showbox e do
Showbox Studio. A execução de referência ocorreu em 13/09/2026 sobre `main`
em `5bfe080e7e6765e9af39412fc182d8bec2404bc8` e os artefatos publicados
`v1.0.0-rc.6`.

> Execute a partir da raiz do repositório. `build/sb019` e os contêineres são
> descartáveis; não adicione seus binários ao Git. A validação não publica
> releases.

## 1. Preparação e gates

```bash
cd /home/helton/Public/fork_dialogbox/showbox
git fetch origin main
git switch -c chore/SB-019-validacao 5bfe080e7e6765e9af39412fc182d8bec2404bc8
mkdir -p build/sb019/{assets,gui,deb-ubuntu,deb-debian,rpm}
git status --short --branch
git rev-parse HEAD
git describe --always --dirty
just doctor
just build
just test
just check
```

O resultado esperado é `28/28` testes, Trunk sem issues e a proteção da
`main` identificada pelo doctor. Se o sandbox bloquear `QLocalServer`, repita
`just test` fora dele; não aceite um `skip` como validação do transporte.

## 2. CI, estado externo, assets e smokes

```bash
gh run list --repo helton-godoy/showbox --limit 10 \
  --json headSha,name,status,conclusion,createdAt,url
gh issue list --repo helton-godoy/showbox --state open \
  --json number,title,labels,url
gh pr list --repo helton-godoy/showbox --state open \
  --json number,title,url,headRefName
gh run view <RUN_ID> --repo helton-godoy/showbox --json jobs
```

Baixe os sete arquivos e confira os hashes:

```bash
gh release download v1.0.0-rc.6 --repo helton-godoy/showbox \
  --pattern 'showbox*' --pattern 'ShowBox*' --dir build/sb019/assets
find build/sb019/assets -maxdepth 1 -type f -printf '%f\n' | sort
sha256sum build/sb019/assets/*
```

Valores de referência:

```text
AppImage  9222ad0827c230ad9802dc80a5bb9955df67eb41be55faa1750b968f6c2ebd3a
engine RPM 24b336c35e8fdbc9fd65a809c73819fd876249cd9e411efa811ce0ff2574b672
Studio RPM 3d5dc164ee02697e3f7b7386fc2c3435a668fb54676681abf3e1fcc0d8434da6
Studio DEB 80c7420718b721b7733e490db3da217e5a9b803ed0544df2b2cfde9d4f856b2d
Studio Ubuntu d0e0e58450d6d910b7e0f9d1f7e5f96fa82a41785f417d173f15940c8a6df99a
engine DEB aaa8bbf270d60e8da292ec0eadbf3ed8b5195543b521d4b4bacd6c758ab8d9ed
engine Ubuntu 53fbfa2aa859c22037f500aa399554f4bc46df0b1fbd0f4c8462915bba6c9446
```

Separe os pacotes e rode os smokes:

```bash
mkdir -p build/sb019/{deb-ubuntu,deb-debian,rpm}
cp build/sb019/assets/showbox_1.0.0.rc6-1_ubuntu24.04_amd64.deb \
   build/sb019/assets/showbox-studio_1.0.0.rc6-1_ubuntu24.04_amd64.deb build/sb019/deb-ubuntu/
cp build/sb019/assets/showbox_1.0.0.rc6-1_debian13_amd64.deb \
   build/sb019/assets/showbox-studio_1.0.0.rc6-1_debian13_amd64.deb build/sb019/deb-debian/
cp build/sb019/assets/showbox-1.0.0-0.3.rc6.fc46.x86_64.rpm \
   build/sb019/assets/showbox-studio-1.0.0-0.3.rc6.fc46.x86_64.rpm build/sb019/rpm/
just pkg-install-smoke deb build/sb019/deb-ubuntu ubuntu
just pkg-install-smoke deb build/sb019/deb-debian debian
just pkg-install-smoke rpm build/sb019/rpm
just pkg-install-smoke appimage build/sb019/assets/ShowBox-Studio-1.0.0-rc.6-x86_64.AppImage
```

Esperado: `smoke deb ubuntu: OK`, `smoke deb debian: OK`, `smoke rpm: OK` e
`smoke appimage: OK`, todos reportando `1.0.0-rc.6`.

## 3. Automação JSON-RPC, CLI e MCP

Instale os DEBs no ambiente de teste e confira os executáveis:

```bash
sudo apt-get install ./build/sb019/assets/showbox_1.0.0.rc6-1_ubuntu24.04_amd64.deb ./build/sb019/assets/showbox-studio_1.0.0.rc6-1_ubuntu24.04_amd64.deb
showbox --version
showbox-studio --version
showbox-studioctl --help
showbox-studio-mcp --help
```

Interface desligada por padrão:

```bash
rm -f /tmp/sb019-disabled.sock
QT_QPA_PLATFORM=offscreen showbox-studio --automation-socket /tmp/sb019-disabled.sock &
PID=$!; sleep 1
test ! -S /tmp/sb019-disabled.sock
set +e; showbox-studioctl --socket /tmp/sb019-disabled.sock system.describe; test $? -ne 0; set -e
kill "$PID" 2>/dev/null || true
```

Sessão principal:

```bash
rm -f /tmp/sb019.sock
QT_QPA_PLATFORM=offscreen showbox-studio --automation --automation-socket /tmp/sb019.sock >/tmp/sb019.log 2>&1 &
PID=$!; sleep 1
showbox-studioctl --socket /tmp/sb019.sock system.describe
showbox-studioctl --socket /tmp/sb019.sock project.snapshot
showbox-studioctl --socket /tmp/sb019.sock ui.tree
showbox-studioctl --socket /tmp/sb019.sock diagnostics.list
showbox-studioctl --socket /tmp/sb019.sock --params '{"type":"textbox","name":"entry"}' widget.add
showbox-studioctl --socket /tmp/sb019.sock --params '{"name":"entry","property":"text","value":"RC6"}' widget.setProperty
showbox-studioctl --socket /tmp/sb019.sock --params '{"name":"invalid-name","type":"label"}' widget.add || test $? -ne 0
showbox-studioctl --socket /tmp/sb019.sock --params '{"name":"main","type":"label"}' widget.add || test $? -ne 0
showbox-studioctl --socket /tmp/sb019.sock --params '{"name":"entry","property":"unknown","value":"x"}' widget.setProperty || test $? -ne 0
showbox-studioctl --socket /tmp/sb019.sock history.undo
showbox-studioctl --socket /tmp/sb019.sock history.redo
showbox-studioctl --socket /tmp/sb019.sock --params '{"file":"build/sb019/automation.sbxproj"}' project.save
showbox-studioctl --socket /tmp/sb019.sock --params '{"file":"build/sb019/automation.sbxproj","force":true}' project.open
showbox-studioctl --socket /tmp/sb019.sock preview.start || test $? -ne 0
kill "$PID" 2>/dev/null || true
```

Erros esperados: `-32011` sem `--automation-allow-execution`, `-32010` em
somente leitura, `-32020` para nome/widget inválido e `-32602` para parâmetros
inválidos. Os schemas de tabela, combobox, spinbox, tabs e ações devem ser
obtidos por `system.describe` na versão em teste.

```bash
rm -f /tmp/sb019-readonly.sock
QT_QPA_PLATFORM=offscreen showbox-studio --automation --automation-read-only --automation-socket /tmp/sb019-readonly.sock >/tmp/sb019-ro.log 2>&1 &
RO_PID=$!; sleep 1
showbox-studioctl --socket /tmp/sb019-readonly.sock system.describe
showbox-studioctl --socket /tmp/sb019-readonly.sock --params '{"type":"label","name":"blocked"}' widget.add || test $? -ne 0
kill "$RO_PID" 2>/dev/null || true
```

### Eventos e MCP

Use duas conexões Unix persistentes para eventos: uma para assinar e outra
para controlar. A referência usada foi `build/sb019/event_probe.py`:

```bash
python3 build/sb019/event_probe.py | tee build/sb019/event_probe.json
```

Ela deve contar exatamente uma ocorrência de `project.changed`,
`selection.changed`, `dirty.changed` e `preview.finished`.

O MCP é stdio; `tools/list` não deve publicar `events.subscribe`:

```bash
rm -f /tmp/sb019-exec.sock
QT_QPA_PLATFORM=offscreen showbox-studio --automation --automation-allow-execution --automation-socket /tmp/sb019-exec.sock >/tmp/sb019-exec.log 2>&1 &
EXEC_PID=$!; sleep 1
printf '%s\n' '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05","capabilities":{},"clientInfo":{"name":"sb019","version":"1"}}}' '{"jsonrpc":"2.0","id":2,"method":"tools/list","params":{}}' '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"project.snapshot","arguments":{}}}' '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"events.subscribe","arguments":{"events":[]}}}' | showbox-studio-mcp --socket /tmp/sb019-exec.sock | tee build/sb019/mcp-transcript.jsonl
kill "$EXEC_PID" 2>/dev/null || true
```

## 4. Fluxo visual em Xvfb (C/D)

```bash
podman rm -f showbox-sb019-gui 2>/dev/null || true
podman run --name showbox-sb019-gui -d --rm -v "$PWD/build/sb019/assets:/pkg:ro" -v "$PWD/build/sb019/gui:/evidence:rw" ubuntu:24.04 sleep infinity
podman exec showbox-sb019-gui bash -lc 'apt-get update -qq && DEBIAN_FRONTEND=noninteractive apt-get install -y -qq /pkg/showbox_1.0.0.rc6-1_ubuntu24.04_amd64.deb /pkg/showbox-studio_1.0.0.rc6-1_ubuntu24.04_amd64.deb xvfb xdotool imagemagick openbox dbus-x11 at-spi2-core'
podman exec -d showbox-sb019-gui bash -lc 'rm -f /tmp/.X99-lock; Xvfb :99 -screen 0 1440x1000x24 -nolisten tcp >/evidence/xvfb.log 2>&1 & sleep 1; DISPLAY=:99 openbox >/evidence/openbox.log 2>&1 &'
podman exec -d showbox-sb019-gui bash -lc 'export DISPLAY=:99; QT_ACCESSIBILITY=1 showbox-studio >/evidence/studio.log 2>&1'
sleep 2
podman exec showbox-sb019-gui bash -lc 'export DISPLAY=:99; xdotool search --name "Showbox Studio" getwindowgeometry'
```

Use o ID retornado (`$GUI_WIN`) e coordenadas relativas à janela 1200×800.
Crie o trio campo/botão/rótulo, renomeie para `entry`, `run`, `result` e, na
aba **Ações** de `run`, escolha evento `clicked`, tipo `Comando Bash` e informe:

```text
showbox_get VALUE entry
showbox_set result text OK
```

Salve com Ctrl+S em `/evidence/visual-basic.sbxproj`, execute pelo toolbar,
digite `abc`, clique no botão e capture:

```bash
podman exec showbox-sb019-gui bash -lc 'export DISPLAY=:99; import -window root /evidence/37-preview-action.png'
```

No menu **File → Exportar aplicação Bash...**, salve `/evidence/visual-basic.sh`
e execute fora do Studio:

```bash
podman exec -d showbox-sb019-gui bash -lc 'export DISPLAY=:99; /evidence/visual-basic.sh >/evidence/exported.log 2>&1'
podman exec showbox-sb019-gui bash -lc 'export DISPLAY=:99; xdotool search --name "Showbox" getwindowgeometry'
```

Digite `xyz`, clique `Button` e capture `/evidence/42-exported-action.png`.
Para widget não exportável, use um projeto contendo `table` e confirme que
`preview.start` recusa com mensagem clara.

## 5. Sessão exploratória F (30 minutos)

Copie e abra pelo diálogo (Ctrl+O, Ctrl+L) um cenário com abas, tabela,
combobox e spinbox:

```bash
cp build/sb019/automation.sbxproj build/sb019/gui/automation.sbxproj
xdotool key --window "$GUI_WIN" ctrl+o
xdotool key ctrl+l
xdotool type --clearmodifiers /evidence/automation.sbxproj
xdotool key Return
```

Alterne os controles por 30 minutos reais, registrando cada minuto:

```bash
rm -f build/sb019/gui/f-session.log
for i in $(seq 1 30); do
  printf '%s iteration=%s\n' "$(date -Is)" "$i" | tee -a build/sb019/gui/f-session.log
  xdotool mousemove --window "$GUI_WIN" 550 307 click 1
  xdotool key --window "$GUI_WIN" Down
  xdotool key --window "$GUI_WIN" Return
  xdotool mousemove --window "$GUI_WIN" 400 336 click 1
  xdotool key --window "$GUI_WIN" ctrl+a
  xdotool type --window "$GUI_WIN" "$((i%20+1))"
  xdotool key --window "$GUI_WIN" Return
  xdotool mousemove --window "$GUI_WIN" $((300+(i%3)*60)) 364 click 1
  sleep 60
done
```

Também exercite `Ctrl+Z`/`Ctrl+Y`, menu contextual de dois widgets → `Group in
Frame`, `View → Toolbox Style → Tree` e `Help → About Showbox Studio`. Antes
de sair, faça uma alteração não salva e use Ctrl+Q: deve aparecer `O projeto
atual tem alterações não salvas. Descartar?`. Escolha **No/Cancel** e depois
repita escolhendo **Yes**.

Se a janela encerrar sem confirmação, preserve `studio.log`,
`f-session-run.log` e a última captura. Isso é bloqueador de promoção, não
cosmético. Ao terminar, pare somente o contêiner:

```bash
podman stop showbox-sb019-gui
```

## 6. Encerramento e decisão

```bash
git diff --check
just check
gh issue list --repo helton-godoy/showbox --state open
gh pr list --repo helton-godoy/showbox --state open
git status --short --branch
git rev-parse HEAD
git log -1 --oneline
```

A–E precisam estar 100% verdes. Qualquer crash, perda de dados, quebra de
contrato, evento incorreto ou pacote divergente exige `rc.7`; somente achados
cosméticos podem ir para backlog. Versione apenas Markdown e deixe imagens,
logs e pacotes em `build/sb019` (ignorado). Não crie a tag `v1.0.0` durante a
validação.

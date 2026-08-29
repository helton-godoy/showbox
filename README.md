# Showbox

Showbox builds Qt 6 graphical interfaces from commands read on stdin and emits
user actions on stdout. It is a KDE Plasma 6 compatible successor to Dialogbox,
designed for shell scripts while preserving the established command contract.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
QT_QPA_PLATFORM=offscreen ctest --test-dir build --output-on-failure
```

Example:

```bash
showbox <<'EOF'
set title "Example"
add label "Hello from Showbox" greeting
add pushbutton "Close" close exit default
show
EOF
```

Documentation starts at [docs/index.md](docs/index.md). The refactoring status
and isolated-session work packages are tracked in
[docs/project/v1/ROADMAP.md](docs/project/v1/ROADMAP.md).

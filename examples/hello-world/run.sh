#!/usr/bin/env bash
# Demonstração de desenvolvimento; a exportação pelo Studio incorpora a biblioteca.
set -o pipefail
showbox_example_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
# shellcheck source-path=SCRIPTDIR
# shellcheck source=../../shell/lib/runtime.sh
source "$showbox_example_dir/../../shell/lib/runtime.sh"

_sb_ui() {
    cat <<'SHOWBOX_UI'
set title "Olá, Showbox"
add textbox "Nome" entry
add pushbutton "Saudar" run
add label "Preencha o nome e clique em Saudar." result
show
SHOWBOX_UI
}

_sb_greet() {
    showbox_get VALUE entry
    showbox_set result text "Olá: $VALUE"
    printf 'Ação concluída.\n'
}

_sb_dispatch() {
    case "$1" in
        run=clicked) _sb_launch _sb_greet run clicked "${1#*=}" ;;
    esac
}

_sb_run

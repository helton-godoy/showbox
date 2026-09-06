#!/usr/bin/env python3
"""Cria uma worktree com contrato previamente revisado."""
from pathlib import Path
import re
import subprocess
import sys


def git(*args):
    return subprocess.check_output(['git', *args], text=True).strip()


def main():
    if len(sys.argv) != 3:
        raise SystemExit('Uso: worktree.py SB-014 shell-dispatch')
    task, topic = sys.argv[1:]
    if not re.fullmatch(r'SB-[0-9]{3,}', task) or not re.fullmatch(r'[a-z0-9]+(?:-[a-z0-9]+)*', topic):
        raise SystemExit('Identificador ou assunto inválido.')
    root = Path(git('rev-parse', '--show-toplevel'))
    if git('status', '--porcelain'):
        raise SystemExit('A árvore base precisa estar limpa antes de criar outra tarefa.')
    contract = root / 'tasks' / f'{task}-{topic}.md'
    if not contract.is_file():
        raise SystemExit(f'Crie e integre primeiro o contrato: {contract}')
    base = git('rev-parse', 'HEAD')
    branch = f'feat/{task}-{topic}'
    first = git('worktree', 'list', '--porcelain').splitlines()[0]
    primary = Path(first.removeprefix('worktree '))
    destination = primary.parent / 'showbox-worktrees' / f'{task}-{topic}'
    if destination.exists():
        raise SystemExit(f'Destino já existe: {destination}')
    print(f'Base: {base}\nContrato: {contract}\nBranch: {branch}', flush=True)
    subprocess.run(['git', 'worktree', 'add', '-b', branch, str(destination), base], check=True)
    print(f'Worktree criada: {destination}')


if __name__ == '__main__':
    main()

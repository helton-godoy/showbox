#!/usr/bin/env python3
"""Verifica isolamento e recusas do criador de worktrees em repositórios temporários."""
from pathlib import Path
import subprocess
import tempfile
import unittest

SCRIPT = Path(__file__).resolve().parents[2] / 'tools/git/worktree.py'


class WorktreeTest(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.repo = Path(self.temp.name) / 'showbox'
        self.repo.mkdir()
        self.git('init', '-q')
        self.git('config', 'user.name', 'Teste Showbox')
        self.git('config', 'user.email', 'teste@example.invalid')
        (self.repo / 'tasks').mkdir()
        (self.repo / 'tasks/SB-014-shell-dispatch.md').write_text('Objetivo: teste isolado.\n')
        self.git('add', '.')
        self.git('commit', '-qm', 'test(git): cria base temporária')

    def git(self, *args):
        return subprocess.check_output(['git', '-C', str(self.repo), *args], text=True).strip()

    def run_tool(self, task='SB-014', topic='shell-dispatch'):
        return subprocess.run(['python3', str(SCRIPT), task, topic], cwd=self.repo,
                              text=True, capture_output=True)

    def test_cria_branch_isolada_e_preserva_base(self):
        base = self.git('rev-parse', 'HEAD')
        branch = self.git('branch', '--show-current')
        result = self.run_tool()
        self.assertEqual(result.returncode, 0, result.stderr)
        target = Path(self.temp.name) / 'showbox-worktrees/SB-014-shell-dispatch'
        self.assertTrue((target / 'tasks/SB-014-shell-dispatch.md').is_file())
        self.assertEqual(self.git('branch', '--show-current'), branch)
        self.assertEqual(self.git('rev-parse', 'feat/SB-014-shell-dispatch'), base)
        self.assertNotEqual(self.run_tool().returncode, 0)

    def test_recusa_arvore_suja(self):
        (self.repo / 'arquivo.txt').write_text('não perder\n')
        result = self.run_tool()
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('limpa', result.stderr)
        self.assertEqual((self.repo / 'arquivo.txt').read_text(), 'não perder\n')

    def test_recusa_sem_contrato_e_argumento_invalido(self):
        self.assertNotEqual(self.run_tool('SB-015').returncode, 0)
        self.assertNotEqual(self.run_tool(topic='../outro').returncode, 0)
        self.assertNotEqual(self.run_tool(topic="'; touch indevido; '").returncode, 0)
        self.assertFalse((self.repo / 'indevido').exists())


if __name__ == '__main__':
    unittest.main()

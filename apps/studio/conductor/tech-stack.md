# Pilha Tecnológica: Showbox Suite

## Linguagens e Frameworks

- **Linguagem Principal:** C++17 (Garante equilíbrio entre performance e recursos modernos).
- **Framework de UI:** Qt 6.6+ (Utilizando QtWidgets para fidelidade desktop, QtCharts para visualização de dados e QtSVG para ícones e ilustrações).
- **Network & I/O:** Qt Network (para possíveis integrações de API).

## Ferramentas de Desenvolvimento

- **Sistema de Build:** CMake (3.16+). A transição de qmake para CMake é prioritária para suportar a modularidade necessária entre Core e Studio.
- **Ambiente de Desenvolvimento:** Obrigatório o uso do container Docker definido em `@src/dev.Dockerfile` para garantir consistência e "pronto para uso".
- **Gerenciamento de Dependências:** CMake FetchContent ou Submodules (para integrar o Core Showbox no Studio).
- **Linter/Formatter:** Ferramentas compatíveis com o padrão C++ (ex: clang-format).

## Arquitetura

- **Estrutura Modular:** O `showbox-suite` é organizado em bibliotecas (`libs/showbox-ui`) e executáveis (`showbox` CLI e `showbox-studio`).
  - **Padrões de Design:**
    - **Builder Pattern:** (`IShowboxBuilder`) para desacoplar a lógica de parsing da criação de widgets, garantindo paridade entre CLI e Studio.
    - **Command Pattern:** Uso de `QUndoStack` e `QUndoCommand` para encapsular ações do usuário, permitindo rastreamento de histórico e reversibilidade (Undo/Redo).
    - **Passive View:** Widgets em `libs/showbox-ui` devem ser "passivos", focando apenas na exibição e sinais, sem lógica de execução de shell. - **Factory:** (`WidgetFactory`) para instanciação dinâmica de componentes.
  - **Observer:** Uso nativo de Signals/Slots do Qt para comunicação entre componentes.

## Distribuição e Empacotamento

- **Estratégia de Empacotamento:** Uso de ambientes Docker isolados e otimizados por formato (AppImage, DEB, RPM, Flatpak), localizados em `packaging/**`.
  - Permite builds paralelos.
  - Gera imagens individualmente menores e mais seguras.
- **Formatos Suportados:**
  - **Linux:** AppImage (via `linuxdeploy`), DEB, RPM, Flatpak.
  - **Outros:** Windows (MSIX) e macOS (DMG) planejados.
- **Deploy:** Scripts automatizados em `packaging/` orquestram o build dentro dos containers.

# Initial Concept

Profissionais de TI, normalmente de operações, que conseguem criar apenas shell scripts e não podem ou não querem se tornar desenvolvedores de software, mas precisam criar soluções visuais, principalmente para processos de automação ou algum tipo de gestão de ativos. Esses profissionais necessitam de um facilitador para acelerar o desenvolvimento ou até guiá-los (como no caso do Showbox-Studio), devido à falta de experiência em design ou programação.

ShowBox é um aplicativo desenvolvido em C++/Qt6 que oferece widgets de interface gráfica para scripts de shell. Ele utiliza Qt6 (incluindo QtWidgets, QtCharts e QtSVG) com o sistema de compilação qmake, inicialmente concebido como um monolito, mas agora em processo de refatoração para adotar CMake, visando uma estrutura modular. O principal objetivo é produzir código de qualidade profissional (nível premium), seguindo as melhores práticas e padrões de design adequados à linguagem e ao propósito, com a expectativa de favorecer o desenvolvimento do showbox-studio.

O Showbox-Studio é uma ferramenta visual intuitiva desenvolvida em Qt6 (C++), projetada para facilitar a criação de interfaces gráficas para o Showbox, uma ferramenta de linha de comando (CLI) especializada em diálogos gráficos.

# Visão do Produto: Showbox Suite

## Conceito Inicial

O **Showbox Suite** é um ecossistema de ferramentas projetado para empoderar profissionais de TI e Operações — que dominam shell scripting, mas não necessariamente desenvolvimento de software tradicional — a criar interfaces gráficas ricas e funcionais (GUIs) para suas automações e ferramentas de gestão. O projeto é composto por dois pilares principais: o **Showbox** (CLI/Engine) e o **Showbox-Studio** (IDE Visual).

## Público-Alvo

- **Profissionais de Operações e SysAdmins:** Indivíduos que criam scripts de automação e gestão de ativos, mas que carecem de ferramentas para criar interfaces visuais modernas.
- **DevOps e Engenheiros de Infraestrutura:** Profissionais que buscam agilizar a criação de dashboards e ferramentas internas sem mergulhar em frameworks de UI complexos.
- **Scripters:** Usuários avançados de shell que desejam elevar a qualidade de suas ferramentas de linha de comando.

## Estratégia de Desenvolvimento e Refatoração

A evolução do Showbox Suite é guiada por uma relação de estrita interdependência entre seus componentes:

- **Refatoração Motivada pelo Studio:** A transição da arquitetura do `showbox` (de monolito qmake para modular CMake) foi impulsionada pela necessidade de viabilizar o desenvolvimento do `showbox-studio`. A estrutura anterior impedia o reuso eficiente de componentes necessários para a IDE visual.
- **Evolução Paralela (Lockstep):** O desenvolvimento ocorre de forma sincronizada. A implementação de recursos no Studio frequentemente demanda a refatoração ou exposição prévia de APIs na biblioteca core do Showbox. Melhorias no Core desbloqueiam novas capacidades no Studio.

## Pilares do Ecossistema

### 1. Showbox (A Fundação)

- **Natureza:** Aplicação CLI e Biblioteca (Core) desenvolvida em C++17 e Qt6.
- **Função:** Fornece uma coleção de widgets de alta fidelidade (nível premium) que podem ser invocados diretamente via shell scripts.
- **Arquitetura:** Modular baseada em CMake, desenhada para ser consumível tanto pelo CLI quanto pelo Studio.
- **Componentes:** Inclui suporte a Qt Widgets, Qt Charts e Qt SVG para renderização de interfaces complexas.

### 2. Showbox-Studio (O Facilitador)

- **Natureza:** Aplicação Desktop GUI desenvolvida em C++17 e Qt6.
- **Função:** Atua como um construtor visual de interfaces (IDE) para o Showbox, consumindo a arquitetura modular do Core.
  - **Recursos Chave:**
    - **Drag-and-Drop:** Construção intuitiva de formulários e dashboards arrastando componentes para um canvas.
    - **Manipulação Hierárquica:** Reordenação dinâmica de widgets e agrupamento em contêineres (Frames/GroupBoxes) via árvore de objetos e menus contextuais.
    - **Edição Reversível:** Sistema completo de Undo/Redo para todas as ações de design (adição, deleção, propriedades e estrutura).
    - **Edição de Propriedades:** Ajuste visual de cores, tamanhos e comportamentos dos widgets. - **Geração de Código:** Compila o layout visual automaticamente em scripts shell prontos para execução pelo Showbox.
  - **Templates:** Modelos pré-configurados para casos de uso comuns de TI.

## Objetivos Estratégicos

- **Qualidade Premium:** Entregar código C++ robusto, modular e seguindo as melhores práticas e padrões de design (Design Patterns).
- **Facilidade de Uso:** Abstrair a complexidade do Qt, permitindo que usuários foquem na lógica de seus scripts enquanto o Showbox cuida da apresentação visual.
- **Integração:** Garantir que o código gerado pelo Studio seja 100% compatível e otimizado para a engine do Showbox.

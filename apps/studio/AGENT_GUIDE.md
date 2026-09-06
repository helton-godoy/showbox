# Meta-Prompt: Desenvolvimento do Showbox Studio (C++ Edition)

> **Objetivo:** Este documento orienta o agente de desenvolvimento a implementar um editor visual desktop para o Showbox utilizando **C++ e Qt6**, maximizando a reutilização de código do projeto original e garantindo alta fidelidade visual.

---

## 🎯 Missão

Desenvolver o **Showbox Studio**, um ambiente de desenvolvimento integrado (IDE) em **C++ (Qt6)** para criação visual de interfaces Showbox. O foco é reutilizar componentes nativos do Showbox (especialmente widgets complexos) enquanto fornece uma interface de edição robusta baseada em propriedades e arrastar-e-soltar.

---

## 🏗️ Arquitetura Técnica (Padrão Builder & Factory)

Para lidar com o acoplamento do código original, o Studio adotará o padrão **Builder** para construir a interface visual. Isso permite separar a _criação_ dos widgets da _lógica_ de execução do Showbox.

### Componentes Principais

```plaintext
┌─────────────────────────────────────────────────────────────────────┐
│                         SHOWBOX STUDIO (Qt6)                        │
├───────────┬─────────────────────────────────┬───────────────────────┤
│           │                                 │                       │
│  TOOLBOX  │           CANVAS                │   PROPERTY EDITOR     │
│ (Palette) │        (QScrollArea)            │ (QtTreePropertyBrowser│
│           │                                 │    ou customizado)    │
│ [Widgets] │  ┌─────────────────────────┐    │                       │
│ [Layouts] │  │     StudioBuilder       │    │ [Propriedades]        │
│           │  │  (Gera widgets inertes  │    │ Name: btn1            │
│           │  │   com EventFilter)      │    │ Label: OK             │
│           │  └─────────────────────────┘    │ Stylesheet: ...       │
│           │                                 │                       │
├───────────┴─────────────────────────────────┴───────────────────────┤
│                       OBJECT HIERARCHY                              │
│                 (QTreeWidget - Estrutura DOM)                       │
├─────────────────────────────────────────────────────────────────────┤
│  SOURCE EDITOR (QSyntaxHighlighter) │ SIGNAL/SLOT EDITOR            │
└─────────────────────────────────────────────────────────────────────┘
```

---

## 📋 Estratégia de Reutilização de Código

### 1. Inclusão Direta ("White-box")

Arquivos do Showbox que serão compilados diretamente no Studio para garantir fidelidade visual 100%:

- `src/code/showbox/custom_chart_widget.cpp/h` (Gráficos)
- `src/code/showbox/icon_helper.cpp/h` (Carregamento de ícones)
- Qualquer outro widget customizado futuro.

### 2. Adaptação ("Wrapper")

Para widgets padrão (QPushButton, QCheckBox), criaremos wrappers ou factories no Studio que configuram o estilo visual para igualar o Showbox, mas sem conectar os slots de lógica de negócio (`clicked` -> `exec_command`).

---

## 📝 Etapas de Implementação

### Fase 1: Fundação e Infraestrutura (Prioridade Máxima)

#### 1.1 Configuração do Projeto (CMake/QMake)

- Criar `showbox-studio.pro` ou `CMakeLists.txt`.
- Configurar include paths para apontar para `../showbox/src/code/showbox`.
- Compilar um "Hello World" que instancia um `CustomChartWidget` do Showbox dentro de uma janela vazia.

#### 1.2 Widget Factory & DOM

- Criar uma classe base `StudioItem` que armazena as propriedades (nome, título, etc.) independente do widget Qt.
- Implementar `WidgetFactory` que cria o widget Qt correspondente e instala um **EventFilter**.
- **EventFilter Crítico:** Deve interceptar `MouseButtonPress` para impedir que o botão "clique" e, em vez disso, selecione o item no editor.

#### 1.3 Canvas Básico

- Implementar `CanvasWidget` (baseado em `QWidget` ou `QScrollArea`).
- Implementar sistema básico de layout (simulando a grade/fluxo do Showbox).

### Fase 2: Editores Principais

#### 2.1 Property Browser

- Implementar painel lateral que reflete as propriedades do item selecionado.
- Campos necessários: String, Bool, Enum (Combobox), Color, Font.
- **Sincronia:** Alterar valor no painel -> Atualizar widget no Canvas imediatamente.

#### 2.2 Toolbox (Paleta)

- Lista de widgets disponíveis (botão, label, tabela, gráfico, etc.).
- Implementar Drag & Drop da paleta para o Canvas.

#### 2.3 Geração de Código (Export)

- Classe `ScriptGenerator`.
- Percorrer a árvore de objetos (`StudioItem` tree) e gerar os comandos `add`, `set`, `end`.
- Suportar a sintaxe Here-Doc (`<< EOD`) usada pelo Showbox.

### Fase 3: Recursos Avançados

#### 3.1 Preview Real (Run)

- Botão "Testar".
- Gera o script temporário e executa o binário real do `showbox` em um subprocesso (`QProcess`).
- Captura `stdout` para mostrar logs no Studio.

#### 3.2 Importação (Parser Reverso)

- Capacidade de ler um script `.sh` existente contendo comandos Showbox.
- "Parsear" os comandos `add/set` e reconstruir a árvore visual.
- _Nota:_ Focar apenas no bloco Showbox, ignorar lógica shell complexa ao redor.

---

## ⚠️ Regras de Ouro (C++ Specific)

1. **Não modifique o Showbox (ainda):** O Studio deve se adaptar ao código existente do Showbox. Se precisar mudar algo no Showbox para facilitar o Studio, discuta antes.
2. **Gerenciamento de Memória:** O sistema de parentesco do Qt (`QObject::setParent`) deve ser respeitado estritamente para evitar leaks no Canvas.
3. **Separação Modelo-Visualização:** Mantenha os dados do projeto (quais widgets existem, suas propriedades) separados dos widgets Qt reais. Se o Canvas for destruído, os dados do projeto devem persistir.

---

## 📁 Estrutura de Pastas Sugerida

```plaintext
showbox-studio/
├── CMakeLists.txt (ou .pro)
├── src/
│   ├── main.cpp
│   ├── core/
│   │   ├── ProjectModel.h/cpp      # Dados do projeto
│   │   ├── ScriptGenerator.h/cpp   # Gerador de shell script
│   │   └── WidgetFactory.h/cpp     # Cria widgets Qt
│   ├── gui/
│   │   ├── MainWindow.h/cpp
│   │   ├── Canvas.h/cpp            # Área de desenho
│   │   ├── PropertyPanel.h/cpp     # Editor de propriedades
│   │   └── Toolbox.h/cpp           # Paleta de widgets
│   └── utils/
│       └── ShowboxBridge.h         # Funções para chamar o binário original
└── resources/
    └── icons/
```

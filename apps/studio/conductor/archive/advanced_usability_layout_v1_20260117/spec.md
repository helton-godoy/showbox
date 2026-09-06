# Specification: Advanced Usability & Layouts

## 1. Overview

Esta trilha foca em transformar a experiência de edição do Showbox Studio, permitindo manipulação complexa da hierarquia (layouts) e segurança na edição (Undo/Redo).

## 2. Undo/Redo System

Implementação do padrão `Command` usando `QUndoStack` do Qt.

### 2.1. Commands

Cada ação modificadora deve ser encapsulada em uma classe derivada de `QUndoCommand`:

- `AddWidgetCommand`: Cria e adiciona um widget. Undo: Remove (mas não deleta a memória se for redoável, ou serializa estado).
- `DeleteWidgetCommand`: Remove do pai. Undo: Readiciona na mesma posição/index.
- `PropertyChangeCommand`: Altera uma propriedade. Undo: Restaura valor antigo.
- `MoveWidgetCommand`: Reordena ou reparenta um widget.

### 2.2. Integration

- `StudioController` deve possuir o `QUndoStack`.
- Ações na UI (Toolbar/Menu) conectadas a `stack->undo()` e `stack->redo()`.
- Atalhos: `Ctrl+Z`, `Ctrl+Shift+Z` (ou `Ctrl+Y`).

## 3. Hierarchy Management (Reordering)

Permitir alterar a ordem dos widgets e sua hierarquia.

### 3.1. Tree View Drag & Drop

- Habilitar `InternalMove` no `ObjectInspector`.
- Validar "drops":
  - Não permitir soltar um pai dentro de seu filho.
  - Atualizar o `Canvas` (reparentar o widget Qt real) quando a árvore mudar.

### 3.2. Canvas Drag & Drop (Reordering)

- Permitir arrastar um widget já existente para outra posição no Canvas.
- Visual feedback: Linha indicadora de inserção.

## 4. Layout Grouping

O Showbox V1 cria layouts através de contêineres (`Frame`, `GroupBox`) com propriedades de layout (`HBox`, `VBox`).

### 4.1. Context Menu Action: "Group Into..."

- Selecionar múltiplos widgets.
- Right-click -> "Group into Horizontal Frame" / "Group into Vertical GroupBox".
- **Ação:**
  1.  Criar o contêiner pai.
  2.  Mover os widgets selecionados para dentro dele.
  3.  Preservar a ordem visual original.

### 4.2. Layout Properties

- Expor propriedades de layout no `PropertyEditor` para contêineres:
  - `Layout Type`: VBox, HBox.
  - `Spacing`, `Margin` (se suportado pelo ShowboxBuilder).

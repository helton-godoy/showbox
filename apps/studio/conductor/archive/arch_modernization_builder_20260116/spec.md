# Specification: Showbox Architecture Modernization (Builder & Protocol)

## 1. Overview

Esta track foca na modernização radical da arquitetura do Showbox Core para viabilizar o Showbox Studio e melhorar a experiência de desenvolvimento com Shell Scripts. O objetivo central é desacoplar a criação de UI da lógica de parsing legado, implementando um padrão **Builder** robusto e preparando o terreno para um protocolo de comunicação estruturado (baseado em JSON/Objetos), abandonando a retrocompatibilidade estrita em favor de robustez e performance.

## 2. Functional Requirements

### 2.1. Core Architecture (The Builder)

- **Interface `IShowboxBuilder`:** Definir uma interface clara e exaustiva em `libs/showbox-ui` para a criação de todos os widgets suportados.
- **Implementação `ShowboxBuilder`:** Implementar a classe concreta que instancia widgets Qt6 nativos configurados para o estilo Showbox.
- **Desacoplamento:** O Builder NÃO deve depender de nenhuma lógica de parsing de texto antiga. Ele deve receber parâmetros tipados (structs, enums, tipos primitivos).

### 2.2. Widget Migration (Full Scope)

- Migrar/Reimplementar a criação de todos os widgets existentes para usar o novo Builder:
  - **Containers:** Window, VBox, HBox, GroupBox, TabWidget.
  - **Controls:** PushButton, CheckBox, RadioButton, LineEdit, TextEdit, ComboBox, Slider, SpinBox.
  - **Displays:** Label, ProgressBar, Table, List, Tree.
  - **Visuals:** Chart, Image/Icon.

### 2.3. Protocol & Data Structures

- Definir estruturas de dados (C++ structs/classes) que representem as propriedades de cada widget.
- Estas estruturas servirão como "Contrato" entre o Parser (seja JSON ou Texto) e o Builder.

## 3. Non-Functional Requirements

- **Performance:** A instanciação via Builder deve ser tão ou mais rápida que o método inline anterior.
- **Extensibilidade:** Deve ser trivial adicionar um novo widget no futuro sem alterar a interface principal do parser.
- **Testabilidade:** O Builder deve ser testável unitariamente sem necessidade de simular stdin/stdout.

## 4. Acceptance Criteria

- [ ] Interface `IShowboxBuilder` definida e cobrindo 100% dos widgets atuais.
- [ ] Testes unitários comprovando que o Builder instancia widgets corretamente com suas propriedades.
- [ ] Prova de Conceito (PoC): Um executável simples que usa o Builder manualmente (hardcoded) para gerar uma UI complexa, demonstrando que o parser de texto é opcional.

## 5. Out of Scope

- A implementação completa do parser JSON final (será uma track subsequente, mas o Builder já deve estar pronto para ele).
- A interface gráfica do Showbox Studio (o foco aqui é a library que o Studio vai usar).

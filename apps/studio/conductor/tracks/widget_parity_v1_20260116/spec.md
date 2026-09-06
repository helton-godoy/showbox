# Specification: Implementation of Remaining Widgets (V1 Parity)

## 1. Overview

Esta track visa alcançar 100% de paridade de recursos com o Showbox V1/Legacy, implementando os widgets e contêineres remanescentes no novo sistema `ShowboxBuilder`. O foco é garantir que a criação desses componentes seja declarativa e simples, facilitando a futura integração com JSON/Shell Scripts.

## 2. Functional Requirements

### 2.1. Container Widgets

Implementar a construção dos seguintes contêineres, suportando aninhamento via configuração (Construção em Bloco):

- **TabWidget (Abas):** Deve aceitar uma lista de `PageConfig` (Título + Filhos).
- **GroupBox:** Contêiner com título e borda, aceitando layout e filhos.
- **Frame:** Contêiner visual simples (sem título), útil para agrupamento visual.

### 2.2. Selection & Input Widgets

Implementar controles de entrada com propriedades de validação e formatação:

- **CheckBox:** Booleano (Texto + Estado).
- **RadioButton:** Seleção exclusiva (Texto + Estado + Grupo).
- **SpinBox:** Entrada numérica (Min, Max, Passo, Prefixo/Sufixo).
- **Slider:** Entrada numérica deslizante (Min, Max, Orientação).
- **Calendar:** Seleção de data.
- **TextEdit/TextView:** Edição/Exibição de texto multilinha (Rich Text ou Plain).

### 2.3. Utility Widgets

- **Separator:** Linha visual horizontal ou vertical.

### 2.4. Data Structures (WidgetConfigs.h)

Expandir `WidgetConfigs.h` com as novas structs:

- `TabWidgetConfig`, `PageConfig`, `GroupBoxConfig`, `FrameConfig`.
- `CheckBoxConfig`, `RadioButtonConfig`.
- `SpinBoxConfig`, `SliderConfig`, `CalendarConfig`, `TextEditConfig`.
- `SeparatorConfig`.

## 3. Non-Functional Requirements

- **Simplicidade Declarativa:** A configuração de contêineres complexos (Abas) deve ser auto-contida, evitando gestão de estado complexa para o usuário.
- **Consistência:** Seguir os padrões de `BaseConfig` já estabelecidos.

## 4. Acceptance Criteria

- [ ] Todas as structs de configuração implementadas em `WidgetConfigs.h`.
- [ ] Métodos de construção (`buildTabWidget`, `buildCheckBox`, etc.) adicionados a `IShowboxBuilder` e `ShowboxBuilder`.
- [ ] Testes unitários cobrindo a criação e propriedades de cada novo widget.
- [ ] Teste de integração demonstrando uma estrutura complexa (ex: Abas contendo inputs variados) construída em uma única chamada declarativa (simulada).

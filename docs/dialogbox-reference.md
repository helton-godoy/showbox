# Referência do Dialogbox

> Documentação técnica das funcionalidades do Dialogbox versão 1.0 para referência e validação do Showbox (Qt6).

## Sumário

1. [Visão Geral](#visão-geral)
2. [Comandos Suportados](#comandos-suportados)
3. [Widgets Suportados](#widgets-suportados)
4. [Opções e Propriedades](#opções-e-propriedades)
5. [Formato de Saída](#formato-de-saída)
6. [Tokens e Enums](#tokens-e-enums)
7. [Arquitetura](#arquitetura)

---

## Visão Geral

O Dialogbox é uma ferramenta CLI para criação de interfaces gráficas em scripts shell. Ele recebe comandos via stdin e gera widgets Qt, reportando interações do usuário para stdout.

### Características Principais

- **Parser Thread:** Thread separada para parsing de comandos
- **Thread-safe:** Usa `Qt::BlockingQueuedConnection` para garantir execução sequencial
- **Formato de tokens:** Suporta aspas, escape com backslash
- **Extensível:** Sistema de comandos e widgets modular

---

## Comandos Suportados

| Comando    | Descrição                  | Estágios                                           |
| ---------- | -------------------------- | -------------------------------------------------- |
| `add`      | Adiciona widget ao diálogo | Type, Title, Name, Options, Text, AuxText, Command |
| `end`      | Finaliza container atual   | Type, Command                                      |
| `position` | Posiciona widget           | Options, Text, Command                             |
| `remove`   | Remove widget pelo nome    | Name, Command                                      |
| `clear`    | Limpa widget ou diálogo    | Name, Command                                      |
| `step`     | Move para próximo step     | Options, Command                                   |
| `set`      | Define opções/propriedades | Name, Options, Text, Command                       |
| `unset`    | Remove opções/propriedades | Name, Options, Command                             |
| `enable`   | Habilita widget            | Name, Command                                      |
| `disable`  | Desabilita widget          | Name, Command                                      |
| `show`     | Exibe widget               | Name, Command                                      |
| `hide`     | Oculta widget              | Name, Command                                      |
| `query`    | Reporta todos os widgets   | Command                                            |

### Detalhes dos Comandos

#### `add type [title] [name] [options] [text] [auxtext]`

Adiciona um novo widget ao diálogo.

```bash
# Exemplo: Adicionar botão
add pushbutton "Clique aqui" botao_ok default exit "OK"
```

#### `end [type]`

Finaliza containers (groupbox, frame, listbox, tabs, page).

```bash
# Exemplo: Finalizar groupbox
end groupbox
```

#### `set [name] options [text]`

Define propriedades de um widget ou do diálogo.

```bash
# Exemplo: Definir título
set main title "Novo Título"

# Exemplo: Definir texto em textbox
set textbox1 text "Conteúdo"
```

#### `unset [name] options`

Remove propriedades.

```bash
unset textbox1 readonly
```

---

## Widgets Suportados

### Widgets Padrão

| Widget         | Tipo              | Descrição                             |
| -------------- | ----------------- | ------------------------------------- |
| `label`        | LabelWidget       | Rótulo de texto, imagem ou animação   |
| `pushbutton`   | PushButtonWidget  | Botão de pressão                      |
| `checkbox`     | CheckBoxWidget    | Caixa de seleção                      |
| `radiobutton`  | RadioButtonWidget | Botão de rádio                        |
| `textbox`      | TextBoxWidget     | Caixa de texto (QLineEdit)            |
| `textview`     | TextViewWidget    | Área de texto multi-linha (QTextEdit) |
| `listbox`      | ListBoxWidget     | Lista de itens (QListWidget)          |
| `dropdownlist` | ComboBoxWidget    | Lista suspensa não-editável           |
| `combobox`     | ComboBoxWidget    | Lista suspensa editável               |
| `progressbar`  | ProgressBarWidget | Barra de progresso                    |
| `slider`       | SliderWidget      | Controle deslizante                   |
| `separator`    | SeparatorWidget   | Separador visual                      |
| `space`        | OptionSpace       | Espaço vertical                       |
| `stretch`      | OptionStretch     | Expansor de espaço                    |
| `frame`        | FrameWidget       | Frame com borda                       |
| `groupbox`     | GroupBoxWidget    | Grupo com título                      |
| `tabs`         | TabsWidget        | Abas                                  |
| `page`         | PageWidget        | Página em abas                        |
| `item`         | ItemWidget        | Item de lista                         |

### Widgets Estendidos

| Widget     | Tipo           | Descrição                           |
| ---------- | -------------- | ----------------------------------- |
| `table`    | TableWidget    | Tabela editável (CustomTableWidget) |
| `chart`    | ChartWidget    | Gráfico (CustomChartWidget)         |
| `calendar` | CalendarWidget | Calendário (QCalendarWidget)        |

---

## Opções e Propriedades

### Opções de Comando

| Opção        | Código           | Descrição                        |
| ------------ | ---------------- | -------------------------------- |
| `enabled`    | OptionEnabled    | Habilita widget                  |
| `focus`      | OptionFocus      | Foca no widget                   |
| `stylesheet` | OptionStyleSheet | Aplica stylesheet                |
| `visible`    | OptionVisible    | Exibe widget                     |
| `horizontal` | OptionVertical   | Orientation horizontal (inverte) |
| `vertical`   | OptionVertical   | Orientation vertical             |
| `behind`     | OptionBehind     | Posiciona atrás                  |
| `onto`       | OptionOnto       | Posiciona sobre                  |
| `space`      | OptionSpace      | Adiciona espaço                  |
| `stretch`    | OptionStretch    | Adiciona stretch                 |

### Propriedades de Widget

| Propriedade   | Código                 | Widgets                 | Descrição               |
| ------------- | ---------------------- | ----------------------- | ----------------------- |
| `checkable`   | PropertyCheckable      | GroupBox, Button        | Permite seleção         |
| `checked`     | PropertyChecked        | Button, CheckBox, Radio | Estado marcado          |
| `text`        | PropertyText           | LineEdit                | Texto do campo          |
| `title`       | PropertyTitle          | Label, GroupBox, Page   | Título/texto            |
| `password`    | PropertyPassword       | TextBox                 | Modo senha              |
| `placeholder` | PropertyPlaceholder    | TextBox                 | Texto placeholder       |
| `icon`        | PropertyIcon           | Button, Page, Item      | Ícone                   |
| `iconsize`    | PropertyIconSize       | Widgets com ícone       | Tamanho do ícone        |
| `animation`   | PropertyAnimation      | Label                   | Animação GIF            |
| `picture`     | PropertyPicture        | Label                   | Imagem                  |
| `apply`       | PropertyApply          | PushButton              | Gera report ao clicar   |
| `exit`        | PropertyExit           | PushButton              | Fecha diálogo ao clicar |
| `default`     | PropertyDefault        | PushButton              | Botão padrão            |
| `default`     | PropertyDefault        | Widget                  | Define foco padrão      |
| `plain`       | PropertyPlain          | Frame                   | Borda plain             |
| `raised`      | PropertyRaised         | Frame                   | Borda raised            |
| `sunken`      | PropertySunken         | Frame                   | Borda sunken            |
| `noframe`     | PropertyNoframe        | Frame                   | Sem borda               |
| `box`         | PropertyBox            | Frame                   | Borda box               |
| `panel`       | PropertyPanel          | Frame                   | Borda panel             |
| `styled`      | PropertyStyled         | Frame                   | Borda styled panel      |
| `current`     | PropertyCurrent        | Item, Page              | Item/página atual       |
| `activation`  | PropertyActivation     | ListBox                 | Ativação de itens       |
| `selection`   | PropertySelection      | ListBox, ComboBox       | Seleção de itens        |
| `minimum`     | PropertyMinimum        | Slider, Calendar        | Valor mínimo            |
| `maximum`     | PropertyMaximum        | Slider, Calendar        | Valor máximo            |
| `value`       | PropertyValue          | Slider, ProgressBar     | Valor atual             |
| `busy`        | PropertyBusy           | ProgressBar             | Modo busy (0-100)       |
| `file`        | PropertyFile           | TextView, Table, Chart  | Carrega de arquivo      |
| `top`         | PropertyPositionTop    | Tabs                    | Abas no topo            |
| `bottom`      | PropertyPositionBottom | Tabs                    | Abas embaixo            |
| `left`        | PropertyPositionLeft   | Tabs                    | Abas à esquerda         |
| `right`       | PropertyPositionRight  | Tabs                    | Abas à direita          |
| `readonly`    | PropertyReadOnly       | Table                   | Apenas leitura          |
| `headers`     | PropertyHeaders        | Table                   | Headers de coluna       |
| `add_line`    | PropertyAddLine        | Table                   | Adiciona linha          |
| `del_line`    | PropertyDelLine        | Table                   | Remove linha            |
| `search`      | PropertySearch         | Table                   | Mostra busca            |
| `append`      | PropertyAppend         | Chart                   | Append dados            |
| `axis`        | PropertyAxis           | Chart                   | Define eixo             |
| `export`      | PropertyExport         | Chart                   | Exporta gráfico         |
| `date`        | PropertyDate           | Calendar                | Define data             |
| `navigation`  | PropertyNavigation     | Calendar                | Mostra navegação        |

---

## Formato de Saída

### Pushbutton Click

```
<pushbutton name>=clicked
```

### Toggle Pushbutton

```
<pushbutton name>=pressed
<pushbutton name>=released
```

### Slider

```
<slider name>=<value>
```

### ListBox/ComboBox Selection

```
<list widget name>=<value>
```

### ListBox Activation

```
<list widget name>=<value>
```

### Table Cell Edit

```
<table name>[<row>][<col>]=<value>
```

### Chart Slice Click

```
<chart name>.slice["<label>"]=<value>
```

### Calendar Selection

```
<calendar name>=<date>
```

### Query Command

```
<name>=<value>
<name>=<value>
...
```

### Checkbox State

```
<checkbox name>=1
<checkbox name>=0
```

---

## Tokens e Enums

### DialogCommandTokens Namespace

```cpp
namespace DialogCommandTokens {

// Command codes
enum Command {
    NoopCommand = 0,
    AddCommand,
    EndCommand,
    PositionCommand,
    RemoveCommand,
    ClearCommand,
    StepCommand,
    SetCommand,
    UnsetCommand,
    QueryCommand
};

// Control type (widget type)
enum Control {
    WidgetMask = 0xFF00,
    CheckBoxWidget = 0x0100,
    FrameWidget = 0x0200,
    GroupBoxWidget = 0x0400,
    LabelWidget = 0x0800,
    PushButtonWidget = 0x1000,
    RadioButtonWidget = 0x2000,
    SeparatorWidget = 0x4000,
    TextBoxWidget = 0x0108,
    ListBoxWidget = 0x0110,
    ComboBoxWidget = 0x0120,
    ItemWidget = 0x0140,
    ProgressBarWidget = 0x0180,
    SliderWidget = 0x0208,
    TextViewWidget = 0x0408,
    TabsWidget = 0x0800,
    PageWidget = 0x1000,
    TableWidget = 0x2000,
    ChartWidget = 0x4000,
    CalendarWidget = 0x8000
};

// Property mask (applied to control)
enum Property {
    PropertyMask = 0xFF,
    PropertyCheckable = 0x01,
    PropertyChecked = 0x02,
    PropertyText = 0x04,
    PropertyTitle = 0x08,
    PropertyPassword = 0x10,
    PropertyPlaceholder = 0x20,
    PropertyIcon = 0x40,
    PropertyIconSize = 0x80
};

// Option mask (applied to command)
enum Option {
    OptionMask = 0xFF,
    OptionSpace = 0x01,
    OptionStretch = 0x02,
    OptionBehind = 0x04,
    OptionOnto = 0x08,
    OptionEnabled = 0x10,
    OptionFocus = 0x20,
    OptionStyleSheet = 0x40,
    OptionVisible = 0x80
};

// Stages for parsing
enum Stage {
    StageCommand = 0x01,
    StageType = 0x02,
    StageTitle = 0x04,
    StageName = 0x08,
    StageOptions = 0x10,
    StageText = 0x20,
    StageAuxText = 0x40
};

} // namespace DialogCommandTokens
```

---

## Arquitetura

### Componentes Principais

```
┌─────────────────────────────────────────────────────────────┐
│                      DialogParser                           │
│  - run()           : Thread principal de parsing           │
│  - processToken()  : Analisa tokens e monta comandos       │
│  - issueCommand()  : Emite comando para execução           │
└────────────────┬────────────────────────────────────────────┘
                 │
                 │ emit sendCommand(DialogCommand)
                 ▼
┌─────────────────────────────────────────────────────────────┐
│                        ShowBox                              │
│  executeCommand() : Executa comandos do parser             │
│  - add*()         : Métodos de criação de widgets          │
│  - setOptions()   : Configura propriedades                 │
│  - slots          : handlers de eventos                    │
└────────────────┬────────────────────────────────────────────┘
                 │
                 │ Event signals
                 ▼
┌─────────────────────────────────────────────────────────────┐
│                     Widgets Qt                              │
│  - QLabel, QPushButton, QCheckBox                          │
│  - QLineEdit, QTextEdit, QListWidget                       │
│  - QComboBox, QProgressBar, QSlider                        │
│  - QTabWidget, QCalendarWidget                             │
│  - CustomTableWidget, CustomChartWidget                    │
└─────────────────────────────────────────────────────────────┘
```

### Fluxo de Parsing

1. **Leitura:** Thread lê stdin caractere por caractere
2. **Tokenização:** Reconhece tokens (palavras, aspas, escapes)
3. **Processamento:** `processToken()` identifica tipo de token
4. **Montagem:** Constrói `DialogCommand` com estágios
5. **Emissão:** `issueCommand()` envia para thread principal
6. **Execução:** `executeCommand()` cria/configura widgets
7. **Report:** Slots capturam eventos e escrevem em stdout

### DialogCommand Structure

```cpp
struct DialogCommand {
    unsigned int command;  // Código do comando + opções
    unsigned int control;  // Tipo do widget + propriedades
    const char *title;     // Ponteiro para título no buffer
    const char *name;      // Ponteiro para nome no buffer
    const char *text;      // Ponteiro para texto no buffer
    const char *auxtext;   // Ponteiro para texto auxiliar

    const char *getTitle() const { return title == BUFFER_SIZE - 1 ? nullptr : buffer + title; }
    // ... métodos similares para name, text, auxtext
};
```

---

## Exemplo de Uso

```bash
#!/bin/bash
# Exemplo de diálogo com showbox

# Criar diálogo principal
add groupbox "Configurações" main
    # Adicionar checkbox
    add checkbox "Opção 1" opt1 checked
    add checkbox "Opção 2" opt2

    # Adicionar botão
    add pushbutton "Salvar" btn_save default exit apply

end groupbox

# Loop de eventos
echo "Dialog started"
```

---

## Notas de Implementação

### Diferenças Qt5 vs Qt6

- O código atual é compatível com Qt5
- Showbox deve ser portado para Qt6:
  - Atualizar includes: `QtCharts/QPieSeries` → `QtCharts`
  - Verificar deprecated APIs
  - Atualizar conectores de sinais

### Custom Widgets

- **CustomTableWidget:** Wrapper para QTableWidget com funcionalidades extras
- **CustomChartWidget:** Wrapper para QChartView com gerenciamento de dados
- Ambos usam QtCharts para visualização

### Thread Safety

- `Qt::BlockingQueuedConnection` garante que comandos sejam executados sequencialmente
- Evita race conditions em sequências show/hide/show
- Parser thread bloqueia até execução completar

---

_Documentação gerada para referência do projeto Showbox (Qt6)._

# Showbox Studio

O **Showbox Studio** é uma IDE visual para o ecossistema Showbox, focada em empoderar profissionais de TI a criar interfaces gráficas ricas através de uma experiência drag-and-drop.

## Arquitetura Modular

O Studio é construído sobre a biblioteca **showbox-ui** (Core), garantindo que os widgets visualizados no editor sejam idênticos aos renderizados pelo CLI do Showbox.

### Padrão de Integração de Widgets

Para adicionar um novo widget ao ecossistema e torná-lo disponível no Studio:

1. **Core (showbox-ui):**
   - Implemente o widget em `libs/showbox-ui/src`

   - Registre o widget na `WidgetFactory` em `libs/showbox-ui/src/widget_factory.cpp`.

```cpp
    reg["meu_widget"] = []() { return new MeuWidget(); };
```

1. **Studio (showbox-studio):**
   - Adicione o método de criação na interface `IStudioWidgetFactory.h`.
   - Implemente o método em `StudioWidgetFactory.h`, delegando para a Core Factory. - Adicione o item correspondente na Toolbox (`MainWindow::setupUI`).

## Desenvolvimento com Docker

É obrigatório o uso do ambiente de desenvolvimento isolado para garantir consistência:

```bash
./src/start-dev.sh
```

Dentro do container, utilize o CMake para build:

```bash
cmake -S . -B build && cmake --build build
```

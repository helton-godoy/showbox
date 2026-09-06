# Diretrizes do Produto: Showbox Suite

## 1. Comunicação e Tom de Voz

A comunicação do Showbox Suite deve equilibrar precisão técnica com acessibilidade, visando empoderar o usuário não-programador.

- **Técnico, mas Acessível:** Evite jargões excessivos de C++/Qt (como "QObject", "Slots", "Virtual Functions") na interface do usuário final. Use termos funcionais (ex: "Ação ao Clicar" em vez de "clicked signal").
- **Inspirador e Empoderador:** Reforce a mensagem de que o usuário está criando softwares poderosos. Use verbos de ação e mensagens de sucesso que validem o progresso (ex: "Script gerado com sucesso! Sua interface está pronta.").
- **Bilinguismo e Localização:**
  - **Idiomas:** Suporte de primeira classe para **Português do Brasil (pt-BR)** e **Inglês (en)**.
  - **Nomenclatura:** Nomes internos de componentes podem permanecer em inglês (padrão de código), mas a interface e documentação devem ser totalmente localizadas.
  - **Clareza Semântica:** A descrição de componentes deve focar no _problema_ e na _solução_, não na implementação.

## 2. Padrão de Documentação de Componentes

Todo componente ou widget exposto pelo Showbox deve seguir rigorosamente a seguinte estrutura de documentação para garantir a usabilidade por profissionais de operações:

- **Cabeçalho:** Nome do Componente (en) / Nome Exibido (Localizado).
- **Descrição Funcional:** O que ele faz (1 parágrafo).
- **Problema que Resolve:** Por que usar este componente?
- **Casos de Uso:**
  - _Recomendado:_ Onde usar (ex: "Formulários de login").
  - _Evitar:_ Onde não usar (ex: "Processamento de pagamentos complexos").
- **Interface Pública (Tabela de Props):**
  - `Nome`: (ex: `title`, `isDisabled`).
  - `Tipo`: (ex: Texto, Booleano/Sim-Não, Lista).
  - `Obrigatoriedade`: Sim/Não.
  - `Descrição`: Explicação do impacto da propriedade no widget.
- **Eventos:** Ações que o widget dispara (ex: `onChange` -> "Quando o valor muda").

## 3. Identidade Visual e UX (Showbox-Studio)

A interface deve ser flexível e adaptável ao ambiente do usuário, suportada por um sistema robusto de **Temas**.

- **Tema Padrão: Modern Dark**
  - **Estilo:** Alto contraste, cores escuras (inspirado em VS Code/Terminais), acentos de cor vibrantes para ações.
  - **Objetivo:** Foco, produtividade e redução de cansaço visual.
- **Tema Alternativo: Nativo do SO**
  - **Estilo:** Segue estritamente os padrões de renderização do sistema operacional (GTK/KDE no Linux, WinAPI no Windows, Cocoa no macOS).
  - **Objetivo:** Integração visual perfeita com o desktop do usuário, parecendo uma ferramenta nativa do sistema.
- **Tema Industrial/Utilitário**
  - **Estilo:** Alta densidade de informação, controles compactos, menos espaçamento (padding), linhas guias visíveis.
  - **Objetivo:** Maximizar a área útil para projetos complexos ou dashboards densos (estilo CAD/Engenharia).

## 4. Organização e Estrutura do Projeto

- **Raiz Limpa:** A raiz do projeto deve ser mantida organizada e livre de poluição.
- **Localização de Artefatos:** Cada artefato deve ser salvo em uma área apropriadamente definida.
  - **Justificativa:** A escolha da localização deve ter uma fundamentação clara que justifique a decisão.
  - **Objetivo:** Facilitar a localização, promover escolhas coerentes e criar uma arquitetura intuitiva.
- **Ambientes Isolados:** Ambientes de desenvolvimento e empacotamento devem ser segregados (Docker) para evitar contaminação do host e garantir reprodutibilidade.

## 5. Princípios de Design de Código

- **Abstração:** O código C++ deve ser complexo internamente para permitir simplicidade externamente (na interface e no shell script).
- **Modularidade:** Componentes devem ser independentes. A refatoração do Core para CMake visa garantir que o Studio possa importar _apenas_ o necessário.
- **Passive View:** Widgets nunca devem executar lógica de shell script internamente. Eles devem emitir sinais Qt puros, delegando a execução para a camada de Controle/Builder.
- **Padrão de Referência:** Implementações novas devem seguir estritamente o "Golden Sample" (atualmente o PushButton) para garantir consistência arquitetural e facilidade de manutenção por agentes de IA.

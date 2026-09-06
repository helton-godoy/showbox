# Specification: Visual Refinement & Asset Management

## 1. Overview

Esta trilha visa melhorar a estética e a experiência de customização visual no Showbox Studio, introduzindo editores especializados para propriedades complexas (cores, ícones, fontes).

## 2. Specialized Property Editors

O `PropertyEditor` deve deixar de ser puramente textual para usar componentes específicos do Qt baseados no tipo da propriedade.

### 2.1. Color Editor

- Identificar propriedades que contenham "color" ou "background".
- Exibir um pequeno "Color Swatch" (quadrado colorido) ao lado do valor hexadecimal.
- Ao clicar, abrir o `QColorDialog`.
- Suportar Undo/Redo via `PropertyChangeCommand`.

### 2.2. Icon Selector

- O Showbox utiliza ícones SVG internos (via `IconHelper` na lib `showbox-ui`).
- Para a propriedade `icon`, exibir um botão que abre uma galeria (Grid View).
- A galeria deve listar todos os ícones disponíveis no sistema de recursos.

## 3. Font & Style Refinement

- Editor simples de fonte (Família, Tamanho, Negrito).
- Integração com `QFontDialog`.

## 4. Canvas Themes

- Permitir selecionar o "Tema Ativo" para o Canvas (ex: Dark, Light, Blue).
- Isso não afeta apenas o widget, mas injeta o StyleSheet do tema no Canvas para simular o Showbox CLI.

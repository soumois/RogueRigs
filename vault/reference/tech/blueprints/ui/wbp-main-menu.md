---
id: wbp-main-menu
description: "WBP_MainMenu: main menu widget with key-art background, left gradient, title and New Run, Options, Exit buttons; open to change the menu look or wiring."
parent: blueprints-ui-index
kind: doc
---
# WBP_MainMenu

- asset_path: /Game/RogueRigs/UI/WBP_MainMenu (texture /Game/RogueRigs/UI/T_MainMenuBackground, 1672x940, TEXTUREGROUP_UI, from `vault/assets/main-menu-background.png`)
- parent_class: `URRMainMenuWidget` ([[sys-main-menu]])
- purpose: the main menu screen, created by the menu player controller from [[bp-main-menu-game-mode]].
- components: Root CanvasPanel > `BackgroundImage` (full anchor); `GradientRow` (HorizontalBox, left, 1000 wide) with `GradientBand0..4` (Border, colour 0.01/0.012/0.03, alpha 0.85/0.65/0.45/0.22/0.07); `MenuColumn` (VerticalBox, left offset 120, vertically centred) with `TitleText` "ROGUE RIGS" (Roboto Bold 80, white, outline 3), then `NewRunButton`, `OptionsButton`, `ExitButton`, each with a text (Roboto Bold 40, white, outline 2). Options is disabled, not focusable, grey 0.4. The stepped bands stand in for a smooth gradient (a gradient texture could replace them).
- exposed_defaults: desired focus widget `NewRunButton`; labels hardcoded in English ([[main-menu]]).
- events:
  - `OnClicked` New Run → `StartNewRun`; Exit → `QuitGame`.
  - `OnHovered` of New Run and Exit → `SetKeyboardFocus` on that button.
  - `EventConstruct` → `SetKeyboardFocus` NewRunButton, then `RefreshColours`.
  - `EventTick` → `RefreshColours`.
- logic_in_bp: function `RefreshColours` (New Run and Exit texts orange 1/0.35/0.1 when the button has keyboard focus, else white). Look only; run logic is C++.
- assets: T_MainMenuBackground; Roboto Bold font.
- owner_agent: agent-bp

## Relations
- [[blueprints-ui-index]] — parent index.
- [[sys-main-menu]] — the C++ base and behaviour.
- [[bp-main-menu-game-mode]] — the game mode that creates this widget.
- [[main-menu]] — the design.
- [[wbp-loading]] — font and colour reference.

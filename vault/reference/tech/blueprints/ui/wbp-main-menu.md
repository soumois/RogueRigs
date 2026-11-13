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
- components (Root CanvasPanel; top-level blocks are direct children):

| Widget | Anchors / slot | Notes |
|---|---|---|
| BackgroundImage (Image) | (0,0)-(1,1), offsets 0 | T_MainMenuBackground |
| GradientRow (HorizontalBox) | (0,0)-(0,1), offset right 1000 | `GradientBand0..4` (Border, colour 0.01/0.012/0.03, alpha 0.85/0.65/0.45/0.22/0.07), each Fill; stepped stand-in for a smooth gradient |
| MenuColumn (VerticalBox) | point (0,0.5), align (0,0.5), offset left 120, auto size | `TitleText` "ROGUE RIGS" (Roboto Bold 80, white, outline 3, bottom padding 40), then `ButtonColumn` |
| ButtonColumn (VerticalBox) | MenuColumn slot HAlign Left | `NewRunButton`, `OptionsButton`, `ExitButton`, slots HAlign Fill, padding 8 top and bottom: all three take the width of the widest (New Run) |

- plate buttons (task-487-01, 663c7c6a): style as [[wbp-pause-menu]] (T_Shop_ButtonContinue, Box, margin L/R 0.155, image 640x163; Pressed tint 0.7; Disabled tint 0.5 alpha 0.85 = greyed plate), Normal and Pressed padding 0, BackgroundColor white; size from content: each button holds `<Name>Content` (Overlay, fill) with `<Name>Text` (Roboto Bold 36, no outline, dark brown 0.05/0.04/0.03, centred, padding 125 left/right, 30 top/bottom; about 440 x 105 at 1080p). Options is enabled since task-510-08 (8ef36596): bIsEnabled true, bound by the C++ base, opens [[wbp-options]].
- focus look: none (task-540-01, 2f08ef00, owner 2026-11-13). The orange outline images `NewRunFocus` and `ExitFocus` (task-487) and the `RefreshColours` function were removed; the engine navigation focus rectangle is off project-wide (`RenderFocusRule=Never`, [[sys-main-menu]] data). Keyboard focus still exists but is invisible.
- exposed_defaults: desired focus widget `NewRunButton`; labels hardcoded in English ([[main-menu]]).
- events:
  - `OnClicked` New Run → `StartNewRun`; Exit → `QuitGame`.
  - `OnHovered` of New Run and Exit → `SetKeyboardFocus` on that button.
  - `EventConstruct` → `SetKeyboardFocus` NewRunButton.
  - No `EventTick` since task-540-01.
- logic_in_bp: none beyond the events above; run logic is C++.
- assets: T_MainMenuBackground; Roboto Bold font.
- owner_agent: agent-bp
- button_hover (task-485-09, then task-487-01): NewRunButton and ExitButton take the plate hover (C++ AttachNamed with plate brightening since task-487: brighter plate, grow 1.05); OptionsButton attached since task-510-04. Hovering a button also gives it keyboard focus (OnHovered), so Enter acts on the hovered button. See [[sys-button-hover]]; values of [[ui-button-hover]].
- verified (task-487-02): compiled with warnings as errors, saved; -game L_MainMenu at 1920 x 1080, 1280 x 720, 1920 x 1200, 2560 x 1080, at rest and New Run, Exit, Options hovered (Saved/Captures/task487_sheets/rest_sheet.png, hover_sheet.png). The column is taller than with text buttons, so the centred title sits about 60 px higher at 1080p. Not verified on screen: Enter and clicks (graph events unchanged).
- verified (task-540-02): -game L_MainMenu 1920 x 1080 with real key presses: no rectangle at open, after Down then Up, or on hover; Down from New Run lands on Exit (Options skipped, not investigated); Up then Enter loads L_ArenaM1. Sheet `Saved/Captures/task540_final/before_after.png`.

## Relations
- [[blueprints-ui-index]] — parent index.
- [[sys-main-menu]] — the C++ base and behaviour.
- [[bp-main-menu-game-mode]] — the game mode that creates this widget.
- [[main-menu]] — the design.
- [[wbp-loading]] — font and colour reference.
- [[sys-button-hover]] — shared mouse hover of its buttons.
- [[wbp-pause-menu]] — source of the plate button style.
- [[wbp-options]] — the options screen its Options button opens.

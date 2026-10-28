---
id: bp-main-menu-game-mode
description: "BP_MainMenuGameMode: Blueprint child of the C++ main menu game mode that names WBP_MainMenu; the game mode of L_MainMenu."
parent: blueprints-player-index
kind: doc
---
# BP_MainMenuGameMode

- asset_path: /Game/RogueRigs/Blueprints/Player/BP_MainMenuGameMode
- parent_class: `ARRMainMenuGameMode` ([[sys-main-menu]])
- purpose: carries the menu widget class so the C++ game mode stays asset-free; used by L_MainMenu World Settings (`DefaultGameMode`).
- components: none
- exposed_defaults: `MainMenuWidgetClass` = `WBP_MainMenu_C`; `DefaultPawnClass` None; `PlayerControllerClass` `ARRMainMenuPlayerController`.
- events: none
- logic_in_bp: none
- assets: [[wbp-main-menu]]
- owner_agent: agent-bp

## Relations
- [[blueprints-player-index]] — parent index.
- [[sys-main-menu]] — the C++ parent and the map set-up.
- [[wbp-main-menu]] — the widget it creates.
- [[bp-rr-game-mode]] — the arena game mode this one replaces on the menu map only.

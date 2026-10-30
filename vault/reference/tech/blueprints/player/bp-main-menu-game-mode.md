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
- exposed_defaults: `MainMenuWidgetClass` = `WBP_MainMenu_C`; `LoadingScreenWidgetClass` = `WBP_Loading_C` (task-379-02, commit 21349fa8; shown over the menu after New Run); `LoadingScreenWidgetZOrder` 10 (C++ default); `DefaultPawnClass` None; `PlayerControllerClass` `ARRMainMenuPlayerController`.
- events: none
- logic_in_bp: none
- assets: [[wbp-main-menu]], [[wbp-loading]]
- owner_agent: agent-bp

## Relations
- [[blueprints-player-index]] — parent index.
- [[sys-main-menu]] — the C++ parent and the map set-up.
- [[wbp-main-menu]] — the widget it creates.
- [[wbp-loading]] — the loading screen class it names for New Run.
- [[bp-rr-game-mode]] — the arena game mode this one replaces on the menu map only.

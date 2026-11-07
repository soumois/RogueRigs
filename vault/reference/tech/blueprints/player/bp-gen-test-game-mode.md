---
id: bp-gen-test-game-mode
description: "BP_GenTestGameMode and the L_GenTest map it runs: enemy-free generator test map, its defaults, map set-up and how to open it."
parent: blueprints-player-index
kind: doc
---
# BP_GenTestGameMode and L_GenTest

- asset_path: /Game/RogueRigs/Blueprints/Player/BP_GenTestGameMode; map /Game/RogueRigs/Maps/L_GenTest ([[task-494-02-test-map]]).
- parent_class: `ARRGenTestGameMode` (child of `ARRGameMode`, task-494-01).
- purpose: the dev test map where the owner plays the miner and drone on generated layouts, without robots, console, bonus chest, shuttle or alert bar, and asks for a new layout with `rr.Level.New [seed]`.
- components: none
- exposed_defaults: `DefaultPawnClass` BP_Miner_C, `PlayerControllerClass` BP_RRPlayerController_C (both as [[bp-rr-game-mode]]); `HiddenHUDWidgetNames` [`AlertObjectives`] (alert bar and objective panel of [[wbp-alert-objectives]]); C++ defaults kept: `StartSeed` 0 (random), `ParamsPath` empty (generator default params), `LayoutFilePrefix` `gen_test_`, `WatchLogSeconds` 30; `RunFlowConfig` left at the C++ defaults (the run flow is off).
- map: duplicate of L_ArenaM1 (lights, post process, space background, asteroid field, PlayerStart, BP_ArenaGrid at the same transform) with BP_EnemySpawner removed, the grid `LayoutName` emptied (the loader fills it) and World Settings `DefaultGameMode` = BP_GenTestGameMode_C. Not in `MapsToCook` (editor and `-game` only).
- open: in the editor, open L_GenTest and press Play; or `UnrealEditor.exe RogueRigs.uproject /Game/RogueRigs/Maps/L_GenTest -game -windowed`. In play, open the console (`~`) and type `rr.Level.New` (random seed) or `rr.Level.New 1234`.
- events: none
- logic_in_bp: none
- assets: none of its own; the map reuses the L_ArenaM1 actors listed above.
- owner_agent: agent-bp

## Relations
- [[blueprints-player-index]] — parent index.
- [[bp-rr-game-mode]] — the run game mode whose pawn and controller it reuses.
- [[wbp-alert-objectives]] — the HUD panel it hides.
- [[sys-layout-loader]] — the asynchronous loader that builds its grid.
- [[task-494-02-test-map]] — the task that made the map.

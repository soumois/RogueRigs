---
id: bp-rr-game-mode
description: "BP_RRGameMode, Blueprint of ARRGameMode setting BP_Miner and BP_RRPlayerController as defaults; project default and L_ArenaM1 override."
parent: blueprints-player-index
kind: doc
---
# BP_RRGameMode

- asset_path: /Game/RogueRigs/Blueprints/Player/BP_RRGameMode
- parent_class: ARRGameMode (/Script/RogueRigs.RRGameMode)
- purpose: run game mode shell; selects the miner and its controller ([[sys-player]]).
- components: none.
- exposed_defaults: DefaultPawnClass = BP_Miner_C; PlayerControllerClass = BP_RRPlayerController_C; RunFlowConfig.ConsoleClass = BP_ObjectiveConsole_C, RunFlowConfig.BonusChestClass = BP_BonusChest_C and RunFlowConfig.ShuttleClass = BP_ExtractionShuttle_C (other run flow values at the C++ defaults, [[sys-objectives]]). After editing it through MCP, compile the Blueprint, else a PIE reads the old value.
- events: none.
- logic_in_bp: none
- assets: none.
- owner_agent: agent-bp
- used_by: project GlobalDefaultGameMode (Config/DefaultEngine.ini, [/Script/EngineSettings.GameMapsSettings]); L_ArenaM1 World Settings game mode override.

## Relations
- [[blueprints-index]] — parent index.
- [[sys-player]] — the C++ class it derives from.
- [[bp-miner]] — default pawn.
- [[bp-rr-player-controller]] — default controller.
- [[bp-objective-console]] — console class set in RunFlowConfig.
- [[bp-bonus-chest]] — chest class set in RunFlowConfig.
- [[bp-extraction-shuttle]] — shuttle class set in RunFlowConfig.
- [[bp-gen-test-game-mode]] — the generator test map game mode, same pawn and controller, run flow off.

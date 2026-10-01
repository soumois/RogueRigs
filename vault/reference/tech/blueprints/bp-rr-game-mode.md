---
id: bp-rr-game-mode
description: "BP_RRGameMode, Blueprint of ARRGameMode setting BP_Miner and BP_RRPlayerController as defaults; project default and L_ArenaM1 override."
parent: blueprints-index
kind: doc
---
# BP_RRGameMode

- asset_path: /Game/RogueRigs/Blueprints/Player/BP_RRGameMode
- parent_class: ARRGameMode (/Script/RogueRigs.RRGameMode)
- purpose: run game mode shell; selects the miner and its controller ([[sys-player]]).
- components: none.
- exposed_defaults: DefaultPawnClass = BP_Miner_C; PlayerControllerClass = BP_RRPlayerController_C.
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

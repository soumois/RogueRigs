---
id: bp-rr-player-controller
description: "BP_RRPlayerController, Blueprint of ARRPlayerController wired to IMC_Miner, IA_Move, IA_StickAim and IA_MouseAim; open for the input mappings and modifiers."
parent: blueprints-index
kind: doc
---
# BP_RRPlayerController

- asset_path: /Game/RogueRigs/Blueprints/Player/BP_RRPlayerController
- parent_class: ARRPlayerController (/Script/RogueRigs.RRPlayerController)
- purpose: routes Enhanced Input to the miner ([[sys-player]]).
- components: none.
- exposed_defaults: DefaultMappingContext = /Game/RogueRigs/Input/IMC_Miner; MoveAction = IA_Move; StickAimAction = IA_StickAim; MouseAimAction = IA_MouseAim (all Axis2D, in /Game/RogueRigs/Input/, created by duplicating the TwinStick assets, originals untouched).
- events: none.
- logic_in_bp: none
- assets: IMC_Miner mappings (all on IA_Move unless noted): Gamepad Left 2D (no modifier); W and Z: Swizzle Input Axis Values YXZ; S: Swizzle YXZ then Negate; D: none; A and Q: Negate. IA_StickAim: Gamepad Right 2D (no modifier). IA_MouseAim: Mouse XY 2D-Axis (no modifier).
- owner_agent: agent-bp

## Relations
- [[blueprints-index]] — parent index.
- [[sys-player]] — the C++ class and the input axis convention.
- [[bp-rr-game-mode]] — default controller of this game mode.

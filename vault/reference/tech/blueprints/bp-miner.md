---
id: bp-miner
description: "BP_Miner, Blueprint of ARRMiner with the Quinn mannequin mesh, ABP_Unarmed and TwinStick capsule size; open to see its visual defaults and PIE readings."
parent: blueprints-index
kind: doc
---
# BP_Miner

- asset_path: /Game/RogueRigs/Blueprints/Player/BP_Miner
- parent_class: ARRMiner (/Script/RogueRigs.RRMiner)
- purpose: the playable miner pawn; visuals only, all logic in C++ ([[sys-player]]).
- components: inherited from C++ only (capsule, mesh, character movement, spring arm, camera, ability system component, three attribute sets); none added.
- exposed_defaults: DroneClass = BP_Drone_C ([[bp-drone]]); Mesh SkeletalMeshAsset = /Game/Characters/Mannequins/Meshes/SKM_Quinn_Simple, AnimClass = ABP_Unarmed_C (mode AnimationBlueprint), relative location (0, 0, -95), rotation yaw -90; capsule half height 88, radius 34 (all copied from BP_TwinStickCharacter, untouched); all C++ defaults kept (camera pitch -50, arm 2200, FOV 75, BaseStatsRowName Miner).
- events: none.
- logic_in_bp: none
- assets: SKM_Quinn_Simple and ABP_Unarmed from the template mannequins.
- owner_agent: agent-bp
- verified: PIE on L_ArenaM1 spawns BP_Miner_C at (8100, 8100, 90.15); MaxWalkSpeed 600; Health 100, MaxHealth 100, MoveSpeed 600, MiningSpeed 1, PickupRadius 300, XPGain 1, Luck 0.

## Relations
- [[blueprints-index]] — parent index.
- [[sys-player]] — the C++ class it derives from.
- [[bp-rr-game-mode]] — default pawn of this game mode.
- [[bp-drone]] — the drone Blueprint set as its `DroneClass`.

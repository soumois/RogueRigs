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
- events: BeginPlay assigns the LevelUp component delegate OnLevelUpEffect to a custom event (task-014-04): it adds a StaticMeshComponent (engine Cylinder, scale (1.5, 1.5, 0.02), z -80, MI_GB_PickupXP cyan, no collision), grows its scale to 6.3 in six 0.1 s steps, then destroys it (a pulsing cyan disc under the miner, not a hollow ring). Sound hook: open, empty. Tunables ChoiceDelay 0.8, HealFraction 0.2, CardCount 3 are the C++ defaults ([[sys-level-up]]).
- logic_in_bp: visual effect only (level-up disc)
- assets: SKM_Quinn_Simple and ABP_Unarmed from the template mannequins.
- owner_agent: agent-bp
- weapons: StartingWeapons = [BP_BurstRifle_C], WeaponAttachSocketName = None (task-008-03: attached to the capsule at the C++ default WeaponAttachOffset (0, 20, -40), no hand socket; the hand_r socket pitched the rifle about 84° up) ([[bp-burst-rifle]]). Read back after save; the in-game muzzle height is not yet checked.
- verified: PIE on L_ArenaM1 spawns BP_Miner_C at (8100, 8100, 90.15); MaxWalkSpeed 600; Health 100, MaxHealth 100, MoveSpeed 600, MiningSpeed 1, PickupRadius 300, XPGain 1, Luck 0.

## Relations
- [[blueprints-index]] — parent index.
- [[sys-player]] — the C++ class it derives from.
- [[bp-rr-game-mode]] — default pawn of this game mode.
- [[bp-drone]] — the drone Blueprint set as its `DroneClass`.
- [[sys-level-up]] — the effect event the disc listens to.
- [[bp-burst-rifle]] — its starting weapon.

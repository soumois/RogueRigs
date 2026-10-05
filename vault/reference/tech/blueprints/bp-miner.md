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
- components: inherited from C++ plus LevelUpDisc (engine cylinder, scale 1.5, 1.5, 0.02 at z -80, MI_GB_PickupXP cyan, hidden, NoCollision, no shadow); int variable DiscStep; function DiscStep_Tick.
- exposed_defaults: DroneClass = BP_Drone_C ([[bp-drone]]); Mesh SkeletalMeshAsset = /Game/Characters/Mannequins/Meshes/SKM_Quinn_Simple, AnimClass = ABP_Unarmed_C (mode AnimationBlueprint), relative location (0, 0, -95), rotation yaw -90; capsule half height 88, radius 34 (all copied from BP_TwinStickCharacter, untouched); all C++ defaults kept (camera pitch -50, arm 2200, FOV 75, BaseStatsRowName Miner).
- events: BeginPlay assigns the LevelUp component delegate OnLevelUpEffect to a custom event (task-014-04; rebuilt in task-024-05). The event clears the DiscStep_Tick timer, sets DiscStep 0, resets LevelUpDisc scale to 1.5, shows it and sets a 0.1 s timer by function name; DiscStep_Tick adds 1 to DiscStep, sets scale 1.5 + 0.8 * step, re-arms the timer while step < 6, then hides the disc. No component is added or destroyed, so overlapping level-ups just restart the animation (the old AddComponent/Delay version raised "not valid (pending kill or garbage)" runtime errors). Sound hook: open, empty. Tunables ChoiceDelay 0.8, HealFraction 0.2, CardCount 3 are the C++ defaults ([[sys-level-up]]).
- logic_in_bp: visual effect only (level-up disc timer steps)
- assets: SKM_Quinn_Simple and ABP_Unarmed from the template mannequins.
- owner_agent: agent-bp
- weapons (task-024-05): StartingWeapons = [BP_BurstRifle_C] only; GuaranteedWeaponClasses empty; WeaponCatalogue = BP_BurstRifle_C, BP_Shotgun_C, BP_TurretLauncher_C with French descriptions ("Des rafales précises qui traversent les robots.", "Une gerbe de plombs à courte portée qui repousse les robots.", "Pose des tourelles qui tirent seules sur les robots proches."); MaxWeapons 4. Earlier: WeaponAttachSocketName = None (attached to the capsule at WeaponAttachOffset (0, 20, -40), task-008-03) ([[bp-burst-rifle]]).
- verified: PIE (task-024-05): starts with BP_BurstRifle_C_0 only; new-weapon cards for the shotgun and the turret launcher appeared in the offer (CurrentOffer) and picking them added BP_Shotgun_C_0 and BP_TurretLauncher_C_0. Level-up disc rebuilt as a persistent component: PIE with GrantXP 100 and five ChooseUpgrade 0 picks logged no Blueprint Runtime Error (DiscStep read 5 afterwards; reaching 6 and hiding the disc was not confirmed before PIE stopped). Earlier: PIE on L_ArenaM1 spawns BP_Miner_C at (8100, 8100, 90.15); MaxWalkSpeed 600; Health 100, MaxHealth 100, MoveSpeed 600, MiningSpeed 1, PickupRadius 300, XPGain 1, Luck 0.

## Relations
- [[blueprints-index]] — parent index.
- [[sys-player]] — the C++ class it derives from.
- [[bp-rr-game-mode]] — default pawn of this game mode.
- [[bp-drone]] — the drone Blueprint set as its `DroneClass`.
- [[sys-level-up]] — the effect event the disc listens to.
- [[bp-burst-rifle]] — its starting weapon.
- [[bp-shotgun]] — a catalogue weapon (no longer a starting weapon).
- [[bp-turret]] — the turret launcher in its catalogue.

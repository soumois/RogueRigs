---
id: bp-miner
description: "BP_Miner, Blueprint of ARRMiner with the Meshy SK_Miner mesh, ABP_Miner, MI_Miner brightness scale (BaseColorScale) and capsule size; open for visual defaults, tuning and PIE readings."
parent: blueprints-player-index
kind: doc
---
# BP_Miner

- asset_path: /Game/RogueRigs/Blueprints/Player/BP_Miner
- parent_class: ARRMiner (/Script/RogueRigs.RRMiner)
- purpose: the playable miner pawn; visuals only, all logic in C++ ([[sys-player]]).
- components: inherited from C++ plus LevelUpDisc (engine cylinder, scale 1.5, 1.5, 0.02 at z -80, MI_GB_PickupXP cyan, hidden, NoCollision, no shadow); int variable DiscStep; function DiscStep_Tick.
- exposed_defaults: DroneClass = BP_Drone_C ([[bp-drone]]); Mesh SkeletalMeshAsset = /Game/RogueRigs/Characters/Miner/SK_Miner (Meshy, task-386 mesh swap 46f17540), AnimClass = ABP_Miner_C (parent URRMinerAnimInstance, task-386); mesh relative location and rotation after the swap: open (not re-read in task-393); capsule half height 88, radius 34 (copied from BP_TwinStickCharacter, untouched); all C++ defaults kept (camera pitch -50, arm 1400 since task-255-01, FOV 75, BaseStatsRowName Miner; task-255-01: the package holds no CameraArmLength or TargetArmLength name, so no override).
- events: BeginPlay assigns the LevelUp component delegate OnLevelUpEffect to a custom event (task-014-04; rebuilt in task-024-05). The event clears the DiscStep_Tick timer, sets DiscStep 0, resets LevelUpDisc scale to 1.5, shows it and sets a 0.1 s timer by function name; DiscStep_Tick adds 1 to DiscStep, sets scale 1.5 + 0.8 * step, re-arms the timer until step 5, then hides the disc (0.5 s, max scale 5.5; orchestrator decision 2026-10-05: keep this short animation). No component is added or destroyed, so overlapping level-ups just restart the animation (the old AddComponent/Delay version raised "not valid (pending kill or garbage)" runtime errors). Task-028-02: the same event also spawns BP_HealRing ([[bp-heal-ring]]) at the miner feet (location z - 85) through SpawnActorFromClass, so the level-up heal shows the green ring (PIE: no runtime error). Sound hook: open, empty. Tunables ChoiceDelay 0.8, HealFraction 0.2, CardCount 3 are the C++ defaults ([[sys-level-up]]).
- logic_in_bp: visual effect only (level-up disc timer steps)
- assets: SK_Miner, ABP_Miner and BS_Miner_Locomotion (task-386, state in [[task-386-miner-mixamo-animation]]; its result notes an open T-pose arm defect in the blend space); material chain in the Miner material section below. SKM_Quinn_Simple and ABP_Unarmed (template mannequins) stay in the project, unused here.
- owner_agent: agent-bp
- weapons (task-024-05): StartingWeapons = [BP_BurstRifle_C] only; GuaranteedWeaponClasses empty; WeaponCatalogue = BP_BurstRifle_C, BP_Shotgun_C, BP_TurretLauncher_C with English descriptions since task-032-02 ("Precise bursts that pierce through robots.", "A short-range spray of pellets that knocks robots back.", "Places turrets that fire at nearby robots on their own."); MaxWeapons 4. Earlier: WeaponAttachSocketName = None (attached to the capsule at WeaponAttachOffset (0, 20, -40), task-008-03) ([[bp-burst-rifle]]).
- verified: PIE (task-024-05): starts with BP_BurstRifle_C_0 only; new-weapon cards for the shotgun and the turret launcher appeared in the offer (CurrentOffer) and picking them added BP_Shotgun_C_0 and BP_TurretLauncher_C_0. Level-up disc rebuilt as a persistent component: PIE with GrantXP 100 and five ChooseUpgrade 0 picks logged no Blueprint Runtime Error (DiscStep read 5 afterwards). Headless (task-024-06, Integration.LevelUpDiscEndsHidden): the timer survives the level-up pause and the disc ends hidden, but DiscStep_Tick hides it on the tick that sets step 5 (0.5 s, max scale 5.5), which is the kept behaviour (doc corrected). Earlier: PIE on L_ArenaM1 spawns BP_Miner_C at (8100, 8100, 90.15); MaxWalkSpeed 600; Health 100, MaxHealth 100, MoveSpeed 600, MiningSpeed 1, PickupRadius 300, XPGain 1, Luck 0.

## Miner material and brightness (task-393-01, provisional)
Owner request 2026-10-30: the Meshy miner looked burnt out in the arena. Lever chosen: the miner material only (Q9 A), value Q10 0.6 provisional, the owner tunes by eye ([[task-393-miner-over-lit]]). Not verified by render or PIE.
- M_Miner (/Game/RogueRigs/Characters/Miner/M_Miner, made by the import script): Default Lit; TextureSample_0 (T_Miner_BaseColor) -> new Multiply node (B = scalar parameter `BaseColorScale`, default 1) -> BaseColor; TextureSample_1 (T_Miner_Normal, Normal sampler) is wired to Normal. Roughness, Specular and Metallic are unwired (engine defaults; numbers not read back); emissive not queried.
- Source texture: mean albedo of texture_0.png about 0.35 sRGB (about 0.10 linear), measured on the source PNG, not the imported asset.
- MI_Miner (/Game/RogueRigs/Characters/Miner/MI_Miner, new): child of M_Miner, `BaseColorScale` 0.6. SK_Miner single slot `BakedMaterial` uses MI_Miner; BP_Miner has no override materials, so it is unchanged. Lights, exposure and bloom untouched ([[lights-dark-ambient]]).
- Tune: open MI_Miner in the editor and change `BaseColorScale` (live in PIE); lower = darker.
- Revert: set `BaseColorScale` 1, or assign M_Miner back on the SK_Miner slot.

## PlayerLight (removed, task-272-15)
Removed on 2026-10-21 by owner direction ("repars d une lumiere flat"): the centre-brighter effect now comes only from the post-process mask ([[mpp-player-light-mask]]). Removed with `ActorTools.remove_component`, compiled and saved; the graphs hold no reference to it. History of the tuning rounds (task-263-02 to task-272-04): [[lights-dark-ambient]], [[lights-no-shadows]].
- Revert: add a PointLightComponent named `PlayerLight` under the capsule (CollisionCylinder), not under the mesh, with the last values (task-272-02): relativeLocation (0, 0, -15), attenuationRadius 3000, intensity 400, intensityUnits Candelas, lightColor (1.0, 0.7608, 0.6196) (8-bit 255/194/158), sourceRadius 20, castShadows true, bUseInverseSquaredFalloff true, mobility Movable; rest default. Then compile and save. Cost of that light: [[perf-risks]].

## Relations
- [[blueprints-index]] — parent index.
- [[sys-player]] — the C++ class it derives from.
- [[bp-rr-game-mode]] — default pawn of this game mode.
- [[bp-drone]] — the drone Blueprint set as its `DroneClass`.
- [[sys-level-up]] — the effect event the disc listens to.
- [[bp-burst-rifle]] — its starting weapon.
- [[bp-shotgun]] — a catalogue weapon (no longer a starting weapon).
- [[bp-heal-ring]] — the ring it spawns on level-up.
- [[bp-turret]] — the turret launcher in its catalogue.
- [[lights-no-shadows]] — the light was the only shadow caster.
- [[perf-risks]] — cost of the removed shadowed light.
- [[mpp-player-light-mask]] — the post-process mask that replaces the light.
- [[task-386-miner-mixamo-animation]] — imported SK_Miner and ABP_Miner and swapped the mesh.
- [[task-393-miner-over-lit]] — added MI_Miner and BaseColorScale.

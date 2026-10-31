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
- weapons (task-024-05): StartingWeapons = [BP_BurstRifle_C] only; GuaranteedWeaponClasses empty; WeaponCatalogue = BP_BurstRifle_C, BP_Shotgun_C, BP_TurretLauncher_C with English descriptions since task-032-02 ("Precise bursts that pierce through robots.", "A short-range spray of pellets that knocks robots back.", "Places turrets that fire at nearby robots on their own."); MaxWeapons 4. task-400: skeletal mesh asset SK_Miner has socket WeaponSocket_R on bone RightHand (location (0.005868, -0.065782, -0.001565), rotation pitch -17.502942 yaw -79.928061 roll 17.606433, scale 0.01; the RightHand bone scale is about 100 in component space, so the socket gives world scale 1). Added on the mesh asset, not SKEL_Miner (no socket tool). Mesh relative yaw -90, location z -88; mesh forward = component +Y. Derived in PIE from A_Miner_Idle (socket X along mesh forward, grip midway between RightHand and RightHandMiddle1); PIE check with the ABP in idle and run fwd/back/left/right: rifle at the socket, scale 1, X axis within about 16 deg pitch and 10 deg yaw in strafes, 0 in idle. Values provisional, the owner can retouch in the Skeleton editor. Mixamo rifle stance puts the right hand at the cheek/shoulder (rifle shouldered beside the head); the left hand was about 14 cm off the handguard, fixed by the left-hand IK below (task-401). BP_BurstRifle sets HandSocketName to this socket ([[bp-burst-rifle]]). Earlier: WeaponAttachSocketName = None (attached to the capsule at WeaponAttachOffset (0, 20, -40), task-008-03) ([[bp-burst-rifle]]).
- verified: PIE (task-024-05): starts with BP_BurstRifle_C_0 only; new-weapon cards for the shotgun and the turret launcher appeared in the offer (CurrentOffer) and picking them added BP_Shotgun_C_0 and BP_TurretLauncher_C_0. Level-up disc rebuilt as a persistent component: PIE with GrantXP 100 and five ChooseUpgrade 0 picks logged no Blueprint Runtime Error (DiscStep read 5 afterwards). Headless (task-024-06, Integration.LevelUpDiscEndsHidden): the timer survives the level-up pause and the disc ends hidden, but DiscStep_Tick hides it on the tick that sets step 5 (0.5 s, max scale 5.5), which is the kept behaviour (doc corrected). Earlier: PIE on L_ArenaM1 spawns BP_Miner_C at (8100, 8100, 90.15); MaxWalkSpeed 600; Health 100, MaxHealth 100, MoveSpeed 600, MiningSpeed 1, PickupRadius 300, XPGain 1, Luck 0.

## Left hand IK in ABP_Miner (task-401-03, provisional)
Owner "Ok, par défaut" 2026-10-30 ([[task-401-miner-hands-fit-rifle]]). AnimGraph (since task-406 the upper body layer sits before LocalToComponent, see the section below): Locomotion state machine -> Layered blend per bone -> LocalToComponent -> Two Bone IK -> ComponentToLocal -> Output Pose.
- Two Bone IK: IK bone LeftHand (chain LeftHand, LeftForeArm, LeftArm); effector location = `LeftHandIKTarget` in Bone Space of bone RightHand; alpha = `LeftHandIKAlpha` (both from [[sys-player-classes]]). Joint target (45, 10, 95) cm in Component Space (mesh forward is component +Y, left is +X; elbow down and out), provisional. Stretching on with defaults (from 1.0, max 1.2); twist defaults; the end bone takes no effector rotation (hand orientation not matched to the grip).
- Which value moves what (task-404): the left HAND is set by the socket `LeftHandGrip` on SM_BurstRifle (Static Mesh Editor, Socket Manager). The Joint Target Location is the left ELBOW pole only; editing it does not move the hand, and in the ABP editor preview the IK alpha is 0 (no weapon held), so nothing visibly changes there. The target is computed at weapon add, so a live RifleMesh or LeftHandGrip edit in PIE is not followed, a saved one is at the next spawn ([[sys-player-classes]]).
- Grip point: socket `LeftHandGrip` on SM_BurstRifle ([[burst-rifle-mesh]]).
- verified: PIE read-back hand-to-grip distance 0.00001 uu standing and in the four forced directions; transients of 2-3 frames at start or force change up to about 26 uu. Capture (task-401-04, frames in `Saved/Captures/HandsFit401/`): C1 to C3 ambiguous (right hand hidden from the top-down camera, left hand at the front of the rifle), C4 seen with caveats; the owner judges.

## Upper body layer in ABP_Miner (task-406, provisional)
Decided in [[task-406-miner-upper-body-idle-layer]] (Q1, Q2). Commits: cpp 49a881c3, bp b03e520d.
- AnimGraph: State machine `Locomotion` -> Layered blend per bone -> LocalToComponent -> TwoBoneIK (LeftHand, effector `LeftHandIKTarget`, alpha `LeftHandIKAlpha`) -> ComponentToLocal -> Output Pose.
- Layered blend per bone: BasePose = Locomotion; BlendPoses_0 = looping Sequence Player whose Sequence pin is fed by Get `UpperBodySequence`; branch filter bone `Spine`, blend depth 0; mesh space rotation blend on; weight 1. Legs from locomotion, spine and above from the sequence.
- Class default: ABP_Miner `UpperBodySequence` = /Game/RogueRigs/Characters/Miner/Anims/A_Miner_UpperRifle_V2 since task-408-03 (was A_Miner_Idle), provisional ([[sys-player-classes]]).
- Swap the upper source (another weapon pose): change `UpperBodySequence` in the ABP_Miner class defaults; later it can be set from the starting weapon.
- The unconnected `TwoBoneIK_0` leftover was dropped in task-408-03. The graph holds no right arm correction node (the task-407 offsets were removed in task-408-01).
- Unverified: no PIE reading (RightHand and LeftHand relative to Spine while moving; LeftHand vs `LeftHandGrip`); compiler log not checked for a Spine bone warning; bone name `Spine` not read back from SKEL_Miner.

## Upper rifle variants in ABP_Miner (task-408, provisional)
Owner 2026-10-31 ([[task-408-miner-right-arm-in-animation]]). Commits: cpp 4e0cce39, art d7549f09, bp a208103f.
- Assets: /Game/RogueRigs/Characters/Miner/Anims/A_Miner_UpperRifle_V1, _V2, _V3 on SKEL_Miner, imported by `Tools/unreal/import_miner_upper_rifle.py` (editor Python). Right elbow raised 3, 6, 9 cm over A_Miner_Idle, right hand unchanged; construction and measures in [[miner-mixamo-clips]]. A_Miner_Idle is untouched.
- Active: `UpperBodySequence` class default = A_Miner_UpperRifle_V2, provisional until the owner picks.
- Pick another: set `UpperBodySequence` in the ABP_Miner Class Defaults. For a test without editing the asset: console `rr.Debug.MinerUpperPose <name>` (e.g. A_Miner_UpperRifle_V1; empty restores the class default), optionally with `rr.Debug.MinerAnimFreeze 1` ([[sys-player-classes]]).
- Capture (local, gitignored): `Saved/Captures/UpperRifle408/` with `sheet_side.png`, `sheet_front.png`, `side_*_00.png`, `front_*_00.png` for idle, V1, V2, V3.
- Capture verdicts: C1 ambiguous (the miner drone floats at a different place per frame); C2 ambiguous, leaning seen (elbow rises from idle to V3 in the side view; 3, 6, 9 cm not confirmable; front view dark); C3 ambiguous (rifle in place, near hand moves slightly); C4 seen; C5 seen (no pose problem visible).
- Unverified: elbow heights in the editor or PIE; the FBX import logged "invalid bind poses" warnings (no visible effect in the captures). The owner compares the sheets or uses `rr.Debug.MinerUpperPose`, then picks.

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
- [[task-400-burst-rifle-in-hand]] — added WeaponSocket_R.
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
- [[task-401-miner-hands-fit-rifle]] — added the left hand Two Bone IK in ABP_Miner.
- [[sys-player-classes]] — the anim instance variables the IK reads.
- [[burst-rifle-mesh]] — the LeftHandGrip socket.
- [[task-404-rifle-mesh-owner-offset]] — left hand vs elbow pole note and refresh limit.
- [[task-393-miner-over-lit]] — added MI_Miner and BaseColorScale.
- [[task-406-miner-upper-body-idle-layer]] — added the upper body layer and UpperBodySequence.
- [[task-408-miner-right-arm-in-animation]] — added the upper rifle variants and removed the arm offsets.
- [[miner-mixamo-clips]] — how the three variant clips were built.

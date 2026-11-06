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
- components: inherited from C++ plus FloorWallLight (below). The cyan LevelUpDisc, its variable DiscStep and its function DiscStep_Tick were removed in task-441-05.
- exposed_defaults: LevelUp component (CDO path `BP_Miner.Default__BP_Miner_C:LevelUp`, task-441): LevelUpTextSystem = NS_LevelUpText, LevelUpWaveSystem = NS_LevelUpWave ([[bp-level-up-burst]]); DroneClass = BP_Drone_C ([[bp-drone]]); Mesh SkeletalMeshAsset = /Game/RogueRigs/Characters/Miner/SK_Miner (Meshy, task-386 mesh swap 46f17540), AnimClass = ABP_Miner_C (parent URRMinerAnimInstance, task-386); mesh relative location and rotation after the swap: open (not re-read in task-393); capsule half height 88, radius 34 (copied from BP_TwinStickCharacter, untouched); all C++ defaults kept (camera pitch -50, arm 1400 since task-255-01, FOV 75, BaseStatsRowName Miner; task-255-01: the package holds no CameraArmLength or TargetArmLength name, so no override).
- events: BeginPlay assigns the LevelUp component delegate OnLevelUpEffect to a custom event (task-014-04; rebuilt in task-024-05). The event spawns BP_HealRing ([[bp-heal-ring]]) at the miner feet (location z - 85) through SpawnActorFromClass, once per level gained, so the level-up heal shows the green ring (task-028-02). Text, wave and robot push are played by C++ ([[sys-level-up]]); the cyan disc animation of this event was removed in task-441-05. Sound hook: open, empty. Tunables ChoiceDelay 0.8, HealFraction 0.2, CardCount 3 are the C++ defaults ([[sys-level-up]]).
- logic_in_bp: visual effect only (heal ring spawn)
- assets: SK_Miner, ABP_Miner and BS_Miner_Locomotion (task-386, state in [[task-386-miner-mixamo-animation]]; its result notes an open T-pose arm defect in the blend space); material chain in the Miner material section below. SKM_Quinn_Simple and ABP_Unarmed (template mannequins) stay in the project, unused here.
- owner_agent: agent-bp
- weapons (task-024-05): StartingWeapons = [BP_BurstRifle_C] only; GuaranteedWeaponClasses empty; WeaponCatalogue = BP_BurstRifle_C, BP_Shotgun_C, BP_TurretLauncher_C with English descriptions since task-032-02 ("Precise bursts that pierce through robots.", "A short-range spray of pellets that knocks robots back.", "Places turrets that fire at nearby robots on their own."); MaxWeapons 4. task-400: skeletal mesh asset SK_Miner has socket WeaponSocket_R on bone RightHand (location (0.005868, -0.065782, -0.001565), rotation pitch -17.502942 yaw -79.928061 roll 17.606433, scale 0.01; the RightHand bone scale is about 100 in component space, so the socket gives world scale 1). Added on the mesh asset, not SKEL_Miner (no socket tool). Mesh relative yaw -90, location z -88; mesh forward = component +Y. Derived in PIE from A_Miner_Idle (socket X along mesh forward, grip midway between RightHand and RightHandMiddle1); PIE check with the ABP in idle and run fwd/back/left/right: rifle at the socket, scale 1, X axis within about 16 deg pitch and 10 deg yaw in strafes, 0 in idle. Values provisional, the owner can retouch in the Skeleton editor. Mixamo rifle stance puts the right hand at the cheek/shoulder (rifle shouldered beside the head); the left hand was about 14 cm off the handguard, fixed by the left-hand IK below (task-401). BP_BurstRifle sets HandSocketName to this socket ([[bp-burst-rifle]]). Earlier: WeaponAttachSocketName = None (attached to the capsule at WeaponAttachOffset (0, 20, -40), task-008-03) ([[bp-burst-rifle]]).
- verified: PIE (task-024-05): starts with BP_BurstRifle_C_0 only; new-weapon cards for the shotgun and the turret launcher appeared in the offer (CurrentOffer) and picking them added BP_Shotgun_C_0 and BP_TurretLauncher_C_0. Task-441-05: compiled with warnings as errors; PIE GrantXP 5 and ChooseUpgrade 0 logged no Blueprint runtime error and no Niagara warning. Earlier: PIE on L_ArenaM1 spawns BP_Miner_C at (8100, 8100, 90.15); MaxWalkSpeed 600; Health 100, MaxHealth 100, MoveSpeed 600, MiningSpeed 1, PickupRadius 300, XPGain 1, Luck 0.

## Mesh scale 0.9 (task-474, provisional)
Owner 2026-11-06 "réduit la taille du joueur de 10 pourcent" ([[task-474-miner-scale-down]]). Commit 39732c16.
- Mesh (CharacterMesh0) RelativeScale3D 1 -> 0.9; location (0, 0, -88) and yaw -90 kept, so the feet (mesh origin) stay on the floor. Top of head at rest 170 uu -> 153 uu (SK_Miner bind pose and idle frames measured in Blender, times 0.9).
- Unchanged (provisional, to confirm): capsule half height 88, radius 34, every gameplay value; camera, drone, FloorWallLight, ABP_Miner.
- Follows the scale: the burst rifle (world scale 0.9 through the C++ attach rule, [[sys-player-classes]]), its Muzzle (about 110 uu above the capsule bottom instead of about 122), aim laser and muzzle flash at the Muzzle, the hand offset and the left-hand grip. Damage, heal and level-up numbers stay at their capsule-based height.
- Capture: `Saved/Captures/MinerScale474Before/`, `MinerScale474After/`, sheets in `MinerScale474Sheets/` (local, gitignored).
- Revert: set Mesh RelativeScale3D back to 1 in BP_Miner, compile, save (the attach rule gives the rifle scale 1 again).

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
- Class default: ABP_Miner `UpperBodySequence` = /Game/RogueRigs/Characters/Miner/Anims/A_Miner_UpperRifle_V2Grip since task-409-03 (was A_Miner_UpperRifle_V2 from task-408-03, before that A_Miner_Idle) ([[sys-player-classes]]; see the V2Grip section below).
- Swap the upper source (another weapon pose): change `UpperBodySequence` in the ABP_Miner class defaults; later it can be set from the starting weapon.
- The unconnected `TwoBoneIK_0` leftover was dropped in task-408-03. The graph holds no right arm correction node (the task-407 offsets were removed in task-408-01).
- Unverified: no PIE reading (RightHand and LeftHand relative to Spine while moving; LeftHand vs `LeftHandGrip`); compiler log not checked for a Spine bone warning; bone name `Spine` not read back from SKEL_Miner.

## Upper rifle variants in ABP_Miner (task-408)
Owner 2026-10-31 ([[task-408-miner-right-arm-in-animation]]). Commits: cpp 4e0cce39, art d7549f09, bp a208103f.
- Assets: /Game/RogueRigs/Characters/Miner/Anims/A_Miner_UpperRifle_V1, _V2, _V3 on SKEL_Miner, imported by `Tools/unreal/import_miner_upper_rifle.py` (editor Python). Right elbow raised 3, 6, 9 cm over A_Miner_Idle, right hand unchanged; construction and measures in [[miner-mixamo-clips]]. A_Miner_Idle is untouched.
- Active until task-409: `UpperBodySequence` = A_Miner_UpperRifle_V2, chosen by the owner 2026-10-31 ("Je prends le v2"); now replaced by V2Grip (next section). V1, V2, V3 and A_Miner_Idle stay in the project, kept until he asks.
- Pick another: set `UpperBodySequence` in the ABP_Miner Class Defaults. For a test without editing the asset: console `rr.Debug.MinerUpperPose <name>` (e.g. A_Miner_UpperRifle_V1; empty restores the class default), optionally with `rr.Debug.MinerAnimFreeze 1` ([[sys-player-classes]]).
- Capture (local, gitignored): `Saved/Captures/UpperRifle408/` with `sheet_side.png`, `sheet_front.png`, `side_*_00.png`, `front_*_00.png` for idle, V1, V2, V3.
- Capture verdicts: C1 ambiguous (the miner drone floats at a different place per frame); C2 ambiguous, leaning seen (elbow rises from idle to V3 in the side view; 3, 6, 9 cm not confirmable; front view dark); C3 ambiguous (rifle in place, near hand moves slightly); C4 seen; C5 seen (no pose problem visible).
- Unverified: elbow heights in the editor or PIE; the FBX import logged "invalid bind poses" warnings (no visible effect in the captures). The owner compared the variants and picked V2.

## Upper rifle V2Grip in ABP_Miner (task-409, provisional)
Owner request: right hand on the rifle pistol grip, rifle kept where the owner placed it ([[task-409-miner-right-hand-on-grip]]). Commits: 3d 0383712e, bp 3266af72 (import) and 84c03a7d (rifle).
- Asset: /Game/RogueRigs/Characters/Miner/Anims/A_Miner_UpperRifle_V2Grip on SKEL_Miner, imported by `Tools/unreal/import_miner_upper_rifle_v2grip.py` (editor Python); length 3.1 s, get_num_frames 93 (same as V2), loop true, root motion off. Built from V2 in Blender ([[miner-mixamo-clips]]): all 94 keys reach, RightHand moved 16.38 cm in hand space so WeaponSocket_R lands where V2 put the grip (residual 0.0001 cm), hand rotation unchanged, fingers and other bones identical, RightForeArm head height kept at its V2 height on every frame (2-bone solve, hand pinned, Q2).
- Active: ABP_Miner `UpperBodySequence` class default = A_Miner_UpperRifle_V2Grip, compiled with no error, saved. V1, V2, V3 and A_Miner_Idle kept.
- Why the rifle keeps its place: the hand moved to the rifle, not the reverse; RifleMesh offset reset in [[bp-burst-rifle]] (the V2 pose carried the old offset, now baked into the clip).
- Switch back: set `UpperBodySequence` to A_Miner_UpperRifle_V2 in the ABP_Miner Class Defaults. Console `rr.Debug.MinerUpperPose A_Miner_UpperRifle_V2` also works, but with V2 the hand only meets the grip if RifleMesh keeps its old offset (9.898091, -1.129037, -13.0); with RifleMesh at 0 the V2 hand sits 16.38 cm off the grip.
- Unverified: no PIE or capture (the owner checks himself, Q3); compile logs not read; projectile spawn height and left-hand IK (LeftHandGrip) not checked after the change.

## Right hand offset in ABP_Miner (task-410, provisional)
Owner request: move the right hand live ([[task-410-right-hand-offset-live]]); task-411 then made the rifle stay in place (section below). Commits: cpp 5ca2bac7, bp 1e969c39. Unverified: no PIE run by the crew (the owner checks); compile and reach not read.
- AnimGraph order: Locomotion -> Layered blend per bone -> LocalToComponent -> right Two Bone IK -> Transform (Modify) Bone RightHand -> left Two Bone IK (unchanged) -> ComponentToLocal -> Output Pose.
- Right Two Bone IK: IK bone RightHand; effector Bone Space of RightHand = `RightHandIKEffector`; joint target (0, 0, 0) Bone Space of RightForeArm; stretching off; alpha `RightHandOffsetAlpha`.
- Transform (Modify) Bone: bone RightHand; translation Ignore, scale Ignore, rotation Add to Existing, Bone Space = `RightHandRotationDelta`; alpha `RightHandOffsetAlpha`.
- Inputs come from `RightHandOffsetLocation` and `RightHandOffsetRotation` ([[sys-player-classes]]: frame, pivot, math). Offset 0 = the V2Grip pose unchanged (alpha 0). The ABP moves the hand only; the rifle is counter-moved in C++ so it stays in place (below).
- Tuning recipe: in PIE type `rr.Debug.RightHandOffset X Y Z [Pitch Yaw Roll]` (cm, deg, socket frame: X muzzle, Y side, Z rifle top); read the LogRogueRigs line `RightHandOffset ...`; type its `RightHandOffsetLocation` and `RightHandOffsetRotation` into the ABP_Miner Class Defaults (category Animation|RightHand) to keep them; `rr.Debug.RightHandOffset ""` restores the class defaults.
- Reach risk: the arm has a fixed length and stretching is off, so keep offsets to a few cm; beyond reach the hand stops short of the target.

## Right hand offset, rifle stays (task-411, provisional)
Decided in [[task-411-right-hand-offset-rifle-stays]] (cpp 3b80bb41; no Blueprint change). The offset now slides the hand on a rifle that stays in place: C++ counter-moves `RifleMesh` and `Muzzle` by the inverse offset, the left hand IK target is refreshed after ([[sys-player-classes]]). Animations that move the hand still move the rifle; the offset alone does not. The ABP graph is unchanged.
- Tuning recipe (replaces the task-410 wording): type `rr.Debug.RightHandOffset X Y Z [Pitch Yaw Roll]`, the hand slides on the still rifle; the log line now also ends with the counter-moved `relative=(...)` of RifleMesh and Muzzle; keep the offset in the ABP_Miner Class Defaults as before.
- Reach risk (Q1, no clamp): past arm reach the hand stops short and the rifle shifts by the missing part.
- Unverified: no capture, no PIE run by the crew (build OK); rifle stays put, left hand on the handguard and restore on uninitialize not checked.

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

## FloorWallLight (task-425-02, f2c79f4c, provisional)
Owner 2026-11-04 "Tout par défaut" ([[task-425-miner-floor-wall-light]]): a light that touches only floor and walls (no shadow, colour 255/194/158, pool about 400 uu, floor under the miner below 205/255). Not render-checked: the visual check is pending after task-426 (the owner waived the capture); intensity is a first guess.
- Component: PointLightComponent `FloorWallLight`, parent CollisionCylinder, relativeLocation (0, 0, 360) (about 450 uu above the floor).
- Lighting channels: ch0 false, ch1 true, ch2 false, so it lights only components on channel 1 (floor and terrain blocks, [[sys-grid]]); the miner, enemies and barrels stay on channel 0 and are not lit by it.
- Light: castShadows false, lightColor (1.0, 0.7608, 0.6196) (8-bit 255/194/158), Movable, bUseInverseSquaredFalloff false, lightFalloffExponent 4, intensityUnits Unitless, intensity 4 (provisional), attenuationRadius 650, sourceRadius 0, indirectLightingIntensity 0, volumetric scattering off.
- Differs from the removed PlayerLight recipe above (shadowed, channel 0, Candelas, inverse squared falloff).
- Owner retouch: read back 2026-11-05 (task-441, untouched by it): relative location (0, 0, 0), attenuation radius 800, intensity 4; they replace the first values above.
- Tune: change `intensity` (and `attenuationRadius` for the pool size) on the component in BP_Miner. Revert: remove the component.
- Wall shadows (task-445-01, d7feeaf0, owner 2026-11-05, provisional; reverses Q1 of task-425): `castShadows` false -> true. Current values, read back before and after, unchanged: relative location (0, 0, 0), intensity 4, attenuation radius 800, falloff exponent 4, channel 1 only, Movable. Who casts: [[lights-no-shadows]].
- Shadow length: location 0 puts the light at the capsule centre, world Z 90.15 in PIE, under the block top at Z 100 ([[sys-grid]] `BlockHeight`). A floor point behind a wall is always hidden, so the shadow has no end of its own: it runs from the wall foot to the edge of the light (800 uu, about 12 cells). Block tops get none of this light. With the light above the block top, at height h, a wall at distance d casts a shadow d x 100 / (h - 100) long.
- Shadow off: untick Cast Shadows on `FloorWallLight` in BP_Miner, compile, save. The pool stays.
- Unverified: no capture (the owner looks in game, A6); GPU cost not measured (Q17, [[perf-risks]]).
- Calibration on owner image 2 (task-473-01, d2415904, owner 2026-11-06 "Ok pour tout le reste", provisional; [[task-473-miner-light-calibration]]): intensity 4 -> 6.5, attenuationRadius 800 -> 700, lightFalloffExponent 4 -> 3, lightColor 255/194/158 -> 255/175/60 (warm orange). Unchanged: relative location (0, 0, 0), channel 1 only, castShadows true, Unitless, no inverse squared falloff. Result: orange centre fading to red-magenta, pool size close to image 2; floor near the miner R channel p95 about 219 (above the about 205 rule; image 2 itself about 252), outer ring redder than the image's pink-violet. Captures `Saved/Captures/task473-*` (local).
- Set headless, editor closed: `Tools/unreal/miner_floor_light.py` (`intensity= radius= exponent= color=r,g,b`; no argument reads back; never writes the location; copy to a path without spaces).
- Revert to the pre-task-473 pool: `miner_floor_light.py intensity=4 radius=800 exponent=4 color=255,194,158`, or the same values on the component in BP_Miner, compile, save.

- actor_shadow (task-472, commits 890894ef, 0028c7a2): BP_Miner keeps the inherited C++ `BlobShadow` (light-independent purple-grey disc, [[sys-blob-shadow]]); per-type `Opacity` 0.6 and `SizeScale` 1.2 (owner 2026-11-06 Q3, provisional) are its Blueprint defaults, no value overridden in this Blueprint. Footprint = capsule diameter (68 uu, radius 34 unchanged by the task-474 mesh scale).

## Relations
- [[sys-blob-shadow]] — the shadow under the miner.
- [[task-474-miner-scale-down]] — set the mesh scale 0.9.
- [[task-425-miner-floor-wall-light]] — added FloorWallLight.
- [[task-445-miner-light-wall-shadows]] — turned on its wall shadows.
- [[task-473-miner-light-calibration]] — calibrated the FloorWallLight colour, intensity and size on owner image 2.
- [[task-410-right-hand-offset-live]] — added the right arm IK and hand rotation nodes.
- [[task-411-right-hand-offset-rifle-stays]] — made the rifle stay in place under the offset.
- [[task-400-burst-rifle-in-hand]] — added WeaponSocket_R.
- [[blueprints-index]] — parent index.
- [[sys-player]] — the C++ class it derives from.
- [[bp-rr-game-mode]] — default pawn of this game mode.
- [[bp-drone]] — the drone Blueprint set as its `DroneClass`.
- [[sys-level-up]] — the effect event the heal ring listens to, and the burst.
- [[bp-level-up-burst]] — the text and wave systems it sets.
- [[task-441-level-up-effect]] — removed the disc.
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
- [[miner-mixamo-clips]] — how the three variant clips and V2Grip were built.
- [[task-409-miner-right-hand-on-grip]] — added A_Miner_UpperRifle_V2Grip and set it as UpperBodySequence.

---
id: bp-turret
description: "BP_TurretLauncher, BP_Turret and BP_TurretBullet, the turret launcher, placed turret and bullet: Meshy base and yawing head, green muzzle flash and green shots."
parent: blueprints-player-index
kind: doc
---
# BP_TurretLauncher, BP_Turret, BP_TurretBullet

- asset_path: /Game/RogueRigs/Blueprints/Weapons/BP_TurretLauncher, BP_Turret, BP_TurretBullet (also BP_ShotgunPellet, see [[bp-shotgun]])
- parent_class: ARRTurretLauncher (/Script/RogueRigs.RRTurretLauncher), ARRTurret (/Script/RogueRigs.RRTurret), ARRTurretBullet (/Script/RogueRigs.RRTurretBullet)
- purpose: the visible turret weapon; looks only, charges, placement, targeting and fire are C++ ([[sys-turret]]).
- components: BP_TurretLauncher none added (never shown). BP_TurretBullet: ShotMesh (engine sphere, radius 6 uu, scale 0.12, attached to the root CollisionCapsule at (0, 0, 0), M_PlayerLaserShot green since task-464-02 (commit c4620feb, [[bp-player-laser-shot-material]]; was MI_GB_Shot white-gold), NoCollision; ImpactSparkSystem NS_ShotImpactSparkGreen (same commit, [[bp-shot-impact-spark]]); task-412-01/02: the root is now a capsule 10 x 10 uu like the old sphere, read back and recompiled, no asset change). Turret shots are not affected by the Muzzle rule: the turret overrides `GetShotOrigin` and fires from its own Muzzle, no sweep ([[sys-turret]], [[sys-weapons]]). BP_Turret (task-350-03; scale S = 96 / 156.144 = 0.6148, the old grey-box head top of 96 uu; scaled head top reads 95.99; all meshes NoCollision, overlap events off, no cast-shadow flag: `RRShadows::DisableCastShadows` at runtime): TurretBaseMesh (SM_TurretBase, under Root, loc 0, scale S); HeadPivot (C++, under Root, loc (0.308, -0.318, 40.085) = S x (0.501, -0.518, 65.20), the head offset with Y mirrored); TurretHeadMesh (SM_TurretHead, under HeadPivot, loc 0, rot 0, scale S); Muzzle (C++, under HeadPivot, loc (58.79, 0.325, 18.70) = S x (95.628, 0.528, 30.416): X = canon tip at the head bound, Y/Z = canon axis from `canon_axis_yz_cm_in_head_frame` (-0.528, 30.416) in `turret_cut.report.json`, Y mirrored); MuzzleFlash (under Muzzle, loc 0, scale 0.3, sphere r 15 uu, MI_GB_Shot, hidden; unused since task-464-04, never shown). Muzzle world height 58.8 uu against ShotHeight 50: the flash sits about 8.8 uu above the bullet start. Before: TurretBase, TurretBody, TurretHead (z 69, scale 0.54), TurretBarrel and MuzzleFlash (67.5, 0, 69) at Root, grey box and cylinders, all NoCollision.
- exposed_defaults: BP_TurretLauncher TurretClass = BP_Turret_C, ProjectileClass = BP_TurretBullet_C; BP_Turret ProjectileClass = BP_TurretBullet_C (the launcher copy wins at placement); other C++ defaults kept.
- events: BP_Turret EventTick deleted (task-350-03, 25 nodes: head yaw and barrel/flash orbit; the C++ HeadPivot now turns the head, [[sys-turret]]). BeginPlay and the overlap event untouched. OnTurretFired(Origin, Direction) (task-464-04, commit bfa37e24): Niagara SpawnSystemAttached(NS_MuzzleFlash, Muzzle, KeepRelativeOffset, bAutoDestroy true), then SetRelativeScale3D (0.6, 0.6, 0.6) on the returned component (the turret mesh scale, provisional): the green rifle flash at the barrel tip, along the barrel ([[bp-muzzle-flash]]). Before: MuzzleFlash sphere visible, Delay 0.05 s, hidden. MuzzleFlash was re-parented under Muzzle with `ActorTools.set_parent_component` (this tool re-parents; the older note that MCP cannot re-parent components does not hold for it).
- logic_in_bp: the muzzle flash only (spawn the Niagara flash); no tick logic.
- assets: SM_TurretBase and SM_TurretHead with MI_TurretBody and MI_TurretCanon ([[bp-turret-materials]]); NS_MuzzleFlash for the flash, M_PlayerLaserShot and NS_ShotImpactSparkGreen for the bullet; MI_GB_Shot only on the unused sphere. The grey-box MI_GB_TurretWhite, MI_GB_TurretTeal and MI_GB_TurretRust are no longer used by BP_Turret.
- owner_agent: agent-bp
- verified: task-350-03: compiled and saved; components, transforms and slots as listed read back by the Blueprint agent (slot materials read back in task-350-02, not again in 03). task-350-04: -game capture (frames in `Saved/TurretInGame/after/`, not committed): the base stays fixed, the head turns toward two different robots, the flash is a large blur; 8 tests pass on the final Blueprint (Turret.* x7 and Integration.PlaceTurretInputKillsRobot). Not verified: exact canon-tip shot start (flash blurred), on-screen size against the grey box, no shadow and no collision in frames, canon axis height 18.7 uu on the mesh. Earlier (task-032-02 and before): PIE firing checked, colours unchecked.

## Relations
- [[blueprints-index]] — parent index.
- [[bp-turret-materials]] — the turret textures, material, instances and meshes.
- [[sys-turret]] — the C++ classes and placement rules.
- [[turret]] — the weapon design.
- [[bp-shotgun]] — sibling weapon Blueprint and the pellet.
- [[bp-miner]] — lists the launcher in its WeaponCatalogue.
- [[bp-muzzle-flash]] — the green flash its OnTurretFired spawns.
- [[bp-player-laser-shot-material]] — the green material of its bullet.
- [[bp-shot-impact-spark]] — the green sparks of its bullet.

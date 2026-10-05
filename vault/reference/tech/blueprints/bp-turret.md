---
id: bp-turret
description: "BP_TurretLauncher, BP_Turret and BP_TurretBullet, Blueprints of the turret launcher, the placed turret and its bullet: grey-box look, barrel tracking, muzzle flash."
parent: blueprints-index
kind: doc
---
# BP_TurretLauncher, BP_Turret, BP_TurretBullet

- asset_path: /Game/RogueRigs/Blueprints/Weapons/BP_TurretLauncher, BP_Turret, BP_TurretBullet (also BP_ShotgunPellet, see [[bp-shotgun]])
- parent_class: ARRTurretLauncher (/Script/RogueRigs.RRTurretLauncher), ARRTurret (/Script/RogueRigs.RRTurret), ARRTurretBullet (/Script/RogueRigs.RRTurretBullet)
- purpose: the visible turret weapon; looks only, charges, placement, targeting and fire are C++ ([[sys-turret]]).
- components: BP_TurretLauncher none added (never shown). BP_TurretBullet: ShotMesh (engine sphere, radius 6 uu, MI_GB_Shot white-gold, NoCollision). BP_Turret, all NoCollision, no shadow: TurretBase (cylinder r 28, h 12, rust grey), TurretBody (cube 36 x 36 x 24 at z 24, white), TurretHead (sphere r 18 at z 46, teal), TurretBarrel (cylinder r 5, h 40, pitch 90, rust grey), MuzzleFlash (sphere r 10, white-gold, hidden at rest).
- exposed_defaults: BP_TurretLauncher TurretClass = BP_Turret_C, ProjectileClass = BP_TurretBullet_C; BP_Turret ProjectileClass = BP_TurretBullet_C (the launcher copy wins at placement); other C++ defaults kept.
- events: BP_Turret EventTick: when GetTarget is valid, yaw toward it (FindLookAtRotation), head turned, barrel and muzzle flash moved on a circle around the head (SetWorldLocationAndRotation; components cannot be re-parented through MCP, so the barrel is repositioned instead of attached). OnTurretFired(Origin, Direction): MuzzleFlash visible for 0.05 s.
- logic_in_bp: visual only (barrel orbit and flash).
- assets: /Game/RogueRigs/Materials/GreyBox/MI_GB_TurretWhite (0.9), MI_GB_TurretTeal (0, 0.6, 0.55), MI_GB_TurretRust (0.32, 0.24, 0.2), all instances of M_GreyBox; MI_GB_Shot reused.
- owner_agent: agent-bp
- verified: compiled (warnings as errors), saved. PIE on L_ArenaM1: the launcher came from a new-weapon card (BP_TurretLauncher_C_0 in the miner), key E placed BP_Turret_C_0, which fired (ShotCount 14, BP_TurretBullet actors alive). Not viewed on screen (no screenshot allowed): barrel tracking and colours unchecked visually.

## Relations
- [[blueprints-index]] — parent index.
- [[sys-turret]] — the C++ classes and placement rules.
- [[turret]] — the weapon design.
- [[bp-shotgun]] — sibling weapon Blueprint and the pellet.
- [[bp-miner]] — lists the launcher in its WeaponCatalogue.

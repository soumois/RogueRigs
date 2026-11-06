---
id: sys-aim-indicator
description: "Miner aim laser component: Plane mesh, Muzzle origin, uncapped length ending on first robot, rock or barrel, sharp tip, tuning. Open to change it."
parent: systems-combat-index
kind: doc
---
# Aim indicator

- responsibility: draw the [[aim-indicator]]: a thin white-gold laser line from the shown hand weapon Muzzle along the aim, horizontal at Muzzle height, with no length cap, stopping at the first robot, rock or barrel (task-468). Pure visual. Replaced the ground chevrons in task-415 (decided by the owner 2026-11-02, "Ok, tout par défaut"; values provisional). No turret laser, no reticle.
- main_classes: `Source/RogueRigs/AimIndicator/RRAimIndicatorComponent.h/.cpp`: `URRAimIndicatorComponent` (a `USceneComponent`, Config=Game, Blueprint-spawnable; commits 2b0f9193, 33158869). Default subobject `AimIndicator` of `ARRMiner`; `ARRMiner::GetAimIndicator()` kept. At `BeginPlay` it creates a transient `UStaticMeshComponent` child `AimLaserMesh` (engine Plane, no collision, shadow, decals or tick, absolute transform), with a dynamic instance of [[bp-aim-laser-material]].
- entry_points: `RefreshIndicator()`, `HideIndicator()`, `GetIndicatorMaterialInstance()`, `IsIndicatorShown()` (BlueprintPure), `GetLaserMesh()`, `GetTraceReach()` (was `GetMaxLength()`), `GetLaserLength()`, `GetLaserOrigin()`.
- placement: ticks at `TG_PostPhysics`. Origin = Muzzle of the shown hand weapon (`FindHandWeapon()->GetMuzzle()`), fallback the first weapon's `GetShotOrigin()` ([[sys-weapons]]). Direction = `ARRMiner::GetAimDirection` flattened to the horizontal at Muzzle height, so the line never enters the floor. Plane centre = Muzzle + aim x length / 2, local +X along the aim, scale (length / 100, Width / 100, 1); plane UV.x = (X + 50) / 100, 0 at the Muzzle.
- length (task-468, provisional, owner 2026-11-06; commit 0892d20c): no cap. Nearer of `ARRArenaGrid::GetClearDistance` (first block cell on the aim line: rock, ore, heal ore, special ore or barrel; the arena layouts are closed by rock, so the border walls stop it, [[sys-grid]]) and the first living robot hit by a vertical capsule sweep of radius `RobotSweepRadius` 9.5, half height = the hand weapon shot capsule half height, Pawn objects, miner ignored. Only `GetHitCapsule` of robots that are neither dead nor death-pending counts (shield chaser included: its shield is an attribute, not a collider); pickups, turrets, drone, console and extractor are not block cells nor robot capsules and never stop it. Both searches run over `TraceReach` 10000 uu, a search bound past the screen edge (camera arm 1400 uu, FOV 75), not a visible limit: with nothing in the way the line runs off screen. A robot overlapping the Muzzle gives length 0. Was capped at `MaxLength` 600 uu ([[task-423-miner-aim-laser-range-fade]]).
- hiding: hidden when the owner is not a living miner, the mesh or material is missing (one warning), there is no weapon, or the length is 0. `ARRMiner::HandleDeath` calls `HideIndicator()`. Same for mouse and stick aim.
- parameters (`EditDefaultsOnly` on the component): `DefaultMeshPath` /Engine/BasicShapes/Plane.Plane, `DefaultMaterialPath` /Game/RogueRigs/FX/M_AimLaser.M_AimLaser (Config), `LaserMeshSize` 100, `TraceReach` 10000 uu, `Width` 19 uu (core 3 uu plus about 8 uu glow each side), `RobotSweepRadius` 9.5, and the other material parameters with the defaults of [[bp-aim-laser-material]].
- tip (task-468, provisional, owner 2026-11-06): no fade, the line ends sharp on what stops it. `ApplyParameters` at `BeginPlay` pushes `TipFadeFrac` 0 once; M_AimLaser divides by max(TipFadeFrac, 0.0001), so the whole length stays opaque. The task-423 fixed 150 uu fade (`TipFadeLength`, `ComputeTipFadeFrac`, `UpdateTipFade`, `GetTipFadeLength`, `GetPushedTipFadeFrac`) is removed. The far part still darkens with the light mask ([[aim-indicator]]).
- debug_aim (task-423, commit b12776b6): console command `rr.Debug.Aim <YawDegrees|off>` (ECVF_Cheat, not in shipping; world yaw 0 = +X, 90 = +Y) fixes the miner aim, ignoring mouse and stick, until `off` or no argument; for visual checks only. On `ARRMiner`: protected `bDebugAimOverride`, `DebugAimDirection`; public `SetDebugAimOverride(float Yaw)`, `ClearDebugAimOverride()`, `HasDebugAimOverride()`. Tick sets the aim and skips the mouse update, `ApplyStickAimInput` returns early, `GetMouseAimPoint` returns false while on. Command in `Debug/RRVisualCheckCommands.cpp`; see [[tool-capture]].
- where to tune: search reach, width and sweep radius in the component header or `BP_Miner` `AimIndicator` defaults (`BP_Miner` overrides none); look (colour, core, glow) through the component parameters, which are pushed to the material.
- removed: `M_AimChevrons` (deleted in 8326a0a8) and the chevron decal code, including the decal warm-up use.
- depends_on: [[sys-player]] (miner, aim direction, death), [[sys-weapons]] (Muzzle), [[sys-grid]] (`GetClearDistance`).
- implements: [[aim-indicator]].
- gotchas:
  - Deriving the component from `UStaticMeshComponent` crashed `BP_Miner` load with "Serial size mismatch 54 vs 46": the Blueprint had serialized the subobject as a decal. Changing the base class of a default subobject a Blueprint already saved can crash its loading; hence the `USceneComponent` plus a runtime child mesh.
  - Translucent may hide the line under muzzle flashes; switching M_AimLaser to Additive is one blendMode change.
- unverified: task-468 (no capture, owner checks in game): the line running off screen, its sharp end on border walls, blocks, barrels and robots, and that a Muzzle above `BlockHeight` 100 never shows the line over a wall top are not seen in a frame; build only, no test covers the component. Earlier: the task-415 capture subtask was closed on the owner's validation (2026-11-02, "Ok, je valide ce que j'ai vu"). task-423 captures: the laser ends about 600 uu on open floor and fades to nothing at the maximum range, at a rock block and at a robot; colour unchanged white-gold. A headless run of 276 tests passed, including the 4 earlier listed as failing (Integration.JuiceKillDropsStreamingCubes, Integration.LevelUpUpgradeLoop, Shotgun.Knockback, Turret.FireRangeLifetime).

## Relations
- [[systems-combat-index]] — parent index.
- [[aim-indicator]] — the GDD feature it implements.
- [[bp-aim-laser-material]] — the laser material and its parameters.
- [[sys-player]] — the miner that owns the component and hides it at death.
- [[sys-weapons]] — the Muzzle the laser starts at.
- [[sys-grid]] — block cell query that stops the laser.
- [[tool-capture]] — captures of the laser use `rr.Debug.Aim`.
- [[task-423-miner-aim-laser-range-fade]] — task that set the 600 uu range, the fixed tip fade and `rr.Debug.Aim`.
- [[task-468-aim-laser-unlimited-length]] — task that removed the range cap and the tip fade.

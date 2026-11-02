---
id: sys-aim-indicator
description: "Miner aim laser component: Plane mesh, Muzzle origin, stop at first robot or rock, hiding, tuning, chevron removal. Open to change the aim indicator."
parent: systems-combat-index
kind: doc
---
# Aim indicator

- responsibility: draw the [[aim-indicator]]: a thin white-gold laser line from the shown hand weapon Muzzle along the aim, horizontal at Muzzle height, stopping at the first robot or rock block. Pure visual. Replaced the ground chevrons in task-415 (decided by the owner 2026-11-02, "Ok, tout par défaut"; values provisional). No turret laser, no reticle.
- main_classes: `Source/RogueRigs/AimIndicator/RRAimIndicatorComponent.h/.cpp`: `URRAimIndicatorComponent` (a `USceneComponent`, Config=Game, Blueprint-spawnable; commits 2b0f9193, 33158869). Default subobject `AimIndicator` of `ARRMiner`; `ARRMiner::GetAimIndicator()` kept. At `BeginPlay` it creates a transient `UStaticMeshComponent` child `AimLaserMesh` (engine Plane, no collision, shadow, decals or tick, absolute transform), with a dynamic instance of [[bp-aim-laser-material]].
- entry_points: `RefreshIndicator()`, `HideIndicator()`, `GetIndicatorMaterialInstance()`, `IsIndicatorShown()` (BlueprintPure), `GetLaserMesh()`, `GetMaxLength()`, `GetLaserLength()`, `GetLaserOrigin()`.
- placement: ticks at `TG_PostPhysics`. Origin = Muzzle of the shown hand weapon (`FindHandWeapon()->GetMuzzle()`), fallback the first weapon's `GetShotOrigin()` ([[sys-weapons]]). Direction = `ARRMiner::GetAimDirection` flattened to the horizontal at Muzzle height, so the line never enters the floor. Plane centre = Muzzle + aim x length / 2, local +X along the aim, scale (length / 100, Width / 100, 1); plane UV.x = (X + 50) / 100, 0 at the Muzzle.
- length: min of `MaxLength` 1200 uu, `ARRArenaGrid::GetClearDistance` (first block cell on the aim line, [[sys-grid]]), and the first living robot hit by a vertical capsule sweep of radius `RobotSweepRadius` 9.5, half height = the hand weapon shot capsule half height, Pawn objects, miner ignored. Only `GetHitCapsule` of robots that are neither dead nor death-pending counts; a robot overlapping the Muzzle gives length 0.
- hiding: hidden when the owner is not a living miner, the mesh or material is missing (one warning), there is no weapon, or the length is 0. `ARRMiner::HandleDeath` calls `HideIndicator()`. Same for mouse and stick aim.
- parameters (`EditDefaultsOnly` on the component): `DefaultMeshPath` /Engine/BasicShapes/Plane.Plane, `DefaultMaterialPath` /Game/RogueRigs/FX/M_AimLaser.M_AimLaser (Config), `LaserMeshSize` 100, `MaxLength` 1200, `Width` 19 uu (core 3 uu plus about 8 uu glow each side), `RobotSweepRadius` 9.5, and the six material parameters with the defaults of [[bp-aim-laser-material]].
- where to tune: length, width and sweep radius in the component header or `BP_Miner` `AimIndicator` defaults; look (colour, core, glow, tip fade) through the component parameters, which are pushed to the material.
- removed: `M_AimChevrons` (deleted in 8326a0a8) and the chevron decal code, including the decal warm-up use.
- depends_on: [[sys-player]] (miner, aim direction, death), [[sys-weapons]] (Muzzle), [[sys-grid]] (`GetClearDistance`).
- implements: [[aim-indicator]].
- gotchas:
  - Deriving the component from `UStaticMeshComponent` crashed `BP_Miner` load with "Serial size mismatch 54 vs 46": the Blueprint had serialized the subobject as a decal. Changing the base class of a default subobject a Blueprint already saved can crash its loading; hence the `USceneComponent` plus a runtime child mesh.
  - Translucent may hide the line under muzzle flashes; switching M_AimLaser to Additive is one blendMode change.
- unverified: the capture subtask was closed on the owner's validation (2026-11-02, "Ok, je valide ce que j'ai vu"); 4 tests fail with and without the change (Integration.JuiceKillDropsStreamingCubes, Integration.LevelUpUpgradeLoop, Shotgun.Knockback, Turret.FireRangeLifetime).

## Relations
- [[systems-combat-index]] — parent index.
- [[aim-indicator]] — the GDD feature it implements.
- [[bp-aim-laser-material]] — the laser material and its parameters.
- [[sys-player]] — the miner that owns the component and hides it at death.
- [[sys-weapons]] — the Muzzle the laser starts at.
- [[sys-grid]] — block cell query that stops the laser.

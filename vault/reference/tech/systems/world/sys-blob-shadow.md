---
id: sys-blob-shadow
description: "Actor shadow component (task-264, task-472): light-independent purple-grey disc under miner, drone, robots, turrets, barrels, chest; per-type and global opacity and scale."
parent: systems-world-index
kind: doc
---
# Actor shadow (blob shadow)

- responsibility: the shadow under actors that ignores every light (owner 2026-11-05, [[visual-style]] actor shadows; [[task-472-actor-blob-shadows]] replaced the task-264 discs). One `URRBlobShadowComponent` per owner, no tick of its own.
- main_classes: `URRBlobShadowComponent : UStaticMeshComponent` and `URRBlobShadowSettings : UDeveloperSettings` (`Source/RogueRigs/World/RRBlobShadowComponent.h/.cpp`; the module links `DeveloperSettings` since task-472-01). Mesh `/Engine/BasicShapes/Plane`, shared material `MI_BlobShadow` ([[bp-blob-shadow-material]]), no dynamic instance. Fixed setup: no collision, no navigation, `CastShadow` false, no decals, distance fields, indirect lighting, ray tracing or captures, `TranslucencySortPriority` -10, absolute rotation zero (fitted owners: relative).
- settings (start values owner 2026-11-06 Q3, provisional): per type on the component, `Opacity` 0.6 and `SizeScale` 1.2 (Blueprint defaults of each owner's `BlobShadow`); `FloorOffset` 3 uu. Global: Project Settings > Game > Actor Shadows, `GlobalOpacityMultiplier` 1 and `GlobalScaleMultiplier` 1 (`DefaultGame.ini [/Script/RogueRigs.RRBlobShadowSettings]`), read when a shadow is sized (spawn, grid build).
- size and opacity: disc = footprint x `SizeScale` x global scale; opacity = `Opacity` x global, clamped 0..1, written to custom primitive data 0 (`OpacityDataIndex`), read by the material.
- entry_points: `SetFootprintDiameter` / `SetFootprintSize` (a zero footprint hides it), `SetShadowOpacity`, `SetShadowScale`, `RefreshFromSettings`, `PlaceUnderCapsule` (attached, once), `PlaceOnFloor` (hovering owner, each tick), `FitToOwnerMeshes` (static owner: union of its visible mesh bounds in actor space, centred at their bottom, follows the owner yaw).
- owners:
  - robots (`ARREnemy`): footprint = own capsule diameter; hidden at death, shown again on pool reuse ([[sys-enemies-base]]).
  - miner: footprint = capsule diameter; the i-frames blink leaves it alone ([[sys-player-classes]]).
  - drone: `BlobShadowFootprint` 70 uu, on the miner's floor under the drone, same size at every height ([[sys-drone-parts]]).
  - turret (`ARRTurret`) and bonus chest (`ARRBonusChest`, not consoles): `FitToOwnerMeshes` at BeginPlay; the chest casts no light shadow (`RRShadows::DisableCastShadows`).
  - barrels: one component per barrel cell built by `ARRArenaGrid::BuildBarrelShadows` (`BarrelShadowOpacity` 0.6, `BarrelShadowScale` 1.2 on the grid, footprint = drawn barrel width), destroyed with the barrel ([[sys-grid]]).
- data: no table.
- depends_on: [[bp-blob-shadow-material]].
- implements: actor shadows of [[visual-style]].
- tests: `RogueRigs.BlobShadow.*` (settings, under miner and robot and follow, one per barrel removed with it).
- gotchas:
  - Cost: one translucent draw per owner, as since task-264 ([[perf-lead-blob-shadow-instancing]]); frame time with a full wave not measured in task-472.
  - The blend follows the floor under it: brighter in the light pool, the tint itself never changes.
  - A chest or barrel fills most of its footprint: at 1.2 only a thin rim shows (task-472-03, owner to judge).
  - A zero `SizeScale` or global scale keeps the last disc size; use opacity 0 to hide.

## Relations
- [[systems-world-index]] — parent index.
- [[bp-blob-shadow-material]] — the material and its parameters.
- [[visual-style]] — the actor shadow rule.
- [[task-472-actor-blob-shadows]] — the task that made it light-independent and tunable.
- [[lights-no-shadows]] — history of the first fake shadow.
- [[perf-lead-blob-shadow-instancing]] — one instanced component per world instead of one per actor.
- [[sys-enemies-base]] — the robot owner.
- [[sys-drone-parts]] — the drone owner.
- [[sys-player-classes]] — the miner owner.
- [[sys-grid]] — builds the barrel shadows.
- [[bp-robot-readability]] — robot Blueprints carrying it.
- [[bp-miner]] — miner Blueprint carrying it.
- [[bp-turret]] — turret Blueprint carrying it.
- [[bp-bonus-chest]] — chest Blueprint carrying it.

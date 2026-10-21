---
id: sys-blob-shadow
description: "Blob shadow component (task-264-02): flat dark disc under robots, miner and drone; placement, sizing, hiding, render flags, cost. Open to tune or reuse it."
parent: systems-world-index
kind: doc
---
# Blob shadow

- responsibility: the fake round shadow under actors (owner 2026-10-20: only the player light casts real shadows, [[lights-no-shadows]]). One `URRBlobShadowComponent` per owner, created in the owner's constructor (never re-created on pool reuse), no tick of its own.
- main_classes: `URRBlobShadowComponent : UStaticMeshComponent` (`Source/RogueRigs/World/RRBlobShadowComponent.h/.cpp`). Constructor: mesh `/Engine/BasicShapes/Plane` and material `/Game/RogueRigs/FX/BlobShadow/MI_BlobShadow` ([[bp-blob-shadow-material]]) through `ConstructorHelpers::FObjectFinder` (missing asset = empty slot, nothing drawn); one shared instance, no dynamic material. Fixed setup: `NoCollision`, no overlap events, no navigation, `CastShadow` false, `bReceivesDecals` false, `bAffectDistanceFieldLighting` false, `bAffectDynamicIndirectLighting` false, `bUseAsOccluder` false, not in ray tracing, reflection or sky captures, `TranslucencySortPriority` -10, absolute rotation zero (never tilts or turns). `FloorOffset` (EditAnywhere, 3 uu, provisional) above the floor.
- entry_points: `SetFootprintDiameter(D)` (scale X = Y = D / 100; D <= 0 hides it), `PlaceUnderCapsule(HalfHeight)` (attached owners, once: relative Z = FloorOffset − half height, the capsule bottom, same floor convention as the debris and XP drop point of [[sys-enemies-base]]), `PlaceOnFloor(OwnerLocation, FloorZ, SizeScale)` (hovering owner, absolute location, skips unchanged updates).
- owners:
  - robots (`ARREnemy`, every subclass): `BlobShadow` on the root capsule, diameter = own capsule radius (before the terrain shrink, unscaled) × `BlobShadowDiameterScale` (`Enemy|Blob Shadow`, 2.2): heavy 48 → 106 uu, shooter 34 → 75 uu, chaser 24 → 53 uu (radii from [[sys-enemies-base]]; kamikaze and shield chaser from their Blueprint capsules). Hidden in `HandleDeath` (the puff covers the shrink), shown again by `RestoreVisualParts` on pool reuse; skipped by the wind-up flash (`SetMeshesVisible`). Parked robots are hidden with the actor (`SetActorHiddenInGame`, [[sys-actor-pool]]).
  - miner (`ARRMiner`): `BlobShadow` on the capsule, diameter = capsule radius × `BlobShadowDiameterScale` (`Miner|Blob Shadow`, 2.2); the i-frames blink (mesh and its children) leaves it alone ([[sys-player-classes]]).
  - drone (`ARRDrone`): `BlobShadow` attached to `Mesh` with absolute location, rotation and scale, placed by `UpdateBlobShadow` at the end of the drone's own Tick on the miner's floor (capsule bottom) under the drone; `BlobShadowDiameter` 70 uu, shrunk with height ([[sys-drone-parts]]).
- data: no table; defaults above, all provisional.
- depends_on: [[bp-blob-shadow-material]].
- implements: the blob shadow line of [[lights-no-shadows]].
- gotchas:
  - Cost: one more attached primitive per robot. Game thread: its transform follows every root move (the child-transform cost of [[perf-robot-components-audit]], about 1/5 of a robot's parts). Render thread: one translucent mesh draw and one proxy transform update per moving robot; translucent draws are sorted, not merged, so 400 robots add about 400 small draws (unmeasured). The cheaper alternative is [[perf-lead-blob-shadow-instancing]].
  - The disc is translucent: it darkens a ground telegraph decal drawn under it.
  - Not checked on screen: draw order, falloff, size per robot (no capture taken).

## Relations
- [[systems-world-index]] — parent index.
- [[bp-blob-shadow-material]] — the material and its parameters.
- [[lights-no-shadows]] — why the fake shadow exists.
- [[perf-lead-blob-shadow-instancing]] — one instanced component per world instead of one per actor.
- [[sys-enemies-base]] — the robot owner.
- [[sys-drone-parts]] — the drone owner.
- [[sys-player-classes]] — the miner owner.

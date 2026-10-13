---
id: sys-ground-telegraph
description: "Ground telegraph decal component: API, material parameter names and ranges for M_GroundTelegraph, mode colours, hide on death and pool release, no decals on actors."
parent: systems-index
kind: doc
---
# Ground telegraph

- responsibility: draw the [[ground-telegraph]] signal: a decal projected straight down (follows the floor and the top of rocks) whose material shows a zone circle and a progress circle growing to its edge. Pure visual: no gameplay rule, no collision.
- main_classes: `Source/RogueRigs/Telegraph/`: `URRGroundTelegraphComponent` (`RRGroundTelegraphComponent.h`, derives from `UDecalComponent`, Blueprint-spawnable), enum `ERRGroundTelegraphMode` (Danger, ZoneBlocked, ZoneActive).
- entry_points (owner usage): add the component to the owner (Blueprint or C++, attached under the root); call `SetRadius(uu)`, `SetMode`, then `Show()`; drive `SetProgress(0..1)` each tick of the hack, fuse or slam wind-up; `Hide()` when done. `SetOpacity(0..1)`. `SetTelegraphMaterial(Material)` replaces the material (stored values are reapplied). `Show()` returns false and stays hidden while the owner is gone. Statics: `HideAllOn(Actor)`, `DisableDecalsOn(Actor)`.
- material contract (built by task-112-02, deferred decal domain): dynamic instance parameters, names exact:
  - `Radius` (scalar, uu, ≥ 0): zone radius; the decal box half size equals it, so the zone circle is at the UV edge (UV distance 0.5 from the centre); use it to keep dot spacing and line width constant in uu.
  - `Progress` (scalar, 0..1): radius of the progress circle as a fraction of the zone radius; 1 = touches the zone circle.
  - `ZoneColor` (vector, linear RGB): zone circle colour from the mode.
  - `ProgressColor` (vector, linear RGB): progress circle colour, white by default.
  - `Opacity` (scalar, 0..1): global opacity of both circles.
  - Look ([[ground-telegraph]]): sharp lines, no diffuse disc; zone circle dotted, progress circle solid.
- data (component defaults, `EditDefaultsOnly`): `DefaultMaterialPath` (config `[/Script/RogueRigs.RRGroundTelegraphComponent]`, default `/Game/RogueRigs/FX/M_GroundTelegraph`; used when `DecalMaterial` is not set on the component); `DangerColor` (1.0, 0.45, 0.05), `ZoneBlockedColor` (1.0, 0.8, 0.2), `ZoneActiveColor` (0.7, 0.9, 1.0), `ProgressColor` white; `ProjectionHalfDepth` 256 uu (box above and below the component). Starts hidden, `FadeScreenSize` 0, absolute rotation pitch −90.
- users: `ARRArenaGrid` barrel fuse (task-114-01: pooled components on the grid, one per fusing barrel, at the cell centre: Danger, radius = Barrel explosion radius 700, progress = elapsed / 1.5 s fuse, hidden and returned to the pool at the explosion or rebuild; [[sys-grid]]); `ARRConsole` and `ARRBonusChest` hack zones (task-113-01, `GroundTelegraph` default subobject: radius = zone radius, ZoneBlocked / ZoneActive from the marked rocks, progress = hack progress, hidden at completion; [[sys-objectives]]); `ARRHeavy` slam (task-115-01, `GroundTelegraph` default subobject at the feet: Danger, radius = slam radius, progress = telegraph progress, shown only during Telegraph, progress reset on pool reuse; [[sys-enemies-heavy]]).
- hiding: `ARREnemy::HandleDeath` (every death, pending kamikaze fuse included) and `ARREnemy::OnPoolReleased` call `HideAllOn`; `ARRMiner::HandleDeath` too. While shown, the component ticks and hides when `IsOwnerGone` (owner invalid, being destroyed, `IsPooledIdle`, robot `IsDead` / `IsDeathPending`, miner `IsDead`): covers barrels, consoles and any other owner. Reuse from the pool leaves it hidden until the owner shows it. Destroyed with its owner (a component).
- no decals on actors: `DisableDecalsOn` turns `bReceivesDecals` off on every primitive: `ARREnemy::PostInitializeComponents` (every robot, Blueprint meshes included), `ARRMiner::BeginPlay` and each carried weapon in `ARRMiner` weapon attach.
- depends_on: [[sys-actor-pool]] (`IsPooledIdle`), [[sys-enemies]], [[sys-player]].
- implements: [[ground-telegraph]], [[readability-rules]] (ground circles).
- gotchas: without the material asset, nothing draws, values are kept and one warning is logged per run (the missing package is checked once, not retried). A decal is a scene component, not a primitive: no collision, overlap or navigation by construction. Weapons acquired later than the miner attach path are not covered. Whether the Danger zone circle is dotted is open (GDD says dotted for zone circles).

## Relations
- [[systems-index]] — parent index.
- [[ground-telegraph]] — the GDD signal it draws.
- [[readability-rules]] — colour meanings.
- [[sys-actor-pool]] — pool release hiding.
- [[sys-enemies-heavy]] — the heavy slam, a Danger user.
- [[bp-explosion-fx]] — the other ground effect.
- [[sys-automation-tests]] — RogueRigs.GroundTelegraph.* tests.
- [[sys-objectives]] — console and chest hack zone, the first user.
- [[sys-grid]] — barrel fuse zones hosted by the arena grid.

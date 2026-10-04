---
id: code-review-m1-systems
description: "M1 review findings F8 to F12: CSV loading, damage numbers, arena grid size, objective typing, miner and robot shared plumbing."
parent: code-review-m1
kind: doc
---
# Review findings — data, grid, objectives

Continues [[code-review-m1-gameplay]] (F1 to F7); actions in [[code-review-m1-plan]].

## Data
- F8 load-CSV-find-row-log copied in `ARREnemy::LoadStatsRow`, `ARRWeapon::LoadStatsRow`, `FRRBaseStatsLoader::LoadPlayerBaseStats`; `URRExplosionSubsystem::GetExplosionStats` has its own cache. `EnemyStats.csv` is read from disk and parsed on every robot spawn (known gotcha in [[sys-enemies]]). The loader sits in `GAS/` though it is generic.

## Damage numbers
- F9 `ARREnemy::SpawnDamageText` and `ARRTargetDummy::SpawnDamageText` are identical, with the same 8 `Enemy|Damage Text` properties on both. `ARRFloatingDamageText` ships in gameplay but lives in `Debug/`.

## Grid (`RRArenaGrid.cpp` 995, `.h` 581)
- F10 one class does JSON layout parsing (`LoadLayout` 96), instancing (`BuildInstances` 111), terrain health, drops, barrels, marking and shake. Five block types x mesh, material, health, render and collision components = 25 parallel members plus switches. A grid generator needs `LoadLayout` split from apply; the per-type table is Blueprint-heavy (BP_ArenaGrid overrides the meshes).

## Objectives and HUD
- F11 `URRRunFlowSubsystem`, `URRHUDWidget::BindToRunFlow` and the objective delegates are typed `ARRConsole*`. Zone, drone link and progress live in `ARRConsole`. A bonus chest ([[objective-variants]]) would copy them. Fine until a second objective exists.

## Shared pawn plumbing
- F12 `ARRMiner` and `ARREnemy` repeat ASC init, MoveSpeed mirroring, health-set bind and unbind, `IsDead` (about 60 lines each).

## Relations
- [[code-review-m1]] — parent index.
- [[code-review-m1-gameplay]] — findings F1 to F7.
- [[sys-grid]] — arena grid as built.
- [[sys-objectives]] — run flow and console as built.

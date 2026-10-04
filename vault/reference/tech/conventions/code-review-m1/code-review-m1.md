---
id: code-review-m1
description: "M1 C++ review index: overall verdict, what is already good, and links to findings, test hooks, dead code and the ranked refactor plan."
parent: conventions-index
kind: note
---
# C++ code review — M1 (task-020-01)

Scope: all of `Source/RogueRigs/` after task-019-01: 16 system folders, about 15,500 lines of gameplay C++, 101 automation tests in 9,400 lines, plus 1,875 lines of template code in `Variant_TwinStick/`. No code was changed.

## Verdict
Readable, consistent and well commented; under-generalized at the robot and query level. A new human reader finds the header comments complete (every class, function and member, per [[cpp-standard]]), functions short (7 over 60 lines: `ARRArenaGrid::BuildInstances` 111, `LoadLayout` 96, `GetClearDistance` 65, `URRFlowFieldSubsystem::ComputeField` 84, `URRStressSubsystem::SpawnStressEnemies` 79, `ARREnemySpawner::TrySpawnPack` 63, `URRExplosionSubsystem::DamageCharacters` 61), tunables in `UPROPERTY` or CSV, almost no magic numbers, logging on `LogRogueRigs`, cheats isolated. What would hurt next: robot behavior helpers live in each subclass, world lookups and cell searches are copy-pasted, `ARRProjectile` cannot tell friend from foe, and the CSV loader is duplicated and uncached. The shooter, turrets and heavy robot would each copy these again.

## Already good (keep)
- Header comments describe behavior and units; class comments name the sys node.
- `ARREnemy` base already factors ASC, stats, death, XP drop, crowd and movement LOD; subclasses only implement `UpdateBehavior` (Chaser 153 lines, Kamikaze 189).
- `ARRWeapon::FireOnce` virtual plus `ApplyStatsRow` override pattern (shotgun 166 lines) scales to new weapons.
- `URRAutoFireComponent`, `URRUpgradeComponent`, `URRExplosionSubsystem` are self-contained, deterministic with seeds, and tested.
- Cheat front-end (`RRCheats`, `URRCheatSettings`, `URRCheatSubsystem`) is one folder, shipping-gated.
- No `private`, no `AddOnScreenDebugMessage`, includes are module-relative (except the template).

## How Blueprint impact was checked
Property names are plain ASCII in `.uasset` name tables: `grep -a -l <Name>` over `Content/` shows which reflected names a Blueprint overrides. Overridden today (rename needs a CoreRedirect or a re-set): `XPPickupClass`, `BodyMesh` (component in BP_Chaser and BP_Kamikaze), `CoreMesh`, `VisualRoot`, `OrePickupClass`, `RockMesh` and the other grid meshes, `DroneClass`, `StartingWeapons`, `ProjectileClass`, `StatsRowName` (BP_BurstRifle), `ConsoleClass`, `PickupType`, `EnemyClass`, `WeightedEnemyClasses`. Not overridden anywhere: `ArmRange`, `FuseTime`, `ArrivalRangeRatio`, `WindUpFlashRate`, every `Enemy|Damage Text` property, `DamageEffectClass`, `GuaranteedWeaponClasses`, `ShotHeight`. Moving a source file to another folder never needs a redirect (script path is `/Script/RogueRigs.<Class>`).

## Relations
- [[conventions-index]] — parent index.
- [[code-review-m1-gameplay]] — findings F1 to F7: robots, projectiles, weapons, world lookups, cell searches. Open to understand a refactor item.
- [[code-review-m1-systems]] — findings F8 to F12: CSV loading, damage numbers, grid, objectives, shared pawn plumbing.
- [[code-review-m1-hooks]] — test-only hooks and cheats inside gameplay classes, test-code readability, dead code, files over 800 lines.
- [[code-review-m1-plan]] — ranked refactor list R1 to R7 (gain, risk, hours, Blueprint impact) recommended for 020-02. Open first when executing 020-02.
- [[code-review-m1-deferred]] — R8 to R16, deferred with the trigger that should schedule each.
- [[cpp-standard]] — the rule the review measured against.

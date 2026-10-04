---
id: sys-gas-core
description: "GAS foundation: ASC layout per actor, AttributeSets and clamps, ability base class, runtime CSV base-stat loader, debug commands."
parent: systems-index
kind: doc
---
# GAS core

- responsibility: owns the ability system layout shared by every combat actor.
- main_classes (all in `Source/RogueRigs/GAS/`):
  - `URRAbilitySystemComponent`: one per player, weapon (turret launcher included), turret, enemy pawn (approved); none on projectiles, rock blocks or barrels ([[sys-grid]], [[sys-explosions]]). Enemy ASC on the pawn; no replication (single-player).
  - Rule: a value is an attribute only if an upgrade, status or effect changes it during a run; otherwise it is a CSV constant.
  - AttributeSets (naming provisional; attributes start at 0, base values from CSV):
    - `URRHealthAttributeSet` (player, enemies): `Health`, `MaxHealth`, meta `IncomingDamage` — implemented;
    - `URRMovementAttributeSet` (player, enemies): `MoveSpeed` (uu/s) — implemented;
    - `URRPlayerAttributeSet` (player): `PickupRadius`, `XPGain`, `MiningSpeed`, `Luck` — implemented;
    - `URRWeaponAttributeSet`: `Damage`, `FireRate`, `ReloadSpeed`, `CritChance`, `CritDamage` (base 1.5), `MagazineSize`, `Ammo`, `PierceCount` (M2) — not yet;
    - `URREnemyAttributeSet`: `AttackPower` (starts at 1, clamped >= 0; scales `URRGE_DamageFlat`) — implemented.
  - The drone has no ASC; it reads `MiningSpeed` on the player's ASC.
  - `URRGameplayAbility` (abstract): `InstancedPerActor` in its constructor ([[gas-engine-facts]]).
  - `FRRPlayerBaseStatsRow` (`FTableRowBase`) and `FRRBaseStatsLoader` (static helpers).
- entry_points: `InitAbilityActorInfo` at spawn or acquisition, then `FRRBaseStatsLoader::LoadAndApplyPlayerBaseStats(ASC, RowName)` once the three sets are on the ASC; GEs for every later stat change; `IncomingDamage` for damage.
- data: CSV read at runtime into a transient DataTable (`UDataTable::CreateTableFromCSVString`, approved); `Content/RogueRigs/Data/Tables/PlayerBaseStats.csv`, row `Miner`, values from [[player-stats]]. Generic helpers: `LoadTableFromCSV` (fresh parse), `GetCachedTable(Path, RowStruct)` (one rooted table per path, re-parsed when the file timestamp or row struct changes; game thread only) and `FindRow<T>(Path, RowName, OutRow, Context)` (logs `row '<Name>' not found in <path> (<Context>)` once). Player, enemy, weapon and explosion stats use `FindRow`; XP curve and upgrade tables use `GetCachedTable`. Curve Tables and hot reload in M2 (provisional).
- depends_on: plugin GameplayAbilities, modules `GameplayAbilities`, `GameplayTags`, `GameplayTasks` ([[task-004-01-enable-gas]]).
- implements: [[player-stats]], [[upgrades]], [[enemies-index]].
- gotchas:
  - Clamps: base in `PreAttributeBaseChange`, current in `PreAttributeChange`. `Health` in [0, `MaxHealth`]; `MaxHealth`, `IncomingDamage`, `MoveSpeed`, `PickupRadius`, `XPGain`, `MiningSpeed` >= 0; `Luck` unclamped. `Health` is lowered in `PostAttributeChange` when `MaxHealth` drops; it is not raised when `MaxHealth` grows.
  - Set `MaxHealth` before `Health` (the loader does).
  - `IncomingDamage` is consumed in `PostGameplayEffectExecute`: reset to 0, subtracted from `Health`, then `OnDamageReceived` is broadcast ([[sys-damage]]). Single death is detected here: `State.Dead` loose tag + `OnDeath`, unless `SetDeathEnabled(false)`.
  - Packaging: `Data/Tables` must be staged as a non-asset directory; `Config/DefaultGame.ini` stages `RogueRigs/Data` as non-UFS ([[task-004-03-miner-classes]]).
  - A missing SetByCaller returns 0 and logs an error.
  - Debug with native tools: `showdebug abilitysystem`, `AbilitySystem.Debug.NextCategory`, `AbilitySystem.DebugAttribute <attrs>`, `AbilityListGranted`, `EffectListActive`.
  - Cost at 150–200 enemies is expected in movement, animation, navmesh and rendering, not in GAS ([[perf-risks]]).

## Relations
- [[systems-index]] — parent index.
- [[gas-engine-facts]] — engine behaviors behind these rules.
- [[gas-api-notes]] — APIs used here (runtime CSV).
- [[naming]] — class and tag names.
- [[gas-scope]] — hours per milestone.
- [[sys-player]] — the miner, first ASC owner; follows the init order above.

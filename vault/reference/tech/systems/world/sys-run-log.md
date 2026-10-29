---
id: sys-run-log
description: "URRRunLogSubsystem: per-level run log counters (terrain, damage, kills, upgrades, damage taken, XP) fed by events; hub of the JSON writer. Open to change collected fields."
parent: systems-world-index
kind: doc
---
# Run log collector

- responsibility: collect the per-level counters of [[run-log-schema]] during play; the writer ([[sys-run-log-writer]], task-212-02) snapshots them at level end, adds the end-of-level fields and writes `Saved/RunLogs/<id>.json`.
- main_classes (`Source/RogueRigs/RunLog/`): `URRRunLogSubsystem` (`UGameInstanceSubsystem`, survives the map travel; counters cover the current level only); `FRRRunLevelStats` (USTRUCT snapshot, in `RRRunLogTypes.h`); debug command `RRRunLogDebugCommands.cpp` (not in shipping).
- entry_points:
  - Level start: `ARRMiner::BeginPlay` ends with `HandleMinerBeginPlay(Miner)`, after the run snapshot replay ([[sys-run]]), so replayed cards are not counted; first miner of a world only: counters reset, `LevelIndex` and `LayoutName` from `URRRunSubsystem`, `LevelStartWorldSeconds` = world time; binds the grid, the miner's upgrade and progression components. `URRRunTransitionSubsystem::OnLevelReady` ([[sys-run-transition]]) restamps the start time to the end of the loading hold. Restart and travel create a new world, hence a reset.
  - Reads: `GetCurrentLevelStats()` (BlueprintPure copy), `LogCurrentLevelStats(Context)`; static `MakeCardId`, `GetTerrainSourceName`, `GetCellTypeName`.
  - Debug: `rr.RunLog.Dump` logs every counter on `LogRogueRigs`.
- fields and hooks (events only, no polling):
  - `TerrainDestroyedBySource` (barrel, kamikaze, drone, shot, extractor) and `TerrainDestroyedByType` (rock, ore, heal_ore, special_ore, barrel): `ARRArenaGrid::OnCellDestroyed` ([[sys-grid]]); a whole deposit broken by the drone counts each block.
  - `DamageDealtByWeapon` (weapon class name, raw hit damage to robots; turret hits go to the owning launcher; explosion damage not counted), `KillsByRobotType` (EnemyStats.csv row, class name when empty), `KillsBySource` (explosion = `URRGE_DamageExplosion` killing hit, weapon = a weapon instigator, else other), `DamageTakenBySource` (miner: robot type of the instigator, explosion, other; raw damage): `URRHealthAttributeSet::OnAnyDamageReceived`, a static native event added for this system ([[sys-damage]]), filtered to the tracked world.
  - `UpgradesPicked`: `URRUpgradeComponent::OnUpgradeApplied` ([[sys-upgrades]]), ids `<Stat>_<Tier>@<Player|AllWeapons|weapon class>` or `NewWeapon@<class>`.
  - `XPCollected`: `URRProgressionComponent::OnXPGained` (after XPGain; `rr.Cheat.GrantXP` counts too) ([[sys-progression]]).
- data: none; key names chosen here (implementation choice); JSON field names in [[run-log-schema]].
- depends_on: [[sys-run]], [[sys-run-transition]], [[sys-grid]], [[sys-damage]], [[sys-upgrades]], [[sys-progression]].
- implements: [[run-log-schema]] (collector part).
- gotchas: raw damage includes overkill and shield. Robots destroyed without a damage hit (a kamikaze exploding on contact) are not kills. A miner death restarts the map, so the counters reset (the writer snapshots on `OnDeath`, before). Verified by -game -nullrhi logs (task-212-01, `Saved/Logs/task212_01_game.log`: 80 s with the pacing bot, drone 9 and kamikaze 3 rock blocks, burst rifle 2780 damage, 87 kills (weapon 76, explosion 11), 4 cards in order, Chaser damage 80, XP 147.7; `task212_01_game4.log`: kamikaze explosion 30 taken, explosion kill); no automation test (owner rule).
- terrain source `extractor` (task-372-01, de889932, 0e85c9a0): `GetTerrainSourceName(Extractor)` returns `extractor`, a fifth key of `TerrainDestroyedBySource` ([[run-log-schema]]); the blocks destroyed under the extractor footprint ([[sys-grid-api]]) are counted there per block.

## Relations
- [[systems-world-index]] — parent index.
- [[sys-run-log-writer]] — child: level entries and the run JSON file.
- [[run-log-schema]] — the fields this collects.
- [[sys-run]] — run level index and the miner hook order.
- [[sys-run-transition]] — OnLevelReady restamps the start.
- [[sys-damage]] — the global damage event.
- [[sys-grid]] — terrain destruction event.

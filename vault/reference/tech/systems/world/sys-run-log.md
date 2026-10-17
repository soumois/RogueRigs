---
id: sys-run-log
description: "URRRunLogSubsystem: per-level run log counters (terrain, damage per weapon, kills, upgrades, damage taken, XP) fed by events; open to change the collected fields."
parent: systems-world-index
kind: doc
---
# Run log collector

- responsibility: collect the per-level counters of [[run-log-schema]] during play; the writer (task-212-02) snapshots them at level end and adds the end-of-level fields (outcome, time, time left, risk-window ore, events).
- main_classes (`Source/RogueRigs/RunLog/`): `URRRunLogSubsystem` (`UGameInstanceSubsystem`, survives the map travel; counters cover the current level only); `FRRRunLevelStats` (USTRUCT snapshot); debug command `RRRunLogDebugCommands.cpp` (not in shipping).
- entry_points:
  - Level start: `ARRMiner::BeginPlay` ends with `HandleMinerBeginPlay(Miner)`, after the run snapshot replay ([[sys-run]]), so replayed cards are not counted; first miner of a world only: counters reset, `LevelIndex` and `LayoutName` from `URRRunSubsystem`, `LevelStartWorldSeconds` = world time; binds the grid, the miner's upgrade and progression components. `URRRunTransitionSubsystem::OnLevelReady` ([[sys-run-transition]]) restamps the start time to the end of the loading hold. Restart and travel create a new world, hence a reset.
  - Reads: `GetCurrentLevelStats()` (BlueprintPure copy), `LogCurrentLevelStats(Context)`; static `MakeCardId`, `GetTerrainSourceName`, `GetCellTypeName`.
  - Debug: `rr.RunLog.Dump` logs every counter on `LogRogueRigs`.
- fields and hooks (events only, no polling):
  - `TerrainDestroyedBySource` (barrel, kamikaze, drone, shot) and `TerrainDestroyedByType` (rock, ore, heal_ore, special_ore, barrel): `ARRArenaGrid::OnCellDestroyed` ([[sys-grid]]); a whole deposit broken by the drone counts each block.
  - `DamageDealtByWeapon` (weapon class name, raw hit damage to robots; turret hits go to the owning launcher; explosion damage not counted), `KillsByRobotType` (EnemyStats.csv row, class name when empty), `KillsBySource` (explosion = `URRGE_DamageExplosion` killing hit, weapon = a weapon instigator, else other), `DamageTakenBySource` (miner: robot type of the instigator, explosion, other; raw damage): `URRHealthAttributeSet::OnAnyDamageReceived`, a static native event added for this system ([[sys-damage]]), filtered to the tracked world.
  - `UpgradesPicked`: `URRUpgradeComponent::OnUpgradeApplied` ([[sys-upgrades]]), ids `<Stat>_<Tier>@<Player|AllWeapons|weapon class>` or `NewWeapon@<class>`.
  - `XPCollected`: `URRProgressionComponent::OnXPGained` (after XPGain; `rr.Cheat.GrantXP` counts too) ([[sys-progression]]).
- data: none; key names chosen here (implementation choice; the schema field names are task-212-02's).
- depends_on: [[sys-run]], [[sys-run-transition]], [[sys-grid]], [[sys-damage]], [[sys-upgrades]], [[sys-progression]].
- implements: [[run-log-schema]] (collector part).
- gotchas: raw damage includes overkill and shield. Robots destroyed without a damage hit (a kamikaze exploding on contact) are not kills. A miner death restarts the map, so the counters reset (the writer must snapshot before). Nothing is written to disk yet. Verified by -game -nullrhi logs (task-212-01, `Saved/Logs/task212_01_game.log`: 80 s with the pacing bot, drone 9 and kamikaze 3 rock blocks, burst rifle 2780 damage, 87 kills (weapon 76, explosion 11), 4 cards in order, Chaser damage 80, XP 147.7; `task212_01_game4.log`: kamikaze explosion 30 taken, explosion kill); no automation test (owner rule).

## Relations
- [[systems-world-index]] — parent index.
- [[run-log-schema]] — the fields this collects.
- [[sys-run]] — run level index and the miner hook order.
- [[sys-run-transition]] — OnLevelReady restamps the start.
- [[sys-damage]] — the global damage event.
- [[sys-grid]] — terrain destruction event.

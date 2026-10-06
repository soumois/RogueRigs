---
id: task-025-01-numbers-pipeline
description: "World subsystem receiving every damage event, merging per target within 0.25 s, feeding one Niagara system; toggle; actor version removed."
parent: task-025-damage-numbers
kind: task
status: done
---
# Damage number pipeline

- goal: damage numbers come from one pooled system.
- agent: cpp
- depends_on: task-024-06-tests-integration
- inputs: [[damage-numbers]], [[sys-damage]], [[sys-enemies]], `sys-target-dummy` (removed in task-031-01), [[cpp-standard]], [[sys-automation-tests]], [[drgs-analysis]]
- expected_output: URRDamageNumberSubsystem: AddHit(Target, Value, bCrit, Location); merge window 0.25 s per target (sum, crit if any crit); pushes particles to a Niagara system through an array data interface (position, value, crit, spawn time) or BlueprintImplementableEvent batch for the bp agent; settings toggle `bShowDamageNumbers` (GameUserSettings or cheat-free config); ARRFloatingDamageText retired (callers moved); crit size 1.6×, crit color white-gold, normal white; docs: sys-damage or new sys-damage-numbers. Tests `RogueRigs.DamageNumbers.*` (merge, crit, toggle). Side item: in `Integration.LevelUpDiscEndsHidden` the 5-step disc is the kept behaviour: remove the doc-mismatch warning.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless (new ones included); docs updated; lint clean.
- editor_state: closed
- token_ceiling: 110000
- model: opus
- tokens: input 84, cache write 149885, cache read 3989150, output 55379
- runs: 1
- milestone: M2
- result: URRDamageNumberSubsystem (AddHit, 0.25 s merge per target, crit if any, last hit location kept for dead robots, ±25 uu jitter, 64 numbers per tick, config in DefaultGame.ini), one persistent Niagara component for /Game/RogueRigs/FX/NS_DamageNumbers fed through user arrays (DamageNumberPositions, Values, Sizes, Colors, Criticals, SpawnTimes, NewCount); crit 1.6× white-gold, normal 60 white; URRGameUserSettings bShowDamageNumbers toggle (DefaultEngine GameUserSettingsClassName); ARRFloatingDamageText deleted; Niagara added to Build.cs. Disc warning removed. 4 DamageNumbers tests; build 0, 146/146. New sys-damage-numbers; sys-damage, sys-target-dummy, sys-enemies-base, build-toolchain. Commits 36d17ab, e8e20a9.

## Relations
- [[task-025-damage-numbers]] — parent global task.

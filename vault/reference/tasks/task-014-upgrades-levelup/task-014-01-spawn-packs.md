---
id: task-014-01-spawn-packs
description: "Spawner: 30 s trickle, then packs of 8 to 15 every 6 s from one side, rare lone robots, cap 100, kamikaze 15 %."
parent: task-014-upgrades-levelup
kind: task
status: done
---
# Spawn packs

- goal: robots arrive in packs, five times more of them.
- agent: cpp
- depends_on: none
- inputs: [[enemies-index]], [[sys-spawner]], [[sys-flow-field]], [[sys-enemies]], [[sys-automation-tests]], [[cpp-standard]].
- expected_output: `ARREnemySpawner`: editable `TrickleDuration` 30 s, `PackInterval` 6 s, `PackSizeMin` 8, `PackSizeMax` 15, `LoneSpawnChance` per interval (small, documented), `MaxAlive` 100; a pack picks one side of the view rectangle and spawns its robots on distinct candidate cells of that side, clamped to free slots; class weights chaser 85 / kamikaze 15 as the documented default for the placed spawner; candidate scan cost kept acceptable for packs (one scan per pack). [[sys-spawner]] updated. Tests `RogueRigs.Spawner.*`: trickle phase spawns one at a time; a pack spawns between min and max on one side; cap respected; kamikaze share about 15 % over a seeded draw.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless (new ones included); docs updated; lint clean.
- editor_state: closed
- token_ceiling: 80000
- model: opus
- tokens: input 58, cache write 89717, cache read 1683743, output 40425
- runs: 1
- milestone: M1
- result: ARREnemySpawner: 30 s trickle (2 s), then packs of 8 to 15 every 6 s from one side of the view rectangle (one candidate scan per pack, clamped to free slots), lone robot chance 0.1 per 2 s between packs (provisional), MaxAlive 100, seeded stream; TrySpawnPack, SeedRandomStream, GetRunTime, IsInPackPhase, GetSideOfRectangle. Kamikaze weights live on BP_EnemySpawner defaults (3/1) → set 85/15 in task-014-04. 4 new Spawner tests (15.4 % kamikaze over 2000 draws); build 0, 62/62. Docs sys-spawner, sys-automation-tests. Commit 8d152f3, about 96k tokens.

## Relations
- [[task-014-upgrades-levelup]] — parent global task.

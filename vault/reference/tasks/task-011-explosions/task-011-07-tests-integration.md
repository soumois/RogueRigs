---
id: task-011-07-tests-integration
description: "Integration tests with final Blueprints: a barrel chain opens a wall, a kamikaze reaches and hurts the miner, no one-shot, full run."
parent: task-011-explosions
kind: task
status: done
---
# Integration tests

- goal: prove the explosion loop with the final assets.
- agent: cpp
- depends_on: task-011-06-bp-explosions
- inputs: [[sys-automation-tests]], [[sys-explosions]], [[sys-enemies]], [[barrel]], [[enemy-kamikaze]], [[build-toolchain]].
- expected_output: tests `RogueRigs.Integration.*` with `BP_ArenaGrid` (arena_m1 with barrels), `BP_Miner`, `BP_Kamikaze`: shooting a paired barrel chains to its partner 0.08 s later and destroys every block in both radii; a kamikaze follows the flow field, arms and hurts the miner by 30; a full-health miner survives a barrel explosion at point blank; a kamikaze killed by the rifle explodes and destroys nearby blocks. Full `RogueRigs` run. [[sys-automation-tests]] updated.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless (orchestrator re-runs); lint clean.
- editor_state: closed
- token_ceiling: 70000
- milestone: M1
- result: 4 Integration tests: rifle shot on barrel (24, 49) chains to (24, 48) 0.083 s later, 5 of 5 blocks destroyed, FX spawned per explosion and gone after 0.23 s; kamikaze from (34, 34) arms at 5.48 s, miner 100 → 70; point-blank barrels: miner 100 → 60 (second blast in i-frames); rifle kills a kamikaze, its blast destroys the nearby block, 1 XP. Fixed a flaky Kamikaze test check (pickup pop offset). Build exit 0, 52/52 (orchestrator re-ran). About 98k tokens.

## Relations
- [[task-011-explosions]] — parent global task.

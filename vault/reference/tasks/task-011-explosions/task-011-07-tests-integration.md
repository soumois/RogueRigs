---
id: task-011-07-tests-integration
description: "Integration tests with final Blueprints: a barrel chain opens a wall, a kamikaze reaches and hurts the miner, no one-shot, full run."
parent: task-011-explosions
kind: task
status: in-progress
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
- result: open

## Relations
- [[task-011-explosions]] — parent global task.

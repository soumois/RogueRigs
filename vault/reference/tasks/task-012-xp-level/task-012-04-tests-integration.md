---
id: task-012-04-tests-integration
description: "Integration tests with final Blueprints: kill robots with the rifle, collect their XP cubes, level up; full run."
parent: task-012-xp-level
kind: task
status: todo
---
# Integration tests

- goal: prove the XP loop with the final assets.
- agent: cpp
- depends_on: task-012-03-bp-hud-kamikaze
- inputs: [[sys-automation-tests]], `sys-progression`, [[sys-enemies]], [[sys-pickups]], [[build-toolchain]].
- expected_output: tests `RogueRigs.Integration.*`: `BP_Miner` kills 5 `BP_Chaser` with its rifle, collects their 5 XP cubes and reaches level 2 with one level-up event; a `BP_Kamikaze` takes 10 rifle hits (100 HP) before dying. Full `RogueRigs` run; [[sys-automation-tests]] updated.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless (orchestrator re-runs); lint clean.
- editor_state: closed
- token_ceiling: 60000
- milestone: M1
- result: open

## Relations
- [[task-012-xp-level]] — parent global task.

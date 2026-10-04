---
id: task-021-03-tests-integration
description: "Integration tests with the final Blueprints: shooter keeps distance, fires with LOS, repositions around rock, shots hit the miner, mix 70/15/15."
parent: task-021-shooter
kind: task
status: done
---
# Shooter integration tests

- goal: the shooter is covered with the real Blueprints.
- agent: cpp
- depends_on: task-021-02-bp-shooter
- inputs: [[sys-automation-tests]], task-021-02 result.
- expected_output: `RogueRigs.Integration.*` tests with BP_Shooter, BP_Miner and the arena: distance ring held, laser then shot timing, rock blocks then repositioning restores LOS, miner damaged 12 per hit, robots unharmed by shots; full run.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless (new ones included); docs updated; lint clean.
- editor_state: closed
- token_ceiling: 70000
- model: opus
- tokens: input 50, cache write 106422, cache read 1812226, output 25364
- runs: 1
- milestone: M2
- result: Integration tests with real BP_Shooter, BP_ShooterProjectile, BP_Miner: ShooterHoldsRingAndFires (aim at 1.417/3.417/5.417 s, shot 0.6 s later, ring 809–999 uu, miner 100 → 64), ShooterRepositionsAroundRock (reposition, aims with sight at 896 uu, one hit), ShooterShotsSpareRobots (shots pass a chaser on the line, chaser unharmed), PlacedSpawnerMix (placed L_ArenaM1 spawner effective weights 70/15/15 from its external actor file; the 85/15 seen in the editor was session-only). Build 0, 113/113. Doc sys-automation-tests. Commit 2ffff01.

## Relations
- [[task-021-shooter]] — parent global task.

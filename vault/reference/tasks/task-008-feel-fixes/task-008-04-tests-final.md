---
id: task-008-04-tests-final
description: "Headless test that a shot fired by BP_Miner hits a floor-standing BP_TargetDummy, then the full RogueRigs test run with the final Blueprints."
parent: task-008-feel-fixes
kind: task
status: done
---
# Final tests

- goal: prove with the final Blueprints that the miner's shots hit a dummy on the floor.
- agent: cpp
- depends_on: task-008-03-bp-miner-dummy
- inputs: [[sys-automation-tests]], [[sys-weapons]], `sys-target-dummy` (removed in task-031-01), [[bp-miner]], `bp-target-dummy` (removed in task-031-01), [[build-toolchain]].
- expected_output: new test `Weapons.MinerHitsFloorDummy`: `BP_Miner` with its starting `BP_BurstRifle`, a `BP_TargetDummy` (160 uu) resting on the floor 3 cells away in the aim direction on open floor of `arena_m1`, aim set toward it; within 2 s the dummy's `Health` drops below `MaxHealth`. Full `RogueRigs` headless run. [[sys-automation-tests]] updated.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless (orchestrator re-runs); lint clean.
- editor_state: closed
- token_ceiling: 50000
- milestone: M1
- result: new test Weapons.MinerHitsFloorDummy: BP_Miner on (38, 43) aims −Y at a 160 uu BP_TargetDummy on (38, 40) resting on the floor; first hit at 0.167 s, Health 990. Full run 14/14 pass (orchestrator re-ran). No bug found. Doc sys-automation-tests. About 48k tokens.

## Relations
- [[task-008-feel-fixes]] — parent global task.

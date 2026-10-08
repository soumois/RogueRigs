---
id: task-054-02-juice-test
description: "Integration.JuiceKillDropsStreamingCubes updated to the Niagara trail and sparkle of task-054-01."
parent: task-054-niagara-trail-sparkle
kind: task
status: in-progress
---
# Juice test after Niagara

- goal: the existing juice test checks the new Niagara trail and sparkle.
- agent: cpp
- depends_on: task-054-01-niagara-trail-sparkle
- inputs: bp-pickup-xp (vault/reference/tech/blueprints/bp-pickup-xp.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md)
- expected_output: RRJuiceIntegrationTests.cpp, Integration.JuiceKillDropsStreamingCubes: the trail check reads the `TrailFX` UNiagaraComponent of BP_PickupXP (inactive during the pop, active once magnetised) instead of the removed `Trail` static mesh; the collect check counts NS_PickupSparkle Niagara components spawned at the collect points instead of BP_PickupSparkle actors; no other test change, no new test; docs sys-automation-tests.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 40000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-054-niagara-trail-sparkle]] — parent global task.

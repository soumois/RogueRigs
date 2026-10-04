---
id: task-021-03-tests-integration
description: "Integration tests with the final Blueprints: shooter keeps distance, fires with LOS, repositions around rock, shots hit the miner, mix 70/15/15."
parent: task-021-shooter
kind: task
status: in-progress
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
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-021-shooter]] — parent global task.

---
id: task-026-04-tests-integration
description: "Integration tests with the final Blueprints: director phases on the real map, horde banner then front, vignette event on hit."
parent: task-026-pressure
kind: task
status: todo
---
# Pressure integration tests

- goal: the pressure features are covered.
- agent: cpp
- depends_on: task-026-03-bp-pressure
- inputs: [[sys-automation-tests]], task-026-03 result.
- expected_output: `RogueRigs.Integration.*` tests: placed spawner runs the director (calm then first pack), horde announced 4 s before a front of 40+ robots, damage triggers the vignette event; full run.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless (new ones included); docs updated; lint clean.
- editor_state: closed
- token_ceiling: 70000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-026-pressure]] — parent global task.

---
id: task-494-03-capture
description: "Capture: Visual check of the test map and of one reload."
parent: task-494-generator-test-map
kind: task
status: in-progress
---
# Enemy-free test map with a command to generate and reload a new level: capture

- goal: Visual check of the test map and of one reload.
- agent: capture
- depends_on: task-494-02-test-map
- inputs: agent-capture (vault/reference/agents/agent-capture.md)
- expected_output: see done_when
- done_when: C1 the test map shows a generated asteroid with rock, ore, heal veins and barrels, miner at the spawn; C2 no robot, console, chest or shuttle visible after 2 minutes; C3 the seed is shown on screen; C4 after rr.Level.New a different layout is visible with its new seed and the miner at the new spawn; C5 after rr.Level.New with the first seed the first layout is back. Frames sent to the owner.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-07T08:37:42Z
- waiting_for: none
- result: open

## Relations
- [[task-494-generator-test-map]] — parent global task.

---
id: task-525-02-capture
description: "Capture: alert bar before, between and after the hordes"
parent: task-525-horde-marker-one-at-a-time
kind: task
status: in-progress
---
# Un seul point d'exclamation de horde à la fois: capture

- goal: alert bar before, between and after the hordes
- agent: capture
- depends_on: task-525-01-single-marker
- inputs: agent-capture (vault/reference/agents/agent-capture.md), level-flow (vault/reference/gdd/run/level-flow.md)
- expected_output: see done_when
- done_when: C1 level start: one "!"; C2 after the first horde: its "!" gone, the second shown; C3 after the last: none; C4 letters whole; frames sent to the owner.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-13T14:03:33Z
- waiting_for: none
- result: open

## Relations
- [[task-525-horde-marker-one-at-a-time]] — parent global task.

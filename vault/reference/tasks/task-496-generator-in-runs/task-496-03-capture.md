---
id: task-496-03-capture
description: "Capture: One -game capture of the generated terrains of level 1 and level 2, compared with the hand layouts before."
parent: task-496-generator-in-runs
kind: task
status: todo
---
# Layout generator wired into real runs: capture

- goal: One -game capture of the generated terrains of level 1 and level 2, compared with the hand layouts before.
- agent: capture
- depends_on: task-496-02-request-timing
- inputs: agent-capture (vault/reference/agents/agent-capture.md), arena-grid (vault/reference/gdd/terrain/arena-grid.md)
- expected_output: see done_when
- done_when: C1 Level 1 seen after New Run from the main menu: a full arena, irregular asteroid edge, rock, ore, heal ore and barrels, different from arena_m1_fine before. C2 Level 2 seen after Continue: a full arena different from arena_l2 before. C3 The loading screen and the fade in show no frozen or empty arena. Sheet sent to the owner.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-496-generator-in-runs]] — parent global task.

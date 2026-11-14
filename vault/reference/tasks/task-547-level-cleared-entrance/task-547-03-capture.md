---
id: task-547-03-capture
description: "Capture: Entrance sequence frames of the stats screen and the death screen"
parent: task-547-level-cleared-entrance
kind: task
status: in-progress
---
# Level cleared screen appears with the death screen entrance effect: capture

- goal: Entrance sequence frames of the stats screen and the death screen
- agent: capture
- depends_on: task-547-02-stats-entrance
- inputs: death-screen (vault/reference/gdd/run/death-screen.md)
- expected_output: see done_when
- done_when: C1 stats screen frames at about 0.3, 0.7, 1.0 and 1.5 s show backdrop, title drop, frame slide, button last; C2 skip by a key shows the end state at once; C3 death screen sequence matches its before frames.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-14T04:17:55Z
- waiting_for: none
- result: open

## Relations
- [[task-547-level-cleared-entrance]] — parent global task.

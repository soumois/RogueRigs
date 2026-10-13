---
id: task-138-xp-pickups-perf-test
description: "Global task: Measure the frame-time cost of a burst of XP nuts and chips, with and without bloom and pulse, and record the results."
parent: tasks-to-do-index
kind: task
status: todo
---
# XP pickups: add the stress command

- request: Do the cost check of the XP pickup glow: frame time with a burst of nuts and chips, with and without bloom and the emissive pulse (owner, 2026-10-13) (owner, 2026-10-13)
- goal: Measure the frame-time cost of a burst of XP nuts and chips, with and without bloom and pulse, and record the results.
- acceptance: The project compiles (RogueRigsEditor, editor closed); rr.Stress.Pickups 100 6 followed by rr.Stress.ClearPickups works in a -game run with the counts read back in the log; sys-stress updated; lint clean; committed; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-13).
- confirmed_by_owner: yes (2026-10-13)
- milestone: M2
- next_step: task-138-01-xp-pickups-perf-test
- result: open

## Subtasks
1. task-138-01-xp-pickups-perf-test — cpp (the stress command)
2. task-138-02-xp-pickups-perf-test — cpp (measure and record)

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-138-01-xp-pickups-perf-test]] — step 1, cpp.
- [[task-138-02-xp-pickups-perf-test]] — step 2, cpp.

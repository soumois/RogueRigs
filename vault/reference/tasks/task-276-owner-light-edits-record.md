---
id: task-276-owner-light-edits-record
description: "Global task: Owner editor light edits of 2026-10-21 (mask 0.08 / 200 / 1.8, sun 3 lux) recorded in the docs with revert values."
parent: tasks-done-index
kind: task
status: done
---
# Owner light edits recorded

- request: c'est fait (owner saved in the editor himself: MPC_PlayerLight FarBrightness 0.03 -> 0.08, Radius 250 -> 200, Exponent 4 -> 1.8; DirectionalLight_0 intensity 6 -> 3 lux; committed by him as 15dc423d) (owner, 2026-10-21)
- goal: The owner editor changes of 2026-10-21 (MPC_PlayerLight 0.08 / 200 / 1.8, sun 3 lux) are recorded in the docs with before, after and revert values.
- acceptance: Docs updated with before/after/revert values; values confirmed from the uassets; lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-21).
- confirmed_by_owner: yes (2026-10-21, owner saved the values himself)
- milestone: M1
- next_step: none
- result: Docs only. Owner saved and committed 15dc423d himself: MPC_PlayerLight FarBrightness 0.03 -> 0.08, Radius 250 -> 200, Exponent 4 -> 1.8 (Enable 1); DirectionalLight_0 6 -> 3 lux. Confirmed from the float values in both saved packages (before and after); editor closed, not read in the editor. Updated mpp-player-light-mask, lights-dark-ambient, handover-2026-10-21-lighting-floor. Revert 0.03 / 250 / 4, sun 6.

## Subtasks
1. task-276-01-owner-light-edits-record — orchestrator (doc only)

## Relations
- [[tasks-done-index]] — parent index.
- [[task-276-01-owner-light-edits-record]] — step 1, orchestrator (doc only).

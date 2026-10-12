---
id: task-104-hud-regressions
description: "Global task: Two Blueprint regressions of the HUD split are fixed: the damage vignette never fades and WBP_XPStatus divides by zero."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# HUD split regressions: vignette fade and XP divide by zero

- request: "RogueRigs.Integration.PressureVignetteOnHit fails after the HUD split" (found by task-094-10, 2026-10-11) (owner, 2026-10-12)
- goal: Two Blueprint regressions of the HUD split are fixed: the damage vignette never fades and WBP_XPStatus divides by zero.
- acceptance: both fixes compiled 0 errors and saved; the fade logic reads back correct (hold then decay with a non-zero step); the XP division guarded; docs updated; lint clean; committed. The Orchestrator then reruns PressureVignetteOnHit headless; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-12).
- confirmed_by_owner: yes (2026-10-12)
- milestone: M2
- next_step: task-104-01-hud-regressions
- result: open

## Subtasks
1. task-104-01-hud-regressions — bp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-104-01-hud-regressions]] — step 1, bp.

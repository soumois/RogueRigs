---
id: task-146-xp-nut-disc-lighter
description: "Global task: Lower the glow disc intensity of the XP nut."
parent: tasks-done-index
kind: task
status: done
---
# XP nut disc: lighter

- request: The glow disc of the XP nut is too strong (owner, 2026-10-13): Intensity 0.5 to 0.2 (owner, 2026-10-14)
- goal: Lower the glow disc intensity of the XP nut.
- acceptance: Intensity reads 0.2 on the nut instance and 0.5 on the chip instance; asset saved and modified in git; doc updated; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-14).
- confirmed_by_owner: yes (2026-10-14)
- milestone: M2
- next_step: none
- result: Done. MI_XPGlowDisc_Nut Intensity 0.2 (was 0.5), chip instance unchanged at 0.5; first attempt failed because the editor was closed.

## Subtasks
1. task-146-01-xp-nut-disc-lighter — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-146-01-xp-nut-disc-lighter]] — step 1, bp.

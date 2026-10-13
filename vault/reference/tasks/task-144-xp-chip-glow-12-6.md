---
id: task-144-xp-chip-glow-12-6
description: "Global task: Set the big XP chip core glow to 12 and 6, a starting point to tune by eye."
parent: tasks-done-index
kind: task
status: done
---
# XP chip: glow 12 and 6

- request: Put the chip core glow at 12 (bright) and 6 (dark); the nut keeps 25; the owner will tune the chip by eye (owner, 2026-10-13) (owner, 2026-10-13)
- goal: Set the big XP chip core glow to 12 and 6, a starting point to tune by eye.
- acceptance: GlowAmount reads 12 and 6, both assets saved and modified in git, nothing else changed, doc updated; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-13).
- confirmed_by_owner: yes (2026-10-13)
- milestone: M2
- next_step: none
- result: Done. M_XPChip_core_bright GlowAmount 12 and core_dark 6 (colours and pulse unchanged), saved; a starting point for the owner to tune by eye; nut stays 25.

## Subtasks
1. task-144-01-xp-chip-glow-12-6 — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-144-01-xp-chip-glow-12-6]] — step 1, bp.

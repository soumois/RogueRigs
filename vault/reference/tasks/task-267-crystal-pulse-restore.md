---
id: task-267-crystal-pulse-restore
description: "Global task: The deposit crystals pulse again, and the heal crystals get their hashed pulse back."
parent: tasks-done-index
kind: task
status: done
---
# Crystal pulse restore

- request: the crystals no longer pulse (reported 2026-10-20) (owner, 2026-10-21)
- goal: The deposit crystals pulse again, and the heal crystals get their hashed pulse back.
- acceptance: cause of the frozen crystal pulse identified, Duskite and heal deposit crystals pulse with hashed phase and rate again; docs and lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-21).
- confirmed_by_owner: yes (2026-10-21)
- milestone: M1
- next_step: none
- result: Done. M_DepositCrystalGlow reads per-instance 13 (phase) and 14 (rate), CPD parameters PulsePhase and PulseRate deleted (no CPD left); M_XPNutGlow: If(PulseRate > 0, PulseRate, 1) so an unwritten slot means rate 1 (XP nut, XP cube cores and every child recompile, 7 direct referencers). Docs updated. Runtime pulse unverified on screen.

## Subtasks
1. task-267-01-crystal-pulse-restore — cpp
2. task-267-02-crystal-pulse-restore — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-267-01-crystal-pulse-restore]] — step 1, cpp.
- [[task-267-02-crystal-pulse-restore]] — step 2, bp.

---
id: task-062-alert-bar-x2
description: "Global task: Alert bar twice as wide and twice as tall, its icons scaled and repositioned."
parent: tasks-index
kind: task
status: done
---
# Bigger alert bar

- request: "Double la taille de la barre d’alerte, elle est trop petite" (owner, 2026-10-07)
- goal: Alert bar twice as wide and twice as tall, its icons scaled and repositioned.
- acceptance: PIE check with no error; docs updated; lint clean; committed.
- gdd_check: GDD updated by the Orchestrator (owner 2026-10-07, fifth test). No open value.
- confirmed_by_owner: yes (2026-10-07)
- milestone: M2
- next_step: none
- result: Done. Integration.PressureHordeBanner expects icons at Position × 600 (PressureAlertBarWidth 300 → 600). Build 0; full run 194/195 (only the known FlowField.BudgetAt67uu). Commit 693b54a. With task-062-01: alert bar 600 × 36, icons × 2.

## Subtasks
1. task-062-01-alert-bar-x2 — bp
2. task-062-02-pressure-test — cpp

## Relations
- [[tasks-index]] — parent index.
- [[task-062-01-alert-bar-x2]] — step 1, bp.
- [[task-062-02-pressure-test]] — step 2, cpp.

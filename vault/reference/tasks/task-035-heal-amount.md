---
id: task-035-heal-amount
description: "Global task: a heal vein restores 60 HP in total, about 6.67 HP per heal pickup."
parent: tasks-index
kind: task
status: todo
---
# Heal amount

- request: "J'aimerais que le total fasse 60" (2026-10-06, heal per vein)
- goal: collecting a whole 3 × 3 heal vein restores 60 HP.
- acceptance: (1) HealAmount 60 / 9 per heal pickup; (2) build exits 0; tests pass; (3) docs; lint clean; committed.
- gdd_check: GDD heal-ore updated by the Orchestrator (owner 2026-10-06: 60 HP per vein). No open value.
- confirmed_by_owner: yes (2026-10-06)
- milestone: M2
- next_step: task-035-01-heal-value
- result: open

## Subtasks
1. task-035-01-heal-value — cpp: HealAmount default, existing tests

## Relations
- [[tasks-index]] — parent index.
- [[task-035-01-heal-value]] — step 1, cpp.

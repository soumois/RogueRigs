---
id: task-033-tests-speed
description: "Global task: robots all at 180 uu/s and the test suite updated to the owner's data changes (speed, XP curve after the reference game)."
parent: tasks-done-index
kind: task
status: done
---
# Tests after the owner's data changes

- request: "mets à jour les tests et les robots doivent avoir la vitesse de robots doit être de 180" (2026-10-06)
- goal: every robot moves at 180 uu/s; player projectiles collide 20 % larger (owner 2026-10-06); the existing tests match the current data and pass.
- acceptance: (1) EnemyStats.csv MoveSpeed 180 for every robot; (2) existing tests updated to the current speeds and XPCurve.csv (level 1 = 4, then the reference curve ÷ 5), no new test added; (3) build exits 0; all tests pass; (4) docs; lint clean; committed.
- gdd_check: Read enemy nodes and [[xp-level-up]]; the Orchestrator wrote 180 uu/s and the curve in the GDD (owner 2026-10-06). No open value.
- confirmed_by_owner: yes (2026-10-06)
- milestone: M2
- next_step: none
- result: Done. Robots at 180 uu/s, player projectile collision 12 uu, tests updated, 195/195.

## Subtasks
1. task-033-01-update-tests — cpp: update existing test expectations to the current data, full run

## Relations
- [[tasks-done-index]] — parent index.
- [[task-033-01-update-tests]] — step 1, cpp.

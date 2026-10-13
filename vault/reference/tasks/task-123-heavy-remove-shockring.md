---
id: task-123-heavy-remove-shockring
description: "Global task: BP_Heavy no longer shows its ShockRing mesh: the slam zone is drawn only by the ground telegraph."
parent: tasks-done-index
kind: task
status: done
---
# Remove the heavy ShockRing

- request: Retirer l'anneau du heavy: il doit suivre les mêmes règles que le cercle au sol (owner, 2026-10-13)
- goal: BP_Heavy no longer shows its ShockRing mesh: the slam zone is drawn only by the ground telegraph.
- acceptance: Blueprint compiles and is saved; read back shows no ShockRing visibility call left; bp-heavy doc matches; lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-13).
- confirmed_by_owner: yes (2026-10-13)
- milestone: M2
- next_step: none
- result: Done. ShockRing show, scale and hide nodes removed from BP_Heavy (OnTelegraphStart, OnSlam, OnRecover); the component stays hidden because Integration.HeavyTelegraphSlamsMiner looks it up; commits 8ec0b97 2fb3829 2784013

## Subtasks
1. task-123-01-heavy-remove-shockring — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-123-01-heavy-remove-shockring]] — step 1, bp.

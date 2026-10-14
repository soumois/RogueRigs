---
id: task-160-chaser-no-shadow
description: "Global task: Stop the chaser from casting a shadow: BodyMesh and the shield shell cast no shadow."
parent: tasks-done-index
kind: task
status: done
---
# Chaser: no cast shadow

- request: The chaser casts a dark shadow in game and must not (owner screenshot, 2026-10-14) (owner, 2026-10-14)
- goal: Stop the chaser from casting a shadow: BodyMesh and the shield shell cast no shadow.
- acceptance: Both Blueprints compile and save; the values read back; the docs updated; the editor stays open; the owner is told to look at the ground under a chaser in game (no shadow); committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-14).
- confirmed_by_owner: yes (2026-10-14)
- milestone: M2
- next_step: none
- result: Done. BodyMesh and ShieldShell cast no shadow (six properties false), BPs saved; editor was closed once, edit redone

## Subtasks
1. task-160-01-chaser-no-shadow — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-160-01-chaser-no-shadow]] — step 1, bp.

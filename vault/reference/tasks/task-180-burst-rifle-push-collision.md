---
id: task-180-burst-rifle-push-collision
description: "Global task: Stop burst rifle shots from pushing robots through collision: the burst rifle has no knockback, so a hit must not move a robot."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Burst rifle: shots push robots by collision

- request: J'ai parfois que les tirs de mitraillette repoussent les ennemis, mais pas parce qu'il y a un effet de repoussement, mais plutôt une histoire de collision (owner, 2026-10-15)
- goal: Stop burst rifle shots from pushing robots through collision: the burst rifle has no knockback, so a hit must not move a robot.
- acceptance: Cause proven by values from a run; after the fix a rerun shows no robot displacement from burst rifle hits and the shotgun still pushes; build passes with the editor closed; existing weapon and enemy tests still pass except the known EyeMesh one; docs updated; committed by path; committed.
- gdd_check: enemy-death and burst-rifle checked (2026-10-14): no conflict; the burst rifle has no knockback, only the shotgun does.
- confirmed_by_owner: yes (2026-10-15)
- milestone: M2
- next_step: task-180-01-burst-rifle-push-collision
- result: open

## Subtasks
1. task-180-01-burst-rifle-push-collision — cpp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-180-01-burst-rifle-push-collision]] — step 1, cpp.

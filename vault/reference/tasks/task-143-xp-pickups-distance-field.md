---
id: task-143-xp-pickups-distance-field
description: "Global task: Turn on the mesh distance field of the XP nut and chip and test whether they light the floor."
parent: tasks-done-index
kind: task
status: done
---
# XP pickups: mesh distance field test

- request: Test whether the XP nut and chip light the floor through Lumen once their mesh distance field is on: bGenerateMeshDistanceField is false on both meshes and the emissive light source flag alone did not light the floor (owner, 2026-10-13) (owner, 2026-10-13)
- goal: Turn on the mesh distance field of the XP nut and chip and test whether they light the floor.
- acceptance: bGenerateMeshDistanceField reads true on both meshes, both saved and modified in git, project settings reported, nothing else changed, doc updated, no UI automation used; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-13).
- confirmed_by_owner: yes (2026-10-13)
- milestone: M2
- next_step: none
- result: Done. bGenerateMeshDistanceField true on SM_XPNut and SM_XPChipBig, saved; no tool rebuilds the distance field (owner presses Apply Changes); project: Lumen, Generate Mesh Distance Fields true, hardware RT allowed, Software Ray Tracing Mode GlobalTracing (per-mesh distance fields are traced only in Detail Tracing); nothing global changed.

## Subtasks
1. task-143-01-xp-pickups-distance-field — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-143-01-xp-pickups-distance-field]] — step 1, bp.

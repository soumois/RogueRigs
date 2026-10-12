---
id: task-101-barrel-asset
description: "Global task: The explosive barrel has an owner-validated source image, then a Blender-built export-ready mesh."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Barrel asset: image, corrections, Blender mesh

- request: Générer l'image source du baril explosif avec l'agent art, puis après validation le mesh avec l'agent 3d en voie Blender (pas de Meshy) (owner, 2026-10-11)
- goal: The explosive barrel has an owner-validated source image, then a Blender-built export-ready mesh.
- acceptance: the retained barrel image carries the Kessler-Voss sticker and an explosive warning sign and is validated by the owner; a cleaned FBX built in Blender is within budget (about 600 triangles, 134 x 90 cm, pivot bottom centre) with its report and is validated by the owner; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-11).
- confirmed_by_owner: yes (2026-10-11)
- milestone: M2
- next_step: task-101-02-barrel-sticker
- result: open

## Subtasks
1. task-101-01-barrel-asset — art (done)
2. task-101-02-barrel-sticker — art
3. task-101-03-barrel-warning — art
4. task-101-04-barrel-mesh — 3d (Blender route)

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-101-01-barrel-asset]] — step 1, art.
- [[task-101-02-barrel-sticker]] — step 2, art.
- [[task-101-03-barrel-warning]] — step 3, art.
- [[task-101-04-barrel-mesh]] — step 4, 3d.

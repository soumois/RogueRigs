---
id: task-101-barrel-asset
description: "Global task: The explosive barrel has an owner-validated source image, then a Blender-built export-ready mesh."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Barrel source image

- request: Générer l'image source du baril explosif avec l'agent art, puis après validation le mesh avec l'agent 3d en voie Blender (pas de Meshy) (owner, 2026-10-11)
- goal: The explosive barrel has an owner-validated source image, then a Blender-built export-ready mesh.
- acceptance: 2 PNG and their run log exist in Art/Source/barrel/; a pass or exact gap is written per image against the A rules of art-style-guide; nothing in vault/assets/; no more than 2 variants generated; committed (Art/Source/barrel by name); committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-11).
- confirmed_by_owner: yes (2026-10-11)
- milestone: M2
- next_step: task-101-01-barrel-asset
- result: open

## Subtasks
1. task-101-01-barrel-asset — art

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-101-01-barrel-asset]] — step 1, art.

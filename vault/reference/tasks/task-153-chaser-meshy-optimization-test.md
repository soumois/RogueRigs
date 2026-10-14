---
id: task-153-chaser-meshy-optimization-test
description: "Global task: Test the optimisation of the Meshy chaser mesh (792154 triangles) down to the 3000 triangle budget with a baked texture."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Chaser: Meshy optimisation test

- request: The owner received a Meshy mesh of a yellow sentinel mech (792154 triangles, one fused mesh, 4 texture maps) and asks for an optimisation test, since it does not follow the 3000 triangle budget (owner, 2026-10-14) (owner, 2026-10-14)
- goal: Test the optimisation of the Meshy chaser mesh (792154 triangles) down to the 3000 triangle budget with a baked texture.
- acceptance: The script runs headless; the 3000 triangle FBX has at most 3000 triangles, one object, one material and a clean UV; the renders and the report exist; the assessment states the numbers and the problems honestly; nothing imported in Unreal; the script and the report committed by name, the FBX, textures and renders left untracked until the owner decides; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-14).
- confirmed_by_owner: yes (2026-10-14)
- milestone: M2
- next_step: task-153-01-chaser-meshy-optimization-test
- result: open

## Subtasks
1. task-153-01-chaser-meshy-optimization-test — 3d

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-153-01-chaser-meshy-optimization-test]] — step 1, 3d.

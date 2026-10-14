---
id: task-163-chaser-anim-lightmap-fix
description: "Global task: Fix the chaser animation: the engine lightmap UVs overwrite the part index in UV channel 2."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Chaser: keep the part index UV

- request: The game visual check (task-162) found the cause of the shredded chaser: the static mesh SM_Chaser_Meshy_Anim has Generate Lightmap UVs on and its LightMapCoordinateIndex is 2, so the built render data puts a generated lightmap atlas in UV channel 2 and the material reads floor(lightmapU x 10) as the part index (37 triangles even mix bins); in the source model channel 2 is the exact PartId (owner, 2026-10-14) (owner, 2026-10-14)
- goal: Fix the chaser animation: the engine lightmap UVs overwrite the part index in UV channel 2.
- acceptance: The render data check reports ten bins and 0 mixed triangles; the shots show intact parts moving (arms, legs and torso separate and coherent) in the run and the attack, or, if not, a precise description of what is still wrong; only that asset changed; docs updated; committed by name; the editor stays closed; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-14).
- confirmed_by_owner: yes (2026-10-14)
- milestone: M2
- next_step: task-163-01-chaser-anim-lightmap-fix
- result: open

## Subtasks
1. task-163-01-chaser-anim-lightmap-fix — cpp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-163-01-chaser-anim-lightmap-fix]] — step 1, cpp.

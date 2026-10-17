---
id: task-217-xp-cube-simplify
description: "Simplify the owner's Meshy data cube (new big XP pickup) to the chip's triangle cost, look unchanged, chip material and glow disc contract."
parent: tasks-done-index
kind: task
status: done
---
# Big XP data cube: simplify the Meshy mesh

- request: "Voilà la version du cube d'xp, rien à changer. Il faut juste le simplifier pour qu'il rentre dans les coûts acceptables" (owner 2026-10-17; the path he gave was the shield zip, the cube is `Meshy_AI_Neon_Power_Cube_1017063448_texture_fbx.zip`, his latest download).
- goal: `SM_XPCubeBig.fbx` that looks like the Meshy cube, within the current chip's cost, ready to replace `SM_XPChipBig` later.
- acceptance: FBX, report and renders in `Art/Meshes/xp-cube-big-mesh/`; criteria of the subtask pass; renders sent to the owner.
- gdd_check: [[art-prompts-xp-core]] (proposal A, data cube, chosen by the owner), [[color-code]] (big XP magenta-pink; the lilac versus magenta mismatch stays open, the Meshy cube is magenta), [[bp-pickup-xp]] (chip 844 tris, 30 cm footprint, centred pivot, glow_disc last slot, scale 1.53). Budget: no XP budget in [[asset-pipeline]]; set to 844 tris max (the chip it replaces, no added cost), about 600 target (provisional, Orchestrator 2026-10-17, to confirm). Not in this task: Unreal import, Blueprint swap, rest orientation Full.
- confirmed_by_owner: yes
- milestone: M1
- next_step: none
- result: Owner validated the big XP data cube (2026-10-17): `Art/Meshes/xp-cube-big-mesh/SM_XPCubeBig.fbx`, body 748 tris, 30 x 30 x 27.5 cm, chip material and glow disc contract ([[task-217-01-xp-cube-mesh]]). Not in this task: Unreal import, Blueprint swap, rest orientation Full.

## Subtasks
- [[task-217-01-xp-cube-mesh]] — 3D agent: simplify, flat materials with emissive window slots, glow disc, export, renders.

## Relations
- [[tasks-done-index]] — parent index.
- [[art-prompts-xp-core]] — the proposal this cube comes from.

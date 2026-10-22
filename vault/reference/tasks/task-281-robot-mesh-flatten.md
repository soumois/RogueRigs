---
id: task-281-robot-mesh-flatten
description: "Global task: Flatten the Meshy-bulged planar panels of the heavy, kamikaze, shooter and chaser v2 meshes with a headless Blender pass, UVs unchanged."
parent: tasks-to-do-index
kind: task
status: todo
---
# Robot meshes: flatten bulged panels

- request: "Je te donne les 4 nouveaux mesh pour les robots. Seulement, il va falloir faire un traitement blender pour applatir les surfaces ( l'optimisation Meshy à tendance à bomber les surfaces )." (owner, 2026-10-22)
- goal: Flatten the Meshy-bulged planar panels of the heavy, kamikaze, shooter and chaser v2 meshes with a headless Blender pass, UVs unchanged.
- acceptance: every subtask done and its checks passed; every robot has a flattened FBX and report, renders before and after sent to the owner; committed.
- gdd_check: read enemy-heavy, enemy-kamikaze, enemy-shooter, enemy-chaser, chaser-meshy-pipeline, art-prompts-robots-meshy, the bp-* and sys-enemies-* nodes (2026-10-22). Owner decisions 2026-10-22 (provisional): Meshy triangle counts kept, no decimation (chaser v2 5120, heavy 10361, kamikaze 10172, shooter 7263; the chaser 3000 budget is lifted for v2); kamikaze keeps hover, only a very light leg motion, no walk; chaser v2 and a shield chaser v2 are tried in new assets, every chaser v1 and shield chaser v1 asset, file and Blueprint kept intact for restore; shooter gets walk, aim and fire clips. Open, asked with defaults: none.
- depends_on_global: none (raw meshes stored, robots-meshy-raw-2026-10-22)
- confirmed_by_owner: no (prepared 2026-10-22, owner said "on ne lance rien")
- milestone: M2
- next_step: task-281-01-flatten-heavy
- result: open

## Subtasks
1. task-281-01-flatten-heavy — 3d: Flatten the heavy panels
2. task-281-02-flatten-kamikaze — 3d: Flatten the kamikaze panels
3. task-281-03-flatten-shooter — 3d: Flatten the shooter panels
4. task-281-04-flatten-chaser-v2 — 3d: Flatten the chaser v2 panels

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-281-01-flatten-heavy]] — step 01, 3d.
- [[task-281-02-flatten-kamikaze]] — step 02, 3d.
- [[task-281-03-flatten-shooter]] — step 03, 3d.
- [[task-281-04-flatten-chaser-v2]] — step 04, 3d.

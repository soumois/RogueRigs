---
id: task-282-robot-texture-cleanup
description: "Global task: Clean the texture artefacts of the four new robot meshes (heavy glassy back panels, kamikaze smeared leg tips) against the owner turnaround sheets."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Robot meshes: clean textures from the sheets

- request: "Aussi, il y a des artefacts dans les textures ( voir image 1 qui est le dos du gros et image 2 qui est une des pattes du kamikaze ), il faudrait qu'avec les images que tu as pour les robots, tu clean ces textures." (owner, 2026-10-22)
- goal: Clean the texture artefacts of the four new robot meshes (heavy glassy back panels, kamikaze smeared leg tips) against the owner turnaround sheets.
- acceptance: every subtask done and its checks passed; every robot has a cleaned texture set and report, renders before and after sent to the owner; committed.
- gdd_check: read enemy-heavy, enemy-kamikaze, enemy-shooter, enemy-chaser, chaser-meshy-pipeline, art-prompts-robots-meshy, the bp-* and sys-enemies-* nodes (2026-10-22). Owner decisions 2026-10-22 (provisional): Meshy triangle counts kept, no decimation (chaser v2 5120, heavy 10361, kamikaze 10172, shooter 7263; the chaser 3000 budget is lifted for v2); kamikaze keeps hover, only a very light leg motion, no walk; chaser v2 and a shield chaser v2 are tried in new assets, every chaser v1 and shield chaser v1 asset, file and Blueprint kept intact for restore; shooter gets walk, aim and fire clips. Defaults accepted by the owner (provisional, owner 2026-10-22): use of the Meshy normal, metallic and roughness maps (default: base colour only, constant roughness, metallic 0, as chaser v1).
- depends_on_global: task-281
- confirmed_by_owner: yes (2026-10-22, plan accepted; launch on the owner's word)
- milestone: M2
- next_step: task-282-04-texture-clean-chaser-v2
- result: open

## Subtasks
1. task-282-01-texture-clean-heavy — 3d: Clean the heavy textures
2. task-282-02-texture-clean-kamikaze — 3d: Clean the kamikaze textures
3. task-282-03-texture-clean-shooter — 3d: Clean the shooter textures
4. task-282-04-texture-clean-chaser-v2 — 3d: Clean the chaser v2 textures

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-282-01-texture-clean-heavy]] — step 01, 3d.
- [[task-282-02-texture-clean-kamikaze]] — step 02, 3d.
- [[task-282-03-texture-clean-shooter]] — step 03, 3d.
- [[task-282-04-texture-clean-chaser-v2]] — step 04, 3d.

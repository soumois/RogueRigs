---
id: task-350-03-bp-turret-meshes
description: "BP_Turret shows SM_TurretBase and SM_TurretHead on the C++ HeadPivot at the grey-box height, flash at the canon tip, grey box and EventTick orbit removed."
parent: task-350-turret-in-game
kind: task
status: done
---
# BP_Turret: Meshy base and head on the C++ pivots

- goal: BP_Turret draws the new turret: a base mesh fixed on the actor and a head mesh under the C++ `HeadPivot`, uniformly scaled so the top of the head is as high as today's grey-box head top (owner Q2), the C++ `Muzzle` at the canon tip, the white-gold MuzzleFlash at the Muzzle (owner Q6); the grey-box components and the EventTick head yaw and barrel orbit are removed (C++ turns the head since subtask 01); OnTurretFired still shows the flash for 0.05 s.
- agent: bp
- depends_on: task-350-01-turret-head-yaw, task-350-02-turret-import
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md), bp-turret (vault/reference/tech/blueprints/player/bp-turret.md), sys-turret (vault/reference/tech/systems/combat/sys-turret.md), bp-objective-console (vault/reference/tech/blueprints/world/bp-objective-console.md, head under a scaled parent precedent), art-prompts-turret (vault/reference/gdd/art-audio/art-prompts-turret.md); decisions: global `request` and `gdd_check` (vault/reference/tasks/task-350-turret-in-game.md); subtask 01 change summary (HeadPivot and Muzzle API) and subtask 02 change summary (asset paths, read-back bounds) from the Executor.
- method: (1) Read back the current grey-box head top (TurretHead location and scale, about 96 uu) BEFORE removing anything; scale S = that height / SM_Turret total height (156.14 cm), about 0.61. (2) Add `TurretBaseMesh` (SM_TurretBase) under the root at scale S, location 0. (3) Set `HeadPivot` relative location = S x head offset in Unreal axes ((0.501, -0.518, 65.20) cm, Y mirrored; check against the imported bounds) and add `TurretHeadMesh` (SM_TurretHead) under `HeadPivot`, scale S, location 0, rotation 0 so the canon points along the pivot's +X. (4) Set `Muzzle` relative location = S x canon tip in the head frame (the front end of the canon on its axis, from the SM_TurretHead bounds and the art-prompts-turret canon axis); C++ takes only its horizontal position for the bullets (height stays `ShotHeight`, subtask 01), the flash shows at the Muzzle itself; report the muzzle world height against `ShotHeight` 50 uu. (5) Recreate `MuzzleFlash` (white-gold sphere, hidden at rest) under `Muzzle` (MCP cannot re-parent: delete and recreate). (6) Delete TurretBase, TurretBody, TurretHead, TurretBarrel grey-box components and the EventTick graph; keep OnTurretFired. (7) NoCollision and no overlap on every mesh; no CastShadow flag to set: C++ `RRShadows::DisableCastShadows` turns it off on every turret primitive at runtime. (8) Compile, save, read back.
- expected_output: BP_Turret updated and saved; change summary (components before and after with transforms, scale S and how it was computed, muzzle position, graph changes, materials seen on the meshes, MCP tools used, doc nodes to update: bp-turret).
- done_when: BP_Turret compiles with no error and is saved; read-back components: TurretBaseMesh, HeadPivot with TurretHeadMesh, Muzzle with MuzzleFlash, no grey-box mesh, no EventTick node; scaled head top within 3 uu of the grey-box head top read in step 1; all meshes NoCollision; committed by path as `task-350-03-bp-turret-meshes: <summary>`; editor LEFT OPEN.
- editor_state: open
- token_ceiling: 1.0 M units
- model: sonnet
- tokens: input 66, cache write 89417, cache read 1934171, output 23097
- usage: turns 33, context start 11k peak 94k, resumes 0, 3 min, units 0.42 M
- runs: 1
- milestone: M2
- started: 2026-10-28T16:21:19Z
- waiting_for: none
- result: Done: BP_Turret has TurretBaseMesh, HeadPivot+TurretHeadMesh, Muzzle+MuzzleFlash, scale S=0.6148 (head top 95.99 vs 96), grey box and EventTick removed, compiled, saved, NoCollision. Muzzle world height 58.8 vs ShotHeight 50 (flash 8.8 uu above bullet start). Not seen on screen. Commit 92a18603.

## Relations
- [[task-350-turret-in-game]] — parent global task.

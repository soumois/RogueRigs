---
id: task-334-near-asteroids
description: "Global task: other low-poly asteroids floating in the void around the arena asteroid, nearer the camera than the space layers, with depth parallax."
parent: tasks-to-do-index
kind: task
status: todo
---
# Nearer asteroids around the arena asteroid

- request: owner, 2026-10-27 (verbatim, French): "Il faut plus un effet nébuleuse et d'autres astéroid plus proche ( on est sur un astéroid )". Point 2 of 2 (asteroids); the nebula is [[task-333-nebula-visible]]. Reading: the arena is an asteroid, so other rocks float in the void around it, nearer the camera than the star, nebula and dust planes. No file given. Owner answer (2026-10-27, relayed verbatim by the main session): "ok", after being told "ok" accepts every default and confirms the plan, so A0 to A7 take their defaults (decided values written in [[visual-style]] and [[color-code]]). Later (2026-10-27, S8 of [[task-335-space-texture-layers]], owner "ok"): this task stays unchanged (3D Blender asteroids) and runs after task-335; the nebula plane at Z -5000 then carries the cloud texture, and a deep texture plane sits at Z -8000.
- goal: looking past the arena edge, the player sees a few separate low-poly asteroids floating below floor level in the void, at several depths, shifting against the space layers as the camera follows the miner, never hiding a unit or the floor and never mistaken for walkable ground.
- acceptance: asteroid meshes built and validated by the owner from a preview render (subtask 01); material compiles (02); meshes imported with the material, no collision (03); BP_AsteroidField placed and saved in L_ArenaM1, every asteroid over void cells and entirely below floor level (04); a `-game` capture near an arena edge at two miner positions shows the asteroids and their shift (05); doc nodes written (06); committed by explicit paths.
- gdd_check: read [[arena-grid]] ("Arena shape: an irregular asteroid inside the grid"; void cells have no floor and no block: consistent with the request), [[visual-style]] (void background line: "no Niagara dust or drifting debris", owner 2026-10-27 provisional; arena edge "a cliff into space with stars and a planet"), [[color-code]] (breakable rock light grey tinted violet; void palette), [[readability-rules]], [[art-3d-lessons]] ("Meshy is not the default", Blender route preferred), [[agent-3d]] (trigger is a validated image; Blender route only when the subtask says so), [[asset-pipeline]] (no asteroid budget defined), [[bp-space-background]] (planes at Z -2000, -5000, -12000), [[mpp-player-light-mask]], [[milestones]] (M3 polish, in scope). Conflict: the request supersedes Q6 and the "no drifting debris" line of task-332 (flagged, A0). Open values: A1 to A7. Questions, each with its default; owner "ok" (2026-10-27) = every default below is DECIDED (provisional values, tuned by eye later):
  - A0 GDD: default the visual-style line becomes "nearer low-poly asteroids float in the void around the arena asteroid"; no Niagara dust; planet still a later request.
  - A1 look: default lit, flat-shaded faceted low-poly rocks (same family as the arena rocks, light grey tinted violet, darker than the arena rocks so they read as background), no crystals, no texture. Alternative: dark unlit silhouettes.
  - A2 source: default Blender route by the 3d agent, no Meshy credit and no reference image (exception to the agent-3d trigger): a bpy script builds 4 variants, at most 400 triangles each, pivot at the centre; a preview render goes to the owner, who validates before import. Alternatives: an image by the art agent then Meshy; engine shapes only.
  - A3 count and depths: default 12 asteroids spread around the whole arena edge, 200 to 1200 uu across, tops between Z -300 and Z -2500 (all below the floor, above or near the dust plane), centres over void cells 300 to 3000 uu outside the floor edge, larger ones deeper; positions read from the layout void cells. Provisional.
  - A4 motion: default slow self-rotation, 1 to 5 deg/s each, through a RotatingMovement component (engine component, no graph logic); no translation drift.
  - A5 light and mask: default lit by the existing sun and sky, cast no shadow, receive no decal, no collision; the darkening mask applies to them like the rest of the screen.
  - A6 capture: default one `-game` run by cpp near an arena edge, one shot at each of two miner positions about 600 uu apart (to show the parallax shift), numbers measured, sheet sent to the owner (lessons rule 2026-10-16). No other run.
  - A7 one actor: default one data-only Blueprint BP_AsteroidField holding the 12 meshes, placed once in L_ArenaM1 (same pattern as BP_SpaceBackground), no C++ change.
- confirmed_by_owner: yes (2026-10-27, "ok", relayed by the main session)
- milestone: M3
- next_step: task-334-01-asteroid-meshes
- started: open
- waiting_for: none
- result: open

## Subtasks
1. task-334-01-asteroid-meshes (vault/reference/tasks/task-334-near-asteroids/task-334-01-asteroid-meshes.md) — 3d: Blender-route script, 4 low-poly asteroid FBX variants, report and preview render for the owner.
2. task-334-02-asteroid-material (vault/reference/tasks/task-334-near-asteroids/task-334-02-asteroid-material.md) — bp: M_Asteroid and MI_Asteroid, lit flat-coloured rock material.
3. task-334-03-asteroid-import (vault/reference/tasks/task-334-near-asteroids/task-334-03-asteroid-import.md) — bp: import SM_Asteroid_01 to 04 with MI_Asteroid, no collision.
4. task-334-04-asteroid-field (vault/reference/tasks/task-334-near-asteroids/task-334-04-asteroid-field.md) — bp: BP_AsteroidField with the 12 asteroids, placed in L_ArenaM1.
5. task-334-05-capture (vault/reference/tasks/task-334-near-asteroids/task-334-05-capture.md) — cpp: one `-game` run near an arena edge, two miner positions, sheet for the owner.
6. task-334-06-docs (vault/reference/tasks/task-334-near-asteroids/task-334-06-docs.md) — doc: doc nodes of M_Asteroid and BP_AsteroidField, world index, bp-arena-grid map line.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-334-01-asteroid-meshes]] — step 1, 3d.
- [[task-334-02-asteroid-material]] — step 2, bp.
- [[task-334-03-asteroid-import]] — step 3, bp.
- [[task-334-04-asteroid-field]] — step 4, bp.
- [[task-334-05-capture]] — step 5, cpp.
- [[task-334-06-docs]] — step 6, doc.
- [[task-333-nebula-visible]] — point 1 of the same owner message.
- [[task-332-space-background]] — the space layers these asteroids sit in front of; its Q6 is superseded.
- [[bp-space-background]] — plane depths the asteroids are placed against.

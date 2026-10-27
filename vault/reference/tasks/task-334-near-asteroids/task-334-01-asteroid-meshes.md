---
id: task-334-01-asteroid-meshes
description: "Blender route: a bpy script builds four low-poly faceted asteroid variants, exports FBX with reports and a preview render for owner validation."
parent: task-334-near-asteroids
kind: task
status: done
---
# Asteroids: meshes (Blender route)

- goal: four export-ready low-poly asteroid meshes the owner has validated from a preview render. Blender route, named here (A2): no Meshy call, no credit, no reference image.
- agent: 3d
- depends_on: none
- inputs: the global `request` and `gdd_check` with the owner answers A0 to A7 (vault/reference/tasks/task-334-near-asteroids.md); blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md); asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md, FBX units, report); art-3d-lessons (vault/reference/agents/art-3d-lessons.md, FBX_SCALE_UNITS, stop Blender processes); art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md); rock-look-reference (vault/reference/gdd/art-audio/rock-look-reference.md, rock family); color-code (vault/reference/gdd/art-audio/color-code.md); visual-style (vault/reference/gdd/art-audio/visual-style.md).
- expected_output: `Tools/art/blender/asteroid.py` (deterministic seeds, headless Blender from `BLENDER_EXE`) building 4 irregular faceted rocks (displaced low-subdivision sphere or convex hull of jittered points, flat shading, one material slot, no texture, no UV beyond channel 0), at most 400 triangles each, 100 uu (1 m) across before scaling, pivot at the bounding-box centre; `Art/Meshes/Asteroid/SM_Asteroid_01.fbx` to `_04.fbx` in centimetres with one `.report.json` each (tris, dimensions in cm, islands, materials, UVs, warnings); one preview render `Art/Meshes/Asteroid/preview.png` of the 4 variants side by side, top-down camera at -50 degrees pitch, one directional light, neutral grey background, flat violet-grey colour; an asset node `asteroid-asset` in vault/reference/tech/pipelines/ (files, budget, seeds, how to rebuild) listed in pipelines-index (vault/reference/tech/pipelines/pipelines-index.md). Blender processes stopped before the report.
- done_when: 4 FBX and 4 reports exist, each at most 400 triangles, one island, dimensions about 100 cm; the preview exists and the Executor has sent it to the owner, who validated it (until then the Executor sets this subtask and the global `wait-answers`, "waiting_for=owner validation of the asteroid preview"); no `blender` process left (`Get-Process blender`); asset node linked, lint clean; committed by explicit paths.
- editor_state: any
- token_ceiling: 1.2 M units
- model: sonnet
- tokens: input 26, cache write 42956, cache read 396910, output 10426
- usage: turns 13, context start 10k peak 46k, resumes 0, 2 min, units 0.15 M
- runs: 1
- milestone: M3
- started: 2026-10-27T23:21:20Z
- waiting_for: none
- result: 4 asteroid FBX (<=400 tris) built by Blender script, preview validated by owner (2026-10-27: 'Les 4 asteroides sont valides, continue la tache 334')

## Relations
- [[task-334-near-asteroids]] — parent global task.

---
id: task-347-01-turret-flat-surfaces
description: "Project the optimised turret vertices of each planar region of the unoptimised mesh onto its plane, canon excluded, and render before, after and reference."
parent: task-347-turret-flat-surfaces
kind: task
status: todo
---
# Turret mesh: flatten the bulged surfaces

- goal: In `Art/Meshes/turret/SM_Turret.fbx`, the surfaces that are flat on the unoptimised Meshy turret are flat again (global `gdd_check` Q3), UVs, textures and canon unchanged.
- agent: 3d
- depends_on: task-345-01-turret-meshy-files, task-346-01-turret-canon
- inputs: agent-3d (vault/reference/agents/agent-3d.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), turret (vault/reference/gdd/weapons/turret.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), mesh-fidelity-loop (vault/reference/tech/pipelines/mesh-fidelity-loop.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), art-prompts-turret (vault/reference/gdd/art-audio/art-prompts-turret.md); decisions: global `request` and `gdd_check` (vault/reference/tasks/task-347-turret-flat-surfaces.md); files: `Art/Meshes/turret/SM_Turret.fbx` and `turret_canon.report.json` (task-346 output, canon faces = material slot `turret_canon` or the selection logged there), `Art/Source/turret-meshy/meshy-unoptimised/` (reference), `Art/Source/turret-meshy/meshy-optimised/` (untouched source, for before renders), `Tools/art/blender/turret_preview.py`.
- method: one headless Blender script `Tools/art/blender/turret_flatten.py`. (1) First check what makes the faces look bulged: vertex positions off-plane, smooth or custom split normals, or the Meshy normal map; log the measured share of each (max distance to the fitted plane, normal deviation, normal-map tilt on those regions). (2) Detect planar regions on the unoptimised mesh with the Q3 thresholds (log thresholds, region count, area). (3) Map each region to the optimised faces lying on it (nearest surface, same normal side), canon faces excluded. (4) Fit one plane per region on the unoptimised faces and project the mapped optimised vertices onto it; a vertex shared by two regions goes to the intersection line; border vertices keep the silhouette within 1 % of the turret height. (5) Flat shading on those faces; if the normal map causes the bulge, report it with numbers and do not edit the texture (separate owner decision). (6) UVs and maps untouched; export over `Art/Meshes/turret/SM_Turret.fbx`, centimetres, pivot bottom centre. (7) Renders in `Saved/TurretMesh/flat/` (not committed) with the task-346 cameras plus one grazing light: optimised source, after, unoptimised reference. (8) Add the flattening result to the "Game mesh" section of art-prompts-turret. (9) Stop every Blender process.
- expected_output: `Tools/art/blender/turret_flatten.py`; updated `Art/Meshes/turret/SM_Turret.fbx`; `Art/Meshes/turret/turret_flat.report.json` (cause measurements, thresholds, regions, vertices moved, max residual distance to plane per region, tris, dimensions, warnings); renders in `Saved/TurretMesh/flat/`; art-prompts-turret updated.
- done_when: every flattened region has a residual distance to its plane under 0.01 cm; canon vertex positions (shroud holes included) identical to task-346 output; total triangles under 16,000 (owner 2026-10-28), no decimation; UV coordinates identical; bounding box within 1 % of the turret height of the input; report names the cause of the bulge with numbers; renders exist and their paths are reported so the Executor sends them to the owner; art-prompts-turret updated; lint clean; committed by path as `task-347-01-turret-flat-surfaces: <summary>`; `Get-Process blender` empty.
- editor_state: any
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-347-turret-flat-surfaces]] — parent global task.

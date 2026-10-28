---
id: task-346-01-turret-canon
description: "Replace the optimised Meshy turret canon by a graft or a Blender rebuild after the unoptimised canon (Q1); render before, after, reference."
parent: task-346-turret-canon
kind: task
status: todo
---
# Turret mesh: replace the optimised canon

- goal: `Art/Meshes/turret/SM_Turret.fbx` = the optimised Meshy turret with its canon replaced by one that follows the unoptimised canon, rebuilt by Blender script (owner Q1 = B), its outer barrel layer perforated with real holes as on the unoptimised mesh (global `gdd_check` Q2, Holes, Q6).
- agent: 3d
- depends_on: task-345-01-turret-meshy-files
- inputs: agent-3d (vault/reference/agents/agent-3d.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), turret (vault/reference/gdd/weapons/turret.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), mesh-fidelity-loop (vault/reference/tech/pipelines/mesh-fidelity-loop.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), color-code (vault/reference/gdd/art-audio/color-code.md), art-prompts-turret (vault/reference/gdd/art-audio/art-prompts-turret.md, created by task-345-01); decisions: global `request` and `gdd_check` (vault/reference/tasks/task-346-turret-canon.md); files: `Art/Source/turret-meshy/meshy-optimised/` and `meshy-unoptimised/` (six Meshy files each), `Art/Source/turret-meshy/turret_meshy_identify.report.json`.
- method: one headless Blender script `Tools/art/blender/turret_canon.py`. (1) Locate the canon in both meshes (geometry selection by position and axis; log the rule used) and measure the unoptimised canon: axis, length, outer and bore radius, segment changes, attachment point, and the hole pattern of the second (outer) layer: hole shape (round or rectangular), count per row, number of rows, hole size and spacing, layer thickness and gap to the inner barrel. (2) Remove the optimised canon faces. (3) Q1 = B (decided): build the new canon in bpy from those measurements, flat facets, flat shading, straight edges, enough sides to read round from the top-down camera: an inner barrel and a second (outer) shroud cylinder with real holes cut through it (geometry, not texture), hole pattern as measured, hole walls closed, the inner barrel visible through the holes. (4) Q2 (decided default): material slot `turret_canon`, base colour, normal and roughness 1024 px baked (selected to active) from the unoptimised canon textures; the optimised atlas and its UVs stay untouched. (5) Everything outside the canon keeps its vertices and UVs. (6) Export FBX in centimetres (`FBX_SCALE_UNITS`), pivot bottom centre, with its maps in `Art/Meshes/turret/`. (7) Previews with `Tools/art/blender/turret_preview.py` in `Saved/TurretMesh/canon/` (not committed): same camera, light and scale for optimised before, after and unoptimised reference, views front, side, three-quarter top and a canon close-up. (8) Bounded comparison against the unoptimised canon per mesh-fidelity-loop. (9) Add a "Game mesh" section to art-prompts-turret: file list, method used, tris, dimensions. (10) Stop every Blender process.
- expected_output: `Tools/art/blender/turret_canon.py`, `Tools/art/blender/turret_preview.py`; `Art/Meshes/turret/SM_Turret.fbx` with its maps; `Art/Meshes/turret/turret_canon.report.json` (tris before and after, canon tris, dimensions in cm, materials, UVs, canon measurements in both meshes, warnings); renders in `Saved/TurretMesh/canon/`; art-prompts-turret updated.
- done_when: report shows canon length, radii, position and axis within 2 % of the turret height of the unoptimised canon; vertex count outside the canon equals the optimised source; FBX reimported in a clean Blender scene has the expected dimensions in cm and pivot at bottom centre; materials per Q2; renders exist for the four views (before, after, reference) and their paths are reported so the Executor sends them to the owner; report gives the measured hole pattern of the unoptimised shroud and of the new one (shape, rows, count per row, size, spacing; counts equal, sizes and spacing within 10 %); holes are open in the geometry (boundary loops or hole walls listed in the report, a ray through a hole centre reaches the inner barrel); after and close-up renders show the holes; total triangles under 16,000 (Q6, owner 2026-10-28), body outside the canon unchanged, nothing decimated; art-prompts-turret updated; lint clean; committed by path as `task-346-01-turret-canon: <summary>`; `Get-Process blender` empty.
- editor_state: any
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-28T06:32:49Z
- waiting_for: none
- result: open

## Relations
- [[task-346-turret-canon]] — parent global task.

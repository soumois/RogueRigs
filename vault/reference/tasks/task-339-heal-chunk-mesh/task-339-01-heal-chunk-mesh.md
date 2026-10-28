---
id: task-339-01-heal-chunk-mesh
description: "3D, Blender route: SM_HealChunk_01..03 from the chosen heal pickup sheet, 30 cm, centred pivot, two red slots, FBX, report and render."
parent: task-339-heal-chunk-mesh
kind: task
status: in-progress
---
# Heal pickup chunks: Blender meshes

- goal: Blender route (no Meshy, no credit; owner 2026-10-28 "en blender"): one script `Tools/art/blender/heal_chunk.py` that builds the three heal pickup chunk variants SM_HealChunk_01, _02, _03 from the chosen sheet `vault/assets/heal-pickup-sheet.png` (left, middle, right piece), exports one FBX and one report each. Values (global `gdd_check` Q3, provisional): longest side 30 cm like the current 30 uu cube and the Duskite chunk; pivot at the geometric centre of the bounds (pickup root moved by SetActorLocation and simulated by Chaos, as SM_OreChunk, [[bp-pickup-ore]]); triangle budget 300 target, 500 max per variant (ore chunk pickup budget, [[asset-pipeline]]); two material slots `crystal_bright` (prism faces and the small tip) and `crystal_dark` (fracture faces of the body), same slot names and order on the three variants (the Blueprint relies on slot defaults, [[sys-pickups-lifecycle]] material rule); flat shading, no texture; UV0 `UVMap` and UV1 `LightMap` per [[blender-asset-route]]; export with `FBX_SCALE_UNITS`, 1 unit = 1 cm (task-189 lost one run on a 100x scale). Shape per Q1 (owner "ok" 2026-10-28): follow the sheet (two whole prisms on a wide fracture body, not snapped-off stumps) with a flat resting face underneath so the chunk lies and rolls a little under physics.
- fidelity_criteria (from `vault/assets/heal-pickup-sheet.png`, [[mesh-fidelity-loop]]):
  - C1: each variant is one compact solid piece; longest side 30 cm (+-1); height / largest horizontal width 0.8 to 1.2.
  - C2: two main faceted prisms (about six sides) with pointed tips rise side by side from the top of the body, touching at their base, leaning at most 20 degrees from vertical, not splayed like a flower; they stand above the body for 40 to 55 percent of the total height.
  - C3: the second prism is 75 to 100 percent of the height of the first (sheet: left about 85, middle about 80, right about 95).
  - C4: a small third tip, at most 25 percent of the total height, on the front of the body: 01 front centre, 03 front left, none on 02.
  - C5: the body under the prisms is a wide lump of large angular flat chips with jagged edges and a few notches, reading as a fresh fracture of the same crystal, not as a separate rock; the two prisms together span 55 to 75 percent of the body width; the bottom has one flat resting face covering at least 40 percent of the footprint.
  - C6: in the render, prisms and tip bright pure red, fracture faces a darker duller pure red; no orange, no pink.
  - C7: flat shading, large facets, no smoothing, no texture.
  - C8: the three variants share their bounding size within 10 percent per axis; they differ only by prism lean, relative heights and the small tip.
- max_returns: 5 ([[mesh-fidelity-loop]]).
- render: orthographic, yaw -35, elevation 35 (the sheet camera, mid-height three-quarter view), light-grey background (A16), the sun and world of [[blender-asset-route]]; one 3-up render of the three variants side by side in the sheet order, same scale, `Art/Meshes/HealChunk/heal_chunk_preview.png`, plus the fidelity renders the loop needs.
- agent: 3d
- depends_on: task-336-01-heal-crystal-variants (the owner-chosen sheet copied to `vault/assets/heal-pickup-sheet.png`)
- inputs: agent-3d (vault/reference/agents/agent-3d.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), mesh-fidelity-loop (vault/reference/tech/pipelines/mesh-fidelity-loop.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), color-code (vault/reference/gdd/art-audio/color-code.md), heal-ore (vault/reference/gdd/terrain/heal-ore.md), art-prompts-heal-pickup (vault/reference/gdd/art-audio/art-prompts-heal-pickup.md), bp-pickup-ore (vault/reference/tech/blueprints/fx/bp-pickup-ore.md), sys-pickups-lifecycle (vault/reference/tech/systems/world/sys-pickups/sys-pickups-lifecycle.md); image `vault/assets/heal-pickup-sheet.png`; script model `Tools/art/blender/ore_chunk.py` (slot names, centred pivot, cm export)
- expected_output: `Tools/art/blender/heal_chunk.py`; `Art/Meshes/HealChunk/SM_HealChunk_01.fbx`, `_02.fbx`, `_03.fbx`, each with its `.report.json` (tris, dimensions_cm, materials, uv_layers, uv_checks, warnings, `fidelity` object with returns_used, per-criterion verdict and evidence, final_render); `Art/Meshes/HealChunk/heal_chunk_preview.png`; [[blender-asset-route]] Scripts line gains `heal_chunk.py` (3d writes its own nodes).
- done_when: three FBX and reports exist; each within 500 tris, longest side 30 cm (+-1), pivot centred, slots crystal_bright and crystal_dark in the same order on the three; every fidelity criterion pass, or open failures listed after 5 returns; no Blender process left (`Get-Process blender`); the Executor sends the preview to the owner with SendUserFile and the owner accepts the meshes (one return to the agent at most before asking him); committed by path.
- editor_state: closed
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-28T01:25:53Z
- waiting_for: none
- result: open

## Relations
- [[task-339-heal-chunk-mesh]] — parent global task.

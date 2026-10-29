---
id: task-368-01-extractor-rebake-maps
description: "3d: re-unwrap SM_Extractor (geometry kept), bake the four Meshy maps from the dense mesh onto it, report stretch and fidelity, render source, old and new."
parent: task-368-extractor-rebake
kind: task
status: done
---
# Extractor mesh: re-bake the Meshy textures on clean UVs: 3d

- goal: From `Art/Meshes/extractor/SM_Extractor.fbx` and the raw Meshy FBX, the same mesh with a new UV0 and four baked maps, per the global Q1 to Q3.
- agent: 3d
- depends_on: none
- inputs: agent-3d (vault/reference/agents/agent-3d.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), art-prompts-extractor (vault/reference/gdd/art-audio/art-prompts-extractor.md), mesh-fidelity-loop (vault/reference/tech/pipelines/mesh-fidelity-loop.md), task-366-02-extractor-flatten (vault/reference/tasks/task-366-extractor-flat-faces/task-366-02-extractor-flatten.md), decisions: global `request`, `gdd_check` and `## Questions` (vault/reference/tasks/task-368-extractor-rebake.md); Q2, Q4, Q5 of task-366 (vault/reference/tasks/task-366-extractor-flat-faces.md), files: `Art/Meshes/extractor/SM_Extractor.fbx`, `Art/Meshes/extractor/extractor_flat.report.json` (regions, scale, rotation), `Art/Meshes/extractor/raw/` FBX, `Art/Source/extractor-meshy/` maps, `Tools/art/blender/extractor_flatten.py`, `Tools/art/blender/extractor_preview.py`, `Tools/art/blender/rr_blender.py` (header docstring only)
- expected_output: `Tools/art/blender/extractor_rebake.py` (imports rr_blender, never redefines its helpers); `Art/Meshes/extractor/SM_Extractor.fbx` replaced (centimetres, `FBX_SCALE_UNITS`, one slot, one UV layer UVMap); `T_Extractor_BaseColor|Metallic|Normal|Roughness.png` replaced (2048 x 2048, base colour sRGB, others non-colour); `Art/Meshes/extractor/extractor_rebake.report.json` (UV method, seams, island count, margin, overlap count, stretch old versus new, bake settings: extrusion, max ray distance, missed rays, dilation; normal convention; normal tilt on the flattened regions; colour difference source versus old and versus new from the same cameras; geometry check; warnings); renders in `Saved/ExtractorMesh/rebake/` (not committed): Meshy source, old SM_Extractor, re-baked SM_Extractor, same cameras and light, plus close-ups of two large rusty panels; "Re-bake" section added to art-prompts-extractor.
- done_when: reimport in a clean scene: 47,000 triangles, vertex positions identical to the task-366 SM_Extractor.fbx (max difference 0.001 cm), same flat-shaded faces; one UV layer, every face mapped, 0 overlapping islands, all UVs in 0-1, margin at least 8 px at 2048; stretch metric lower than the old UV0 (numbers in the report); four 2048 x 2048 PNG, no unbaked pixel inside an island (missed rays reported, 0 or located); normal-map tilt on the flattened regions not above 3.7 deg mean; colour difference to the source lower than the old SM_Extractor from the same cameras; renders exist and their paths are reported so the Executor sends them to the owner; art-prompts-extractor updated; lint clean; committed by path as `task-368-01-extractor-rebake-maps: <summary>`; `Get-Process blender` empty.
- editor_state: any
- token_ceiling: 1.2 M units
- model: sonnet
- tokens: input 126, cache write 306118, cache read 7459648, output 115696
- usage: turns 62, context start 12k peak 204k, resumes 1 (turn-limit x1), 51 min, units 1.71 M
- runs: 2
- milestone: M2
- started: 2026-10-29T19:07:34Z
- waiting_for: none
- result: SM_Extractor.fbx re-unwrapped (4631 islands, 0 overlap, 0 flipped vs 534 old, L2 stretch 1.0009 vs 1.0802) and four 2048 maps baked from the dense Meshy mesh (commit 2dfff06a); geometry identical (47,000 tri, 0.0 cm). Colour difference to source 3.7 vs 10.5 (old) over 5 views, lower in all 7. Missed rays 1114 px (0.13%) filled from neighbours, 0 left. DEVIATION: mean normal tilt on flat faces 4.44 deg (above the 3.7 limit; 3.18 deg excluding dense-mesh relief pixels over 30 deg, baked on purpose per Q2), reported in warnings. ALERT turn limit: first 3d run hit 60 turns (72 tool calls, ~3000 s) with no report, resumed by SendMessage, work finished and committed; runs 2.

## Relations
- [[task-368-extractor-rebake]] — parent global task.

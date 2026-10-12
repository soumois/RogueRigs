---
id: task-101-09-barrel-lid-logo
description: "The Kessler-Voss plate sticker moves from the body to the top of the lid (owner 2026-10-11)."
parent: task-101-barrel-asset
kind: task
status: done
---
# The Kessler-Voss plate sticker moves from the body to the to

- goal: The Kessler-Voss plate sticker moves from the body to the top of the lid (owner 2026-10-11).
- agent: 3d
- depends_on: task-101-08-barrel-warning-sign
- inputs: agent-3d (vault/reference/agents/agent-3d.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md); files Tools/art/blender/barrel.py, Tools/art/blender/barrel_preview.py, Art/Meshes/barrel/SM_Barrel.report.json
- expected_output: Blender route, no credit. In barrel.py move the Kessler-Voss sticker decal (T_Barrel_Sticker.png, M_Barrel_sticker, 28 x 11.5 cm) from the body facet to the top face of the lid: flat on the lid top, offset about 0.25 cm, centred on the lid left of the cap, oriented to read upright from the front (the side the camera sees: azimuth -35 degrees), not touching the cap, the bolts or the edge bands. The body facet it leaves is clean. Keep the scratch mask on the lid. Recheck UVMap and LightMap numerically, keep 90 x 90 x 134 cm, pivot, 600 triangles max; update barrel_preview.py and SM_Barrel.report.json; run the preview once to the scratchpad without opening it. Commit by name.
- done_when: the sticker sits on the lid top, upright from the front, not overlapping the cap or the bands; no sticker left on the body; tris under 600; dimensions and pivot unchanged; UV and lightmap checks pass; barrel_preview.py runs; committed by name.
- editor_state: any
- token_ceiling: 35000
- model: open
- tokens: open
- runs: 1
- milestone: M2
- result: Kessler-Voss sticker moved onto the lid top. Commit c519e31. Cost counted with task-101-06 (same resumed agent).

## Relations
- [[task-101-barrel-asset]] — parent global task.

---
id: task-101-11-barrel-logo-130
description: "The Kessler-Voss sticker on the lid is 30 percent larger (owner 2026-10-11)."
parent: task-101-barrel-asset
kind: task
status: todo
---
# The Kessler-Voss sticker on the lid is 30 percent larger (ow

- goal: The Kessler-Voss sticker on the lid is 30 percent larger (owner 2026-10-11).
- agent: 3d
- depends_on: task-101-10-barrel-sign-double
- inputs: agent-3d (vault/reference/agents/agent-3d.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md); files Tools/art/blender/barrel.py, Tools/art/blender/barrel_preview.py, Art/Meshes/barrel/SM_Barrel.report.json
- expected_output: Blender route, no credit. In barrel.py scale the Kessler-Voss lid sticker by 1.3 in width and height (28 x 11.5 cm to about 36.4 x 15 cm), same centre, still flat on the lid, still not touching the cap, the bolts or the edge bands (move it slightly if needed and say so). Recheck UVMap and LightMap numerically, keep 90 x 90 x 134 cm, pivot, 600 triangles max; update barrel_preview.py and SM_Barrel.report.json; run the preview once to the scratchpad without opening it. Commit by name.
- done_when: sticker is 1.3 times larger, on the lid, not overlapping cap, bolts or bands; tris under 600; dimensions and pivot unchanged; UV and lightmap checks pass; committed by name.
- editor_state: any
- token_ceiling: 25000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-101-barrel-asset]] — parent global task.

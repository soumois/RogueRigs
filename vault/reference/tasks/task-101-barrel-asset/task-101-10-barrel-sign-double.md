---
id: task-101-10-barrel-sign-double
description: "The explosion warning sign is twice as large (owner 2026-10-11)."
parent: task-101-barrel-asset
kind: task
status: done
---
# The explosion warning sign is twice as large (owner 2026-10-

- goal: The explosion warning sign is twice as large (owner 2026-10-11).
- agent: 3d
- depends_on: task-101-09-barrel-lid-logo
- inputs: agent-3d (vault/reference/agents/agent-3d.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md); files Tools/art/blender/barrel.py, Tools/art/blender/barrel_preview.py, Art/Meshes/barrel/SM_Barrel.report.json
- expected_output: Blender route, no credit. In barrel.py scale the explosion warning sign decal by 2 in width and height (about 26 cm to about 52 cm wide), keeping its centre on the visible vertical corner; it may wrap over more facets; if the doubled sign no longer fits in the band between the rings, extend the decal copies to the ring faces it crosses (same offset, same single planar UV projection) rather than shrinking it, and say so in the report. Recheck UVMap and LightMap numerically, keep 90 x 90 x 134 cm, pivot, 600 triangles max; update barrel_preview.py and SM_Barrel.report.json; run the preview once to the scratchpad without opening it. Commit by name.
- done_when: sign is 2 times larger, centred on the same corner, one UV projection; tris under 600; dimensions and pivot unchanged; UV and lightmap checks pass; committed by name.
- editor_state: any
- token_ceiling: 25000
- model: open
- tokens: open
- runs: 1
- milestone: M2
- result: Sign doubled (about 52 cm), decal extended over ring faces. Commit e5bbe5f. Cost counted with task-101-06 (same resumed agent).

## Relations
- [[task-101-barrel-asset]] — parent global task.

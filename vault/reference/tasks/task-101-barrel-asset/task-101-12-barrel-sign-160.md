---
id: task-101-12-barrel-sign-160
description: "The explosion warning sign is 60 percent larger again (owner 2026-10-11)."
parent: task-101-barrel-asset
kind: task
status: done
---
# Sign 60 percent larger

- goal: The explosion warning sign is 60 percent larger again (owner 2026-10-11).
- agent: 3d
- depends_on: task-101-11-barrel-logo-130
- inputs: agent-3d (vault/reference/agents/agent-3d.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md); files Tools/art/blender/barrel.py, Tools/art/blender/barrel_preview.py, Art/Meshes/barrel/SM_Barrel.report.json
- expected_output: Blender route, no credit. In barrel.py scale the explosion warning sign decal by 1.6 in width and height relative to its current size (about 52 cm to about 83 cm wide), same centre. It will cross several facets and both rings: keep the single planar UV projection but cover only the faces whose normal is within about 70 degrees of the projection direction (no stretched or mirrored faces, nothing past the silhouette), and report which faces it covers. Recheck UVMap and LightMap numerically (new decal faces get lightmap islands), dimensions 90 x 90 x 134 cm (decal offset aside), pivot, 600 triangles max; update barrel_preview.py and SM_Barrel.report.json; run the preview once to the scratchpad without opening it. Commit by name.
- done_when: sign is 1.6 times larger, same centre, no stretched or mirrored faces; tris under 600; pivot unchanged; UV and lightmap checks pass; committed by name.
- editor_state: any
- token_ceiling: 25000
- model: open
- tokens: open
- runs: 1
- milestone: M2
- result: Sign 1.6 times larger (83.2 cm), 36 decal faces, same centre. Commit 45d1433. Cost counted with task-101-06 (same resumed agent).

## Relations
- [[task-101-barrel-asset]] — parent global task.

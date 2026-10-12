---
id: task-101-13-barrel-sign-recentre
description: "The explosion sign is raised so that its burst centre sits at mid-height between the rings (owner 2026-10-11)."
parent: task-101-barrel-asset
kind: task
status: done
---
# Sign recentred in height

- goal: The explosion sign is raised so that its burst centre sits at mid-height between the rings (owner 2026-10-11).
- agent: 3d
- depends_on: task-101-12-barrel-sign-160
- inputs: agent-3d (vault/reference/agents/agent-3d.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md); files Tools/art/blender/barrel.py, Tools/art/blender/barrel_preview.py, Art/Meshes/barrel/SM_Barrel.report.json
- expected_output: Owner: the explosion sign sits too low (a triangle with its apex up has its mass low, so it looked low and bit on the lower ring). Blender route, no credit. In barrel.py move the sign up (same size, same horizontal position on the corner, same single planar UV projection, faces recomputed) so that the centre of the explosion burst in the texture (not the bounding box of the triangle) is at the vertical middle of the band between the top of the lower ring and the bottom of the upper ring; report the ring heights, the burst centre and the resulting z. Keep it within the faces whose normal is within about 70 degrees of the projection direction. Recheck UVMap and LightMap numerically, pivot, 600 triangles max; update barrel_preview.py and SM_Barrel.report.json; run the preview once to the scratchpad without opening it. Commit by name.
- done_when: the burst centre is at mid-height between the rings (numbers in the report), same size and corner; tris under 600; pivot unchanged; UV and lightmap checks pass; committed by name.
- editor_state: any
- token_ceiling: 25000
- model: open
- tokens: open
- runs: 1
- milestone: M2
- result: Burst centre at z 65.5 cm, middle of the band between the rings; 498 tris. Commit 93ec8d1. Cost counted with task-101-06 (same resumed agent).

## Relations
- [[task-101-barrel-asset]] — parent global task.

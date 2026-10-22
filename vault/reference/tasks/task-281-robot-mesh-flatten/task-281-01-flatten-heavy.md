---
id: task-281-01-flatten-heavy
description: "Flatten the heavy panels (3d)."
parent: task-281-robot-mesh-flatten
kind: task
status: in-progress
---
# Flatten the heavy panels

- goal: Write Tools/art/blender/meshy_flatten_panels.py (headless, Blender route: no Meshy call) and run it on the raw heavy mesh: find the regions that should be planar (hard-surface panels: near-coplanar face groups bounded by sharp edges) and project their vertices onto the fitted plane, keeping silhouettes, creases and UVs.
- agent: 3d
- depends_on: none
- inputs: art-3d-lessons (vault/reference/agents/art-3d-lessons.md), agent-3d (vault/reference/agents/agent-3d.md), robots-meshy-raw-2026-10-22 (vault/reference/tech/pipelines/robots-meshy-raw-2026-10-22.md), art-prompts-robots-meshy (vault/reference/gdd/art-audio/art-prompts-robots-meshy.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md); files: Art/Source/heavy-meshy/meshy-raw/heavy_meshy_raw.fbx, Art/Source/heavy-meshy/meshy-raw/heavy_meshy_raw_basecolor.png, sheet vault/assets/heavy-turnaround.webp
- expected_output: Art/Meshes/heavy-meshy/heavy_meshy_flat.fbx (FBX_SCALE_UNITS, same object, same single UV and material) and Art/Meshes/heavy-meshy/heavy_meshy_flat.report.json (planar regions found, per region max deviation before and after in cm, max vertex move in cm, tris and verts before and after, UV0 delta 0)
- done_when: report: tris and verts unchanged, UV0 unchanged, deviation from plane after flattening under 0.1 cm (scale 1.9 m) on every listed region, no vertex moved more than 2 cm, no flipped normal; the Orchestrator renders the same camera (barrel_preview.py settings scaled to a 190 cm robot: front three-quarter and back three-quarter) before and after and sends both to the owner; Blender processes stopped (Get-Process blender)
- editor_state: closed (Blender headless only)
- token_ceiling: 60000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-281-robot-mesh-flatten]] — parent global task.

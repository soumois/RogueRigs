---
id: agent-3d
description: "3D agent: turns an owner-validated image into a cleaned, split FBX through Meshy or Tripo and Blender MCP. Invoke after an image is validated."
parent: agents-index
kind: spec
status: provisional
---
# 3D agent

- role: turn a validated source image into an export-ready mesh.
- invoked_by: orchestrator
- trigger: an image the owner has validated (after Assignment #4).
- reads: its subtask node, [[asset-pipeline]], [[art-style-guide]], and the entity node the subtask names.
- produces: the generated model and the cleaned FBX in `Art/Meshes/<asset>/`; a report with polygon count, parts and bones.
- output_format: FBX. Generation: Meshy API for robots, drone and turret; Tripo API for the miner (auto-rig); cleanup, decimation and rigid split in Blender MCP. Keys read from `MESHY_API_KEY` and `TRIPO_API_KEY`. Polygon budget, scale and pivot: open.
- definition_of_done: the owner validates the mesh; [[agent-bp]] then imports it through Unreal MCP.
- must_not: start a paid generation from an image the owner has not validated; import into Unreal; write a key anywhere.
- model: sonnet
- token_ceiling: open

## Relations
- [[agents-index]] — parent index.
- [[agent-art]] — produces its input image.
- [[agent-bp]] — imports its output.

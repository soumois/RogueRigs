---
id: agent-3d
description: "3D agent: validated image to export-ready mesh, via Meshy then Blender cleanup, or a Blender script alone when the subtask says so."
parent: agents-index
kind: spec
status: provisional
---
# 3D agent

- role: turn a validated source image into an export-ready mesh.
- invoked_by: orchestrator
- trigger: a subtask naming an image the owner has validated (in `vault/assets/`).
- reads: its subtask node, [[asset-pipeline]], [[art-style-guide]], [[blender-asset-route]] (Blender route only), and the entity node the subtask names.
- produces: the raw mesh and its run log in `Art/Meshes/<asset>/raw/`; the cleaned FBX `Art/Meshes/<asset>/<SM_Name>.fbx` and its `.report.json` (tris in and out, dimensions in cm, islands, materials, UVs, warnings).
- output_format: generation through `node Tools/art/meshy_3d.js`: Meshy image-to-3D, smart-topology mode (`meshy-t2`), base color texture on; it refuses an image outside `vault/assets/`, and `--resume <task-id>` downloads an interrupted task without paying again. Cleanup through `Tools/art/blender/cleanup.py` in headless Blender (path in `BLENDER_EXE`): vertex merge, small islands removed, decimation to budget, flat shading, height, pivot, FBX in centimetres. Polygon budget, height and pivot: [[asset-pipeline]]. Key read from `MESHY_API_KEY`.
- blender_route (owner 2026-10-11, simple props such as the barrel): when the subtask says "Blender route", no Meshy call and no credit: the agent writes `Tools/art/blender/<asset>.py` (bpy) that builds the mesh from the validated image (flat shading, flat per-material colours, small textured decals only for stickers and signs, A1 as amended), then exports `Art/Meshes/<asset>/<SM_Name>.fbx` in centimetres and its `.report.json` (tris, dimensions, materials, UVs, warnings) with the same headless Blender as the cleanup.
- definition_of_done: the report is within budget and dimensions; the owner validates the mesh; [[agent-bp]] then imports it through Unreal MCP.
- must_not: use the Blender route without the subtask naming it; start a paid generation from an image outside `vault/assets/`; start a second paid generation for the same image unless the subtask allows it; split into rigid parts or rig without a parts and bones list in the subtask; import into Unreal; call a viewport capture tool; write a key anywhere.
- definition_file: `.claude/agents/3d.md`
- model: sonnet
- token_ceiling: open (measured on the first run)

Tripo (auto-rig for the miner) is deferred to the miner task: its v3 API fields are not verified. Blender MCP is not used.

## Relations
- [[agents-index]] — parent index.
- [[agent-art]] — produces its input image.
- [[agent-bp]] — imports its output.
- [[asset-pipeline]] — budgets, height, pivot and the pipeline steps.
- [[blender-asset-route]] — the Blender route: scripts, UV channels, mask contract, preview.

---
id: task-151-12-chaser-assembly
description: "Assemble the ten parts and validate the whole chaser against the sheet."
parent: task-151-chaser-robot
kind: task
status: in-progress
---
# Chaser: assembly and validation

- goal: Assemble the ten parts and validate the whole chaser against the sheet.
- agent: 3d
- depends_on: task-151-06-chaser-part-headtorso, task-151-07-chaser-part-pelvis, task-151-08-chaser-part-upperarm, task-151-09-chaser-part-forearmhand, task-151-10-chaser-part-thigh, task-151-11-chaser-part-shinfoot
- inputs: asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), mesh-fidelity-loop (vault/reference/tech/pipelines/mesh-fidelity-loop.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), color-code (vault/reference/gdd/art-audio/color-code.md), enemy-chaser (vault/reference/gdd/enemies/enemy-chaser.md), agent-3d (vault/reference/agents/agent-3d.md)
- expected_output: Art/Meshes/chaser/SM_Chaser_parts.fbx and SM_Chaser_parts.report.json rebuilt by assemble.py from the six part modules, with the whole-robot renders (front, side, back, three-quarter) and the compare images
- done_when: Criteria C1 to C10 of step 03 pass again on the assembled model (silhouette difference at most 5 % of H per view, proportions, parts and pivots, materials, at most 3000 triangles); overlaps at the joints at most 1 cm; the report keeps the history; committed by name; nothing imported in Unreal
- context: the six part tasks changed their own modules; this step only assembles and validates. If a part does not fit its neighbour at a joint, fix the contract at the joint (say which part) and report; do not rework a whole part.
- criteria: reuse C1 to C10 of task-151-03-chaser-model (same measuring method: orthographic front, side and back renders against the crops, silhouette difference per view, proportions as fractions of H, parts and pivots, materials, total triangles).
- rules: BLENDER ROUTE; no Unreal process; max_returns 3; commit by name with message `task-151-12-chaser-assembly: <summary>`.
- editor_state: closed
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-151-chaser-robot]] — parent global task.

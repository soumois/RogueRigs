---
id: task-151-12-chaser-assembly
description: "Assemble the ten parts and validate the whole chaser against the sheet."
parent: task-151-chaser-robot
kind: task
status: done
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
- known contract fix (decided by the Orchestrator from the thigh report, in line with the owner request of a leg axis at 11 percent of H): in common.py move the knee joint of the contract from y 0.125 H (15.5 cm) to y 0.11 H (13.64 cm) so that ShinFoot (pivot at the knee) lines up with the thigh knee guard (thigh axis at 0.11 H); shift ShinFoot to match (its module takes the knee from the contract: check that its geometry follows), re-run the thigh and shin previews, and note the change in the report and in the doc node chaser-model-pipeline.
- part states to know: HeadTorso C4 (back plates) failed in flat light (geometry is there: 1.9 cm reveal, about 35 degrees; judge it on the three-quarter and back renders and say whether it reads); ShinFoot C3 partial (the foot side has two facet steps, the sheet more); UpperArm was judged only against a mirrored silhouette for the back view: compare its back silhouette with the real back crop; the upper arm module does not draw the dark elbow connector (the forearm module builds it): check the elbow looks right.
- rules: BLENDER ROUTE; no Unreal process; max_returns 3; commit by name with message `task-151-12-chaser-assembly: <summary>`.
- editor_state: closed
- token_ceiling: open
- model: sonnet
- tokens: input 32, cache write 53478, cache read 570049, output 4287
- runs: 1
- milestone: M2
- result: Assembled from the six part modules: 2320 tris, 124 cm, silhouette within 3.2 percent of H, joint overlaps at most 0.93 cm; knee contract moved to 0.11 H; hands, back plates and foot side still coarser than the sheet.

## Relations
- [[task-151-chaser-robot]] — parent global task.

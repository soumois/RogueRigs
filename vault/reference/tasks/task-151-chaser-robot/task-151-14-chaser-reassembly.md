---
id: task-151-14-chaser-reassembly
description: "Reassemble the chaser with the reworked torso and validate joints and silhouettes."
parent: task-151-chaser-robot
kind: task
status: done
---
# Chaser: reassembly with the new torso

- goal: Reassemble the chaser with the reworked torso and validate joints and silhouettes.
- agent: 3d
- depends_on: task-151-13-chaser-torso-pass
- inputs: chaser-model-pipeline (vault/reference/tech/pipelines/chaser-model-pipeline.md), mesh-fidelity-loop (vault/reference/tech/pipelines/mesh-fidelity-loop.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), enemy-chaser (vault/reference/gdd/enemies/enemy-chaser.md), agent-3d (vault/reference/agents/agent-3d.md)
- expected_output: Art/Meshes/chaser/SM_Chaser_parts.fbx and SM_Chaser_parts.report.json rebuilt by assemble.py with the new HeadTorso (eye, logo texture on the hood, reworked back), whole-robot renders, compare images, a joint check
- done_when: the assembled model has the ten objects, four materials and the contract joints; total triangles at most 3000; joint penetrations at most 1 cm (shoulders especially: the torso back, the shoulder pods and the upper arms changed); silhouette difference per view reported against the backup (front 3.09, side 2.72, back 3.21 percent of H); T_ChaserLogo.png is the company emblem texture; committed by name; nothing imported in Unreal
- rules: BLENDER ROUTE; no Unreal process; max_returns 2; do not rework a part (report a mismatch at a joint and fix it through the contract only if it is a placement issue); commit by name with message `task-151-14-chaser-reassembly: <summary>`.
- editor_state: closed
- token_ceiling: open
- model: sonnet
- tokens: input 52, cache write 50267, cache read 925648, output 1569
- runs: 1
- milestone: M2
- result: Reassembled with the new torso: 2648 tris, 123.0 cm tall, ten objects, four slots; shoulder ring penetration 2.52 cm (to fix: shorten the torso ring), side silhouette 6.18 percent (above the 5 percent limit), logo texture referenced.

## Relations
- [[task-151-chaser-robot]] — parent global task.

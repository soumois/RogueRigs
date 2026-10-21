---
id: task-278-06-shield-chaser-body-material
description: "M_ShieldChaser_Anim takes its WPO from MF_RobotAnimWPO and loses the red-mask emissive; instances unchanged."
parent: task-278-chaser-eye-glow
kind: task
status: todo
---
# M_ShieldChaser_Anim takes its WPO from MF_RobotAnimWPO and loses the red-mask emissive; instances unchanged

- goal: M_ShieldChaser_Anim takes its WPO from MF_RobotAnimWPO and loses the red-mask emissive; instances unchanged.
- agent: bp
- depends_on: task-278-05-chaser-body-material
- inputs: bp-shield-chaser (vault/reference/tech/blueprints/enemies/bp-shield-chaser.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md), bp-robot-eye-material (vault/reference/tech/blueprints/enemies/bp-robot-eye-material.md, created by 278-04), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md)
- expected_output: In /Game/RogueRigs/Art/ShieldChaser/M_ShieldChaser_Anim: replace the Custom node ChaserAnimWPO by MF_RobotAnimWPO (AnimTex T_ShieldChaserAnim, ClipOffset and RunRate wired from the existing parameters); remove the emissive chain; keep Visible, OpacityMask and BaseColorTex. Read back ClipOffset on MI_ShieldChaser_Body (0) and BodyPlain (2) and RunRate on all instances; compile, save. bp-shield-chaser and chaser-meshy-pipeline updated with before, after, revert (git checkout 2ad7698a of the .uasset).
- done_when: WPO from the MF, no emissive; instance values read back unchanged; compiles; saved; docs updated; lint clean; committed.
- editor_state: open (editor + MCP)
- token_ceiling: 100k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-278-chaser-eye-glow]] — parent global task.

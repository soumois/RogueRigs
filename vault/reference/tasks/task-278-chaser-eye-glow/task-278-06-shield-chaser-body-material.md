---
id: task-278-06-shield-chaser-body-material
description: "M_ShieldChaser_Anim takes its WPO from MF_RobotAnimWPO and loses the red-mask emissive; instances unchanged."
parent: task-278-chaser-eye-glow
kind: task
status: done
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
- model: sonnet
- tokens: input 34, cache write 45023, cache read 497377, output 764
- runs: 1
- milestone: M1
- result: Done (commit a6bc6e3d). M_ShieldChaser_Anim: WPO = MF_RobotAnimWPO (AnimTex T_ShieldChaserAnim, ClipOffset and RunRate from the existing parameters); emissive chain and GlowAmount (25) removed; Masked, OpacityMask alpha x Visible, BaseColorTex, roughness unchanged; recompile and save without error. Instances unchanged (RunRate 0.7; ClipOffset Body 0, BodyPlain 2; Visible Hidden 0, others 1). Docs bp-shield-chaser and chaser-meshy-pipeline with revert and the eye sync rule. Unverified: render, PIE, instruction count.

## Relations
- [[task-278-chaser-eye-glow]] — parent global task.

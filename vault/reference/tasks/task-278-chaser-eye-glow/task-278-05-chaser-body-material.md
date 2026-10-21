---
id: task-278-05-chaser-body-material
description: "M_Chaser_Meshy_Anim takes its WPO from MF_RobotAnimWPO and loses the red-mask emissive."
parent: task-278-chaser-eye-glow
kind: task
status: todo
---
# M_Chaser_Meshy_Anim takes its WPO from MF_RobotAnimWPO and loses the red-mask emissive

- goal: M_Chaser_Meshy_Anim takes its WPO from MF_RobotAnimWPO and loses the red-mask emissive.
- agent: bp
- depends_on: task-278-04-robot-eye-material
- inputs: bp-chaser (vault/reference/tech/blueprints/enemies/bp-chaser.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md), bp-robot-eye-material (vault/reference/tech/blueprints/enemies/bp-robot-eye-material.md, created by 278-04), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md)
- expected_output: In /Game/RogueRigs/Art/Chaser/M_Chaser_Meshy_Anim: replace the Custom node ChaserAnimWPO by MF_RobotAnimWPO (AnimTex T_ChaserAnim, ClipOffset 0, RunRate = the existing RunRate parameter 0.7); remove the emissive chain (texture x saturate((R - 2 max(G,B)) x 3) x GlowAmount); keep base colour and roughness 0.6. Read back the graph, compile, save. bp-chaser and chaser-meshy-pipeline updated: before (Custom node, emissive GlowAmount 25), after, revert (git checkout 2ad7698a of the .uasset).
- done_when: WPO from the MF, no emissive, RunRate 0.7 read back; compiles without error; saved; docs updated; lint clean; committed.
- editor_state: open (editor + MCP)
- token_ceiling: 100k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-278-chaser-eye-glow]] — parent global task.

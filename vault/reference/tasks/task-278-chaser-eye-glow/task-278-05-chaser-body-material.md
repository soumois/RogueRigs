---
id: task-278-05-chaser-body-material
description: "M_Chaser_Meshy_Anim takes its WPO from MF_RobotAnimWPO and loses the red-mask emissive."
parent: task-278-chaser-eye-glow
kind: task
status: done
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
- model: sonnet
- tokens: input 50, cache write 58595, cache read 992489, output 968
- runs: 1
- milestone: M1
- result: Done (commit c34e717b). M_Chaser_Meshy_Anim: WPO = MF_RobotAnimWPO (AnimTex T_ChaserAnim, ClipOffset Constant 0, RunRate parameter 0.7), no second transform; ClipId CPD 0 and AnimStart CPD 1 now from the function; emissive chain and GlowAmount (25) removed; base colour texture, roughness 0.6, metallic 0 unchanged; recompile without error, no LogMaterial entry for it. Docs bp-chaser and chaser-meshy-pipeline (revert git checkout 2ad7698a of the .uasset). Unverified: render, PIE, instruction count.

## Relations
- [[task-278-chaser-eye-glow]] — parent global task.

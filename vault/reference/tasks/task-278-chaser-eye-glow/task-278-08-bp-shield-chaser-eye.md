---
id: task-278-08-bp-shield-chaser-eye
description: "BP_ShieldChaser uses MI_RobotEye_ShieldChaser on the eye slot and swaps it to the Plain eye when the shield breaks."
parent: task-278-chaser-eye-glow
kind: task
status: in-progress
---
# BP_ShieldChaser uses MI_RobotEye_ShieldChaser on the eye slot and swaps it to the Plain eye when the shield breaks

- goal: BP_ShieldChaser uses MI_RobotEye_ShieldChaser on the eye slot and swaps it to the Plain eye when the shield breaks.
- agent: bp
- depends_on: task-278-07-bp-chaser-eye
- inputs: bp-shield-chaser (vault/reference/tech/blueprints/enemies/bp-shield-chaser.md), bp-robot-eye-material (vault/reference/tech/blueprints/enemies/bp-robot-eye-material.md, created by 278-04), sys-enemies-shield-chaser (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-shield-chaser.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md)
- expected_output: BodyMesh OverrideMaterials element 3 (robot_eye) = MI_RobotEye_ShieldChaser (elements 0-2 kept). OnShieldBroken: add SetMaterial element 3 = MI_RobotEye_ShieldChaserPlain next to the existing element 0 swap (before the debris spawn); nothing else in the graph changes. Read back, compile, save. bp-shield-chaser updated (before, after, revert).
- done_when: Four overrides and the new SetMaterial read back; graph otherwise unchanged; compile without error; saved; doc updated; lint clean; committed.
- editor_state: open (editor + MCP)
- token_ceiling: 100k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-278-chaser-eye-glow]] — parent global task.

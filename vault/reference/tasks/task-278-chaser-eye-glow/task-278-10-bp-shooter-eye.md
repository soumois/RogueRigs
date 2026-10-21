---
id: task-278-10-bp-shooter-eye
description: "BP_Shooter eyes use MI_RobotEye_GreyBox."
parent: task-278-chaser-eye-glow
kind: task
status: todo
---
# BP_Shooter eyes use MI_RobotEye_GreyBox

- goal: BP_Shooter eyes use MI_RobotEye_GreyBox.
- agent: bp
- depends_on: task-278-09-bp-heavy-eye
- inputs: bp-shooter (vault/reference/tech/blueprints/enemies/bp-shooter.md), bp-robot-eye-material (vault/reference/tech/blueprints/enemies/bp-robot-eye-material.md, created by 278-04), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md)
- expected_output: EyeLeft and EyeRight material = MI_RobotEye_GreyBox (was MI_GB_Eye); ShotMesh of BP_ShooterProjectile keeps MI_GB_Eye. Read back, compile, save. bp-shooter updated (before, after, revert).
- done_when: Eye materials read back; compile without error; saved; doc updated; lint clean; committed.
- editor_state: open (editor + MCP)
- token_ceiling: 60k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-278-chaser-eye-glow]] — parent global task.

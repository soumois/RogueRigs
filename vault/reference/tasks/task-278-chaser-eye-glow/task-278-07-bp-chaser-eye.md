---
id: task-278-07-bp-chaser-eye
description: "BP_Chaser BodyMesh uses MI_RobotEye_Chaser on the eye slot."
parent: task-278-chaser-eye-glow
kind: task
status: todo
---
# BP_Chaser BodyMesh uses MI_RobotEye_Chaser on the eye slot

- goal: BP_Chaser BodyMesh uses MI_RobotEye_Chaser on the eye slot.
- agent: bp
- depends_on: task-278-06-shield-chaser-body-material
- inputs: bp-chaser (vault/reference/tech/blueprints/enemies/bp-chaser.md), bp-robot-eye-material (vault/reference/tech/blueprints/enemies/bp-robot-eye-material.md, created by 278-04), sys-enemies-chaser (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-chaser.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md)
- expected_output: BP_Chaser BodyMesh OverrideMaterials: element 0 M_Chaser_Meshy_Anim kept, element 1 (robot_eye) = MI_RobotEye_Chaser. Read back, compile, save. bp-chaser updated (before: one override, eye inside the body material; after; revert).
- done_when: Both elements read back; compile without error; saved; doc updated; lint clean; committed.
- editor_state: open (editor + MCP)
- token_ceiling: 80k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-278-chaser-eye-glow]] — parent global task.

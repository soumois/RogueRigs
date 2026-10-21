---
id: task-278-09-bp-heavy-eye
description: "BP_Heavy eyes use MI_RobotEye_GreyBox, also after the telegraph."
parent: task-278-chaser-eye-glow
kind: task
status: done
---
# BP_Heavy eyes use MI_RobotEye_GreyBox, also after the telegraph

- goal: BP_Heavy eyes use MI_RobotEye_GreyBox, also after the telegraph.
- agent: bp
- depends_on: task-278-08-bp-shield-chaser-eye
- inputs: bp-heavy (vault/reference/tech/blueprints/enemies/bp-heavy.md), bp-robot-eye-material (vault/reference/tech/blueprints/enemies/bp-robot-eye-material.md, created by 278-04), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md)
- expected_output: EyeLeft and EyeRight material = MI_RobotEye_GreyBox (was MI_GB_Eye). OnRecover: the eye SetMaterial calls restore MI_RobotEye_GreyBox instead of MI_GB_Eye; OnTelegraphStart (MI_GB_HeavyGlow) unchanged. Read back, compile, save. bp-heavy updated (before, after, revert).
- done_when: Eye materials and OnRecover targets read back; compile without error; saved; doc updated; lint clean; committed.
- editor_state: open (editor + MCP)
- token_ceiling: 80k
- model: sonnet
- tokens: input 34, cache write 37496, cache read 477516, output 877
- runs: 1
- milestone: M1
- result: Done (commit 0a72c276). BP_Heavy EyeLeft and EyeRight OverrideMaterials [MI_GB_Eye] -> [MI_RobotEye_GreyBox]; OnRecover eye SetMaterial pins MI_GB_Eye -> MI_RobotEye_GreyBox; OnTelegraphStart (MI_GB_HeavyGlow), stripe restore and pitch unchanged; compile without error; saved. Doc bp-heavy (revert git checkout 2ad7698a). Unverified: PIE, render.

## Relations
- [[task-278-chaser-eye-glow]] — parent global task.

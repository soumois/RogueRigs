---
id: task-287-03-bp-shooter-aim-fire
description: "BP_Shooter uses the aim and fire clips (bp)."
parent: task-287-shooter-aim-fire-anim
kind: task
status: in-progress
---
# BP_Shooter uses the aim and fire clips

- goal: Replace T_ShooterAnim with the 287-01 bake (if the import refuses the existing asset, import T_ShooterAnimAim and point MI_Shooter_Body and MI_RobotEye_Shooter to it), extend bounds if the aim pose needs it, move MuzzlePoint to the aimed cannon tip.
- agent: bp
- depends_on: task-287-02-cpp-shooter-aim-fire
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), bp-shooter (vault/reference/tech/blueprints/enemies/bp-shooter.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md), bp-robot-eye-material (vault/reference/tech/blueprints/enemies/bp-robot-eye-material.md); files: T_ShooterAnim.exr and anim report of 287-01
- expected_output: BP_Shooter and its two instances saved; bp-shooter doc updated
- done_when: texture settings, instance parameters and MuzzlePoint read back, compiles without error, doc updated; lint clean
- editor_state: open
- token_ceiling: 80000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-287-shooter-aim-fire-anim]] — parent global task.

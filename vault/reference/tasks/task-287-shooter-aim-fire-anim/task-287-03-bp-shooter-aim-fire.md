---
id: task-287-03-bp-shooter-aim-fire
description: "BP_Shooter uses the aim and fire clips (bp)."
parent: task-287-shooter-aim-fire-anim
kind: task
status: done
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
- model: sonnet
- tokens: input 54, cache write 55514, cache read 971460, output 14316
- runs: 1
- milestone: M2
- result: T_ShooterAnimAim imported (settings read back, old T_ShooterAnim kept), MI_Shooter_Body/MI_RobotEye_Shooter AnimTex T_ShooterAnimAim, RunRate 1.25; MuzzlePoint (45.7,50.4,-17.4) -> (75.57,44.0,21.03) = aimed cannon tip; bounds unchanged (cover aim pose); BP_Shooter compiles.

## Relations
- [[task-287-shooter-aim-fire-anim]] — parent global task.

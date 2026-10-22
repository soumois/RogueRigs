---
id: task-285-03-bp-heavy-slam
description: "BP_Heavy uses the slam clips (bp)."
parent: task-285-heavy-slam-anim
kind: task
status: done
---
# BP_Heavy uses the slam clips

- goal: Replace T_HeavyAnim with the 285-01 bake (if the import refuses the existing asset, import T_HeavyAnimSlam and point MI_Heavy_Body and MI_RobotEye_Heavy to it), extend the mesh bounds for the lean, and remove the BodyMesh pitch 14 of OnTelegraphStart and OnRecover (the clip replaces it, owner 2026-10-22) (eye glow kept).
- agent: bp
- depends_on: task-285-02-cpp-heavy-slam-clips
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), bp-heavy (vault/reference/tech/blueprints/enemies/bp-heavy.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md), bp-robot-eye-material (vault/reference/tech/blueprints/enemies/bp-robot-eye-material.md); files: T_HeavyAnim.exr and anim report of 285-01
- expected_output: BP_Heavy, MI_Heavy_Body, MI_RobotEye_Heavy saved; bp-heavy doc updated
- done_when: texture settings and instance parameters read back, events read back, compiles without error, doc updated; lint clean
- editor_state: open
- token_ceiling: 80000
- model: sonnet
- tokens: input 72, cache write 85563, cache read 1997533, output 20412
- runs: 1
- milestone: M2
- result: T_HeavyAnimSlam imported (settings read back; old T_HeavyAnim kept), MI_Heavy_Body/MI_RobotEye_Heavy(+Glow) AnimTex T_HeavyAnimSlam, RunRate 1.0; SM_Heavy_Meshy_Anim bounds extension (220,220,60) for the lean (estimate); BP_Heavy pitch 14/0 nodes removed, eye glow swap kept; compiles clean.

## Relations
- [[task-285-heavy-slam-anim]] — parent global task.

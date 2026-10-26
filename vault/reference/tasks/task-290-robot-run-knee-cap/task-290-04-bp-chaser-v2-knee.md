---
id: task-290-04-bp-chaser-v2-knee
description: "Import the 290-02 chaser v2 bake as a new texture on the chaser v2 body and eye instances (bp)."
parent: task-290-robot-run-knee-cap
kind: task
status: done
---
# Chaser v2 anim texture in Unreal

- goal: Same as task-290-03 for the chaser v2: new T_ChaserV2AnimRunV2 on MI_ChaserV2_Body and MI_RobotEye_ChaserV2 (T_ChaserV2AnimRunV1 stays); shield chaser v2 untouched.
- agent: bp
- depends_on: task-290-03-bp-heavy-knee
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), bp-robot-meshy-material (vault/reference/tech/blueprints/enemies/bp-robot-meshy-material.md), bp-chaser-v2 (vault/reference/tech/blueprints/enemies/bp-chaser-v2.md); files: task-289-04 node, report of 290-02
- expected_output: T_ChaserV2AnimRunV2; two instances updated; bp-chaser-v2 and bp-robot-meshy-material docs updated
- done_when: texture 20 x 31 and settings read back; AnimTex and RunRate read back equal on both; shield chaser v2 unchanged; docs updated; lint clean
- editor_state: open (owner's editor, MCP route, do not close it)
- token_ceiling: 40000
- model: open
- tokens: not measured: no subagent transcript found (checked 2026-10-26)
- runs: open
- milestone: M2
- result: Cancelled before running (nothing baked or imported): the owner changed his mind on 2026-10-23, replaced by task-291-chaser-v2-v1-anims.

## Relations
- [[task-290-robot-run-knee-cap]] — parent global task.

---
id: task-290-03-bp-heavy-knee
description: "Import the 290-01 heavy bake as a new texture on the heavy body and eye instances (bp)."
parent: task-290-robot-run-knee-cap
kind: task
status: todo
---
# Heavy anim texture in Unreal

- goal: New texture T_HeavyAnimRunV2 from the 290-01 EXR (same settings as T_HeavyAnimRunV1, which stays) set as AnimTex on MI_Heavy_Body, MI_RobotEye_Heavy, MI_RobotEye_Heavy_Glow; RunRate changed only if 290-01 recommends it, all three in sync.
- agent: bp
- depends_on: task-290-02-bake-chaser-v2-knee
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), bp-robot-meshy-material (vault/reference/tech/blueprints/enemies/bp-robot-meshy-material.md), bp-heavy (vault/reference/tech/blueprints/enemies/bp-heavy.md); files: task-289-03 node, report of 290-01
- expected_output: T_HeavyAnimRunV2; three instances updated; bp-heavy and bp-robot-meshy-material docs updated
- done_when: texture 20 x 62 and settings read back; AnimTex and RunRate read back equal on the three instances; no other parameter changed; docs updated; lint clean
- editor_state: open (owner's editor, MCP route, do not close it)
- token_ceiling: 40000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-290-robot-run-knee-cap]] — parent global task.

---
id: task-289-03-bp-heavy-run
description: "Put the 289-01 heavy bake in Unreal and set the new RunRate on its instances (bp)."
parent: task-289-robot-run-v1-motion
kind: task
status: in-progress
---
# Heavy anim texture and RunRate in Unreal

- goal: Reimport the heavy anim texture in place from the 289-01 EXR (headless, editor closed) and set the 289-01 RunRate on the heavy body and eye instances, kept in sync; provisional, owner 2026-10-23.
- agent: bp
- depends_on: task-289-02-bake-chaser-v2-run
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), tool-unreal-reimport (vault/reference/tech/pipelines/tool-unreal-reimport.md), bp-robot-meshy-material (vault/reference/tech/blueprints/enemies/bp-robot-meshy-material.md), robot-parts-bake-pipeline (vault/reference/tech/pipelines/robot-parts-bake-pipeline.md), bp-heavy (vault/reference/tech/blueprints/enemies/bp-heavy.md); files: report of 289-01
- expected_output: T_HeavyAnimSlam (the texture the heavy instances use) reimported from the 289-01 EXR; MI_Heavy_Body, MI_RobotEye_Heavy (and MI_RobotEye_Heavy_Glow if it overrides RunRate) RunRate = 289-01 value; bp-heavy and bp-robot-meshy-material docs updated
- done_when: texture 20 x 62 and settings read back; RunRate read back equal on body and eye; no other parameter changed; docs updated; lint clean
- editor_state: open (owner opened it at 18:48; MCP route, do not close it)
- token_ceiling: 50000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-289-robot-run-v1-motion]] — parent global task.

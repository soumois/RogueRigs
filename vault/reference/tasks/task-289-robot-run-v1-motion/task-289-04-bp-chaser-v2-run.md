---
id: task-289-04-bp-chaser-v2-run
description: "Put the 289-02 chaser v2 bake in Unreal and set the new RunRate on its instances (bp)."
parent: task-289-robot-run-v1-motion
kind: task
status: in-progress
---
# Chaser v2 anim texture and RunRate in Unreal

- goal: Same as task-289-03 for the chaser v2; shield chaser v2 assets untouched.
- agent: bp
- depends_on: task-289-03-bp-heavy-run
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), tool-unreal-reimport (vault/reference/tech/pipelines/tool-unreal-reimport.md), bp-robot-meshy-material (vault/reference/tech/blueprints/enemies/bp-robot-meshy-material.md), robot-parts-bake-pipeline (vault/reference/tech/pipelines/robot-parts-bake-pipeline.md), bp-chaser-v2 (vault/reference/tech/blueprints/enemies/bp-chaser-v2.md); files: report of 289-02
- expected_output: T_ChaserV2Anim reimported; MI_ChaserV2_Body and MI_RobotEye_ChaserV2 RunRate = 289-02 value; bp-chaser-v2 and bp-robot-meshy-material docs updated
- done_when: texture 20 x 31 and settings read back; RunRate read back equal on body and eye; shield chaser v2 instances unchanged; docs updated; lint clean
- editor_state: open (owner opened it at 18:48; MCP route, do not close it)
- token_ceiling: 50000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-289-robot-run-v1-motion]] — parent global task.

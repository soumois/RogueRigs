---
id: task-291-03-bp-chaser-v2-v1-clips
description: "Import the 291-01 bake as a new texture on the chaser v2 body and eye instances, RunRate in sync (bp)."
parent: task-291-chaser-v2-v1-anims
kind: task
status: in-progress
---
# Chaser v2 v1-clips texture in Unreal

- goal: Import the 291-01 EXR as T_ChaserV2AnimV1Clips (T_ChaserV2AnimRunV1 stays, MCP refuses an existing asset), settings TC_HDR, no mips, Nearest, sRGB off, never stream, Clamp; set it as AnimTex on MI_ChaserV2_Body and every chaser v2 eye instance; RunRate from 291-01 on all of them.
- agent: bp
- depends_on: task-291-02-bake-shield-v2-v1-clips
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), bp-chaser-v2 (vault/reference/tech/blueprints/enemies/bp-chaser-v2.md), bp-robot-meshy-material (vault/reference/tech/blueprints/enemies/bp-robot-meshy-material.md); files: vault/reference/tasks/task-289-robot-run-v1-motion/task-289-04-bp-chaser-v2-run.md, report of 291-01
- expected_output: T_ChaserV2AnimV1Clips; instances updated; bp-chaser-v2 and bp-robot-meshy-material updated (before, after, revert)
- done_when: texture 20 x 31 and settings read back; AnimTex and RunRate read back equal on every chaser v2 body and eye instance; shield chaser v2 and v1 untouched; docs updated; lint clean
- editor_state: open (owner editor, MCP route, do not close it)
- token_ceiling: 35000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-291-chaser-v2-v1-anims]] — parent global task.

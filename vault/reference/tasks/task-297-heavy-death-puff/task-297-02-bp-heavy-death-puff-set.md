---
id: task-297-02-bp-heavy-death-puff-set
description: "Set BP_Heavy DeathPuffSystem to NS_HeavyDeathPuff (bp)."
parent: task-297-heavy-death-puff
kind: task
status: done
---
# BP_Heavy uses the heavy puff

- goal: BP_Heavy class default DeathPuffSystem = NS_HeavyDeathPuff (pool size kept); no other change.
- agent: bp
- depends_on: task-297-01-bp-heavy-death-puff-ns
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), bp-heavy (vault/reference/tech/blueprints/enemies/bp-heavy.md); files: report of 297-01
- expected_output: BP_Heavy updated; bp-heavy updated (before, after, revert)
- done_when: DeathPuffSystem read back; compiled and saved; other robot Blueprints unchanged; doc updated; lint clean
- editor_state: open (owner editor, MCP route, do not close it)
- token_ceiling: 20000
- model: sonnet
- tokens: input 50, cache write 36193, cache read 670386, output 7948
- runs: 1
- milestone: M2
- result: BP_Heavy DeathPuffSystem = NS_HeavyDeathPuff (pool 40 kept), read back after save; the 6 other robot Blueprints still NS_RobotDeathPuff; bp-heavy doc updated (c169d219).

## Relations
- [[task-297-heavy-death-puff]] — parent global task.

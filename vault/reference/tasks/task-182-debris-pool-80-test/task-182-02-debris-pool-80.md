---
id: task-182-02-debris-pool-80
description: "Raise the debris pool from 40 to 80 systems on BP_Chaser and BP_ShieldChaser (DebrisPoolSize), puff pool unchanged."
parent: task-182-debris-pool-80-test
kind: task
status: in-progress
---
# Debris pool 80

- goal: Raise the debris pool from 40 to 80 systems on BP_Chaser and BP_ShieldChaser (DebrisPoolSize), puff pool unchanged.
- agent: bp
- depends_on: none
- inputs: bp-chaser (vault/reference/tech/blueprints/enemies/bp-chaser.md), bp-shield-chaser (vault/reference/tech/blueprints/enemies/bp-shield-chaser.md), sys-enemies-base (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-base.md), sys-actor-pool (vault/reference/tech/systems/world/sys-actor-pool.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: DebrisPoolSize 80 on both Blueprint class defaults (DeathPuffPoolSize stays 40); bp-chaser and bp-shield-chaser updated
- done_when: Both defaults read back 80; both Blueprints compiled and saved by explicit path; docs updated; committed by path; editor stays open
- editor_state: open
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M3
- result: open

## Relations
- [[task-182-debris-pool-80-test]] — parent global task.

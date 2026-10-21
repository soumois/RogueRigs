---
id: task-272-02-player-light-low-apply
description: "PlayerLight on BP_Miner (and a shadowless fill light if the sweep chose one) set to the values chosen from the sweep, read back and documented."
parent: task-272-player-light-low-gradient
kind: task
status: todo
---
# Low player light applied

- goal: PlayerLight on BP_Miner (and a shadowless fill light if the sweep chose one) set to the values chosen from the sweep, read back and documented.
- agent: bp
- depends_on: task-272-05-low-light-sweep-r2
- inputs: bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), lights-dark-ambient (vault/reference/tech/blueprints/world/lights-dark-ambient.md), lights-no-shadows (vault/reference/tech/blueprints/world/lights-no-shadows.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: BP_Miner values as listed in the delegation; docs updated with before, after and revert values.
- done_when: Values read back; compiled and saved; docs updated; lint clean; committed by path.
- editor_state: open
- token_ceiling: 40k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-272-player-light-low-gradient]] — parent global task.

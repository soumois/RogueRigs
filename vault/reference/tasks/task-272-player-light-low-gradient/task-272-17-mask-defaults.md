---
id: task-272-17-mask-defaults
description: "MPC_PlayerLight defaults set to the values chosen from the sweep, effect enabled."
parent: task-272-player-light-low-gradient
kind: task
status: in-progress
---
# Mask defaults

- goal: MPC_PlayerLight defaults set to the values chosen from the sweep, effect enabled.
- agent: bp
- depends_on: task-272-16-flat-mask-sweep
- inputs: mpp-player-light-mask (vault/reference/tech/blueprints/world/mpp-player-light-mask.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: MPC_PlayerLight defaults set to the sweep choice; doc updated.
- done_when: Defaults read back, saved, doc updated, lint clean, committed by path.
- editor_state: open
- token_ceiling: 30k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-272-player-light-low-gradient]] — parent global task.

---
id: task-272-12-light-mask-centre-fix
description: "M_PP_PlayerLightMask world position fixed so the mask is centred on the miner (smoke frames showed it 300 px off to the left)."
parent: task-272-player-light-low-gradient
kind: task
status: in-progress
---
# Light mask centre fix

- goal: M_PP_PlayerLightMask world position fixed so the mask is centred on the miner (smoke frames showed it 300 px off to the left).
- agent: bp
- depends_on: task-272-07-light-mask-writer
- inputs: mpp-player-light-mask (vault/reference/tech/blueprints/world/mpp-player-light-mask.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: Material graph with the engine world position reconstruction; doc updated.
- done_when: Material compiles, graph read back, doc updated, committed by path; centring checked by the next sweep (task-272-09).
- editor_state: open
- token_ceiling: 40k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-272-player-light-low-gradient]] — parent global task.

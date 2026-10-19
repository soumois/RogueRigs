---
id: task-234-05-deposit-look-capture
description: "-game captures of Duskite deposits after 234-04, compared with the current clusters."
parent: task-234-ore-deposit-look
kind: task
status: in-progress
---
# -game captures of Duskite deposits after 234-04, compared with the current clusters

- goal: captures of several Duskite deposits (variants and rotations visible) at the game camera and one closer view, side by side with Saved/RockSlab/after4/ spots, plain description.
- agent: cpp
- depends_on: task-234-04-deposit-mesh-assign
- inputs: vault/reference/tech/performance/perf-rock-slab.md (launch lines), vault/reference/agents/agent-cpp.md, vault/reference/tech/systems/core/sys-cheats.md
- expected_output: captures in Saved/OreDeposit/.
- done_when: captures sent to the owner; no game process left.
- editor_state: closed
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-234-ore-deposit-look]] — parent global task.

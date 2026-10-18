---
id: task-230-17-rock-sag-visible-check
description: "Show broken pieces sagging on a partly damaged wall and recapture the 15 and 16 fixes in game."
parent: task-230-rock-slab-integration
kind: task
status: todo
---
# Show broken pieces sagging on a partly damaged wall and recapture the 15 and 16 fixes in game

- goal: a -game capture shows broken pieces on cells damaged but not destroyed, sagging toward the void (owner 2026-10-18, after the 230-12 check: no sag visible in 230-12), plus captures of Duskite crystals and of a rock mass top after 15 and 16. If the sag is not visible, find why (swap, custom data, material) and fix the C++ part or report the material change needed.
- agent: cpp
- depends_on: task-230-16-rock-top-texture-shade
- inputs: vault/reference/tech/systems/world/sys-grid/sys-grid-rock-damage.md, vault/reference/tech/systems/world/sys-grid/sys-grid-rock-piece-data.md, vault/reference/tech/blueprints/world/bp-rock-slab-material.md, vault/reference/tech/performance/perf-rock-slab.md (capture spots), vault/reference/agents/agent-cpp.md (BugItGo then Walk)
- expected_output: captures in Saved/RockSlab/after2/ (close view of a wall with cells at about 30 % and 70 % damage, Duskite, a mass top), side by side with 230-12; fix if needed.
- done_when: captures sent to the owner; any fix built and tested (2 known pre-existing failures); committed.
- editor_state: closed
- token_ceiling: 80000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-230-rock-slab-integration]] — parent global task.

---
id: task-235-01-ore-loot-over-dig
description: "Ore and heal deposits release their loot one by one during the dig, evenly spread over the dig time, instead of all at destruction."
parent: task-235-ore-loot-over-dig
kind: task
status: in-progress
---
# Loot spread over the dig

- goal: Ore and heal deposits release their loot one by one during the dig, evenly spread over the dig time, instead of all at destruction.
- agent: cpp
- depends_on: none
- inputs: ore-blocks (vault/reference/gdd/terrain/ore-blocks.md), heal-ore (vault/reference/gdd/terrain/heal-ore.md), drone (vault/reference/gdd/player/drone.md), sys-grid-deposits (vault/reference/tech/systems/world/sys-grid/sys-grid-deposits.md), sys-pickups (vault/reference/tech/systems/world/sys-pickups.md), sys-pickups-lifecycle (vault/reference/tech/systems/world/sys-pickups/sys-pickups-lifecycle.md), agent-cpp (vault/reference/agents/agent-cpp.md)
- expected_output: A deposit with N pickups (4 now) pops pickup k when its dig progress reaches k / N (so 1.5 s dig and 4 pickups: one every 0.375 s); a deposit destroyed otherwise (explosion) pops the remaining ones at once; progress kept when the dig stops (existing rule); ore and heal both; tests updated; GDD and sys docs updated.
- done_when: build exits 0 editor closed; RogueRigs.* tests pass except the 2 known pre-existing failures; a -game run logs the pickup times during a deposit dig; docs updated.
- editor_state: closed
- token_ceiling: 80000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-235-ore-loot-over-dig]] — parent global task.

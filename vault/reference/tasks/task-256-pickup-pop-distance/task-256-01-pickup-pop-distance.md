---
id: task-256-01-pickup-pop-distance
description: "Ore and heal pickups land close enough to be collected without moving: PopMaxDistance lowered."
parent: task-256-pickup-pop-distance
kind: task
status: done
---
# Ore pickup pop distance

- goal: Ore and heal pickups land close enough to be collected without moving: PopMaxDistance lowered.
- agent: bp
- depends_on: none
- inputs: bp-pickup-ore (vault/reference/tech/blueprints/fx/bp-pickup-ore.md), sys-pickups (vault/reference/tech/systems/world/sys-pickups.md), sys-pickups-lifecycle (vault/reference/tech/systems/world/sys-pickups/sys-pickups-lifecycle.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), heal-ore (vault/reference/gdd/terrain/heal-ore.md)
- expected_output: BP_PickupOre class default PopMaxDistance = 150 (provisional, owner 2026-10-20; was 300; PopMinDistance 80 kept) so a pickup from a block up to 2 cells (134 uu) from the miner rests within the 300 uu pickup radius; read back, compiled, saved; bp-pickup-ore updated.
- done_when: PopMaxDistance reads 150 on the class default object after compile and save; other defaults unchanged; doc updated; lint clean.
- editor_state: open (editor + MCP)
- token_ceiling: 60k
- model: sonnet
- tokens: input 22, cache write 42420, cache read 327226, output 548
- runs: 1
- milestone: M1
- result: BP_PickupOre PopMaxDistance 300 -> 150 (PopMinDistance 80 kept), compiled, saved, read back; bp-pickup-ore updated. Landing in play unverified.

## Relations
- [[task-256-pickup-pop-distance]] — parent global task.

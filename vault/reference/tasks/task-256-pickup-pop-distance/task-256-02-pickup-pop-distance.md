---
id: task-256-02-pickup-pop-distance
description: "Heal pickup pop distance lowered like the ore pickup."
parent: task-256-pickup-pop-distance
kind: task
status: todo
---
# Heal pickup pop distance

- goal: Heal pickup pop distance lowered like the ore pickup.
- agent: bp
- depends_on: task-256-01-pickup-pop-distance
- inputs: bp-pickup-heal (vault/reference/tech/blueprints/fx/bp-pickup-heal.md), sys-pickups (vault/reference/tech/systems/world/sys-pickups.md), sys-pickups-lifecycle (vault/reference/tech/systems/world/sys-pickups/sys-pickups-lifecycle.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), heal-ore (vault/reference/gdd/terrain/heal-ore.md)
- expected_output: BP_PickupHeal class default PopMaxDistance = 150 (provisional, owner 2026-10-20; was 300; PopMinDistance 80 kept); read back, compiled, saved; bp-pickup-heal and the heal-ore GDD pickup line updated.
- done_when: PopMaxDistance reads 150 on the class default object after compile and save; other defaults unchanged; doc updated; lint clean.
- editor_state: open (editor + MCP)
- token_ceiling: 60k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-256-pickup-pop-distance]] — parent global task.

---
id: task-195-02-floaty-pop-values
description: "BP: floaty pop values on BP_PickupXP, BP_PickupOre and BP_PickupHeal (not the chip), docs updated."
parent: task-195-floaty-pop-midair-magnet
kind: task
status: in-progress
---
# Floaty pop values on Blueprints

- goal: set on BP_PickupXP, BP_PickupOre and BP_PickupHeal (owner 2026-10-15: all pickups except the big XP chip BP_PickupXPBig, which keeps today's pop): PopDuration 1.6 s, PopArcHeight 250 uu, PopMinDistance 80 uu, PopMaxDistance 300 uu (ore and heal included: landing on unbroken rock accepted for now), fall twice as slow as the rise (ratio property from task-195-01 = 2), mid-air magnet on with a delay of 1.2 s from spawn. All values provisional, tuned by eye by the owner.
- agent: bp
- depends_on: task-195-01-floaty-pop-midair-magnet
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), bp-pickup-ore (vault/reference/tech/blueprints/fx/bp-pickup-ore.md), bp-pickup-heal (vault/reference/tech/blueprints/fx/bp-pickup-heal.md), sys-pickups-lifecycle (vault/reference/tech/systems/world/sys-pickups/sys-pickups-lifecycle.md)
- expected_output: the three Blueprints compiled and saved by explicit path; bp-pickup-xp, bp-pickup-ore and bp-pickup-heal updated.
- done_when: Blueprints compile; values read back on each class default object; BP_PickupXPBig unchanged; docs updated; lint clean; committed by path.
- editor_state: open
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-195-floaty-pop-midair-magnet]] — parent global task.

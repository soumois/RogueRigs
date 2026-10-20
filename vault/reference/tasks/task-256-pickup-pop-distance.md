---
id: task-256-pickup-pop-distance
description: "Global task: Ore and heal pickups land close enough to be collected without moving: PopMaxDistance lowered."
parent: tasks-done-index
kind: task
status: done
---
# Ore pickup pop distance

- request: lower PopMaxDistance of the mineral and heal pickups so a vein is collected without moving (owner, 2026-10-20)
- goal: Ore and heal pickups land close enough to be collected without moving: PopMaxDistance lowered.
- acceptance: PopMaxDistance reads 150 on BP_PickupOre and BP_PickupHeal; docs and lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-20).
- confirmed_by_owner: yes (2026-10-20)
- milestone: M1
- next_step: none
- result: Done. BP_PickupHeal PopMaxDistance 300 -> 150 (others unchanged), compiled, saved, read back; bp-pickup-heal and heal-ore GDD line updated (provisional, owner 2026-10-20). Landing in play unverified.

## Subtasks
1. task-256-01-pickup-pop-distance — bp
2. task-256-02-pickup-pop-distance — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-256-01-pickup-pop-distance]] — step 1, bp.
- [[task-256-02-pickup-pop-distance]] — step 2, bp.

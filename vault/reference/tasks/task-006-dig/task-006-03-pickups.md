---
id: task-006-03-pickups
description: "Drops from ore and heal ore (one per 1.5 dig-seconds, 3 on destruction) and ARRPickup collected within the miner's PickupRadius."
parent: task-006-dig
kind: task
status: in-progress
---
# Pickups

- goal: ore and heal ore drop pickups the miner collects.
- agent: cpp
- depends_on: task-006-02-drone-dig
- inputs: [[ore-blocks]], [[heal-ore]], [[ore-economy]], [[player-stats]], [[sys-grid]], [[sys-player]], [[cpp-standard]].
- expected_output: `Source/RogueRigs/Pickups/RRPickup.h/.cpp`: `ARRPickup` (abstract) with `ERRPickupType` (Ore, Heal), a mesh root, a short pop on spawn (random horizontal offset within a cell, editable), then each tick: if the player miner is within its `PickupRadius` attribute, fly to it (editable speed) and be collected on reach: log the type on `LogRogueRigs`, broadcast a miner event `OnPickupCollected(Type)`, destroy; no heal or ore effect yet. In `ARRArenaGrid`: editable pickup class per drop type, `DropInterval` 1.5 dig-seconds and `DestroyDropCount` 3 (editable); ore and heal_ore cells drop one pickup each time their accumulated damage crosses a multiple of `DropInterval` (not at the destroying hit itself), and `DestroyDropCount` when destroyed, whatever the source. New doc `sys-pickups` (template `system.md`) linked from [[systems-index]]; [[sys-grid]] and [[sys-player]] updated.
- done_when: build exits 0 with the editor closed; no new warnings in touched files; docs match; lint clean.
- editor_state: closed
- token_ceiling: 60000
- milestone: M1
- result: open

## Relations
- [[task-006-dig]] — parent global task.

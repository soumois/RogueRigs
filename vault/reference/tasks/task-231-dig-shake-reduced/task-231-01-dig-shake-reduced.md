---
id: task-231-01-dig-shake-reduced
description: "The dug block shake is smaller now that the rock look is final: 8 uu horizontal, 3 uu vertical, and it draws on every piece."
parent: task-231-dig-shake-reduced
kind: task
status: todo
---
# Dig shake reduced

- goal: The dug block shake is smaller now that the rock look is final: 8 uu horizontal, 3 uu vertical, and it draws on every piece.
- agent: cpp
- depends_on: none
- inputs: drone (vault/reference/gdd/player/drone.md), sys-grid-rock-damage (vault/reference/tech/systems/world/sys-grid/sys-grid-rock-damage.md), sys-grid-rock-piece-data (vault/reference/tech/systems/world/sys-grid/sys-grid-rock-piece-data.md), bp-rock-slab-material (vault/reference/tech/blueprints/world/bp-rock-slab-material.md), agent-cpp (vault/reference/agents/agent-cpp.md)
- expected_output: ARRArenaGrid ShakeAmplitude 8 and ShakeVerticalAmplitude 3 (were 25 and 8), MI_RockSlab ShakeParams r and g matched (the material mirrors the C++ values; the MI change goes through a headless python script or is reported for the bp agent); check that the shake draws on intact Nanite pieces (task-230-17 saw no WPO under Nanite); -game capture of a wall being dug, before and after.
- done_when: build exits 0 editor closed; RogueRigs.* tests pass except the 2 known pre-existing failures; shake visible in a -game capture and sent to the owner; docs updated.
- editor_state: closed
- token_ceiling: 60000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-231-dig-shake-reduced]] — parent global task.

---
id: task-248-01-drone-no-flight-over-walls
description: "The drone never hovers over a block: escort, dig and hack positions are chosen in free cells with a clear line from the miner."
parent: task-248-drone-no-flight-over-walls
kind: task
status: in-progress
---
# Drone avoids walls

- goal: The drone never hovers over a block: escort, dig and hack positions are chosen in free cells with a clear line from the miner.
- agent: cpp
- depends_on: task-247-01-drone-escort-movement
- inputs: drone (vault/reference/gdd/player/drone.md), sys-drone (vault/reference/tech/systems/combat/sys-drone.md), sys-drone-dig (vault/reference/tech/systems/combat/sys-drone/sys-drone-dig.md), sys-player (vault/reference/tech/systems/core/sys-player.md), sys-grid-api (vault/reference/tech/systems/world/sys-grid/sys-grid-api.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md)
- expected_output: C++ change in Source/RogueRigs/Drone/: candidate escort slots (ideal, +-45, +-90, behind) rejected when their cell is a block (IsBlockCell) or when the segment miner-slot crosses a block; fallback just above the miner; minimum delay between slot changes; dig and hack hover points moved to the free side of the face (about 90 uu from it, 130 uu high) instead of the arc centre; docs updated.
- done_when: Build passes; existing drone tests pass; one position check (headless, no capture): with the miner next to a wall, the drone cell is never a block cell in Orbit, Dig and Hack; docs updated; lint clean.
- editor_state: closed (C++ build)
- token_ceiling: 150k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-248-drone-no-flight-over-walls]] — parent global task.

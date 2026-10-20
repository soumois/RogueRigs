---
id: task-247-01-drone-escort-movement
description: "The drone no longer circles the miner: it holds an escort slot beside and behind the miner that follows the miner velocity, with a damped spring and a slow figure-eight at rest."
parent: task-247-drone-escort-movement
kind: task
status: done
---
# Drone escort movement

- goal: The drone no longer circles the miner: it holds an escort slot beside and behind the miner that follows the miner velocity, with a damped spring and a slow figure-eight at rest.
- agent: cpp
- depends_on: none
- inputs: drone (vault/reference/gdd/player/drone.md), sys-drone (vault/reference/tech/systems/combat/sys-drone.md), sys-drone-dig (vault/reference/tech/systems/combat/sys-drone/sys-drone-dig.md), sys-player (vault/reference/tech/systems/core/sys-player.md), sys-grid-api (vault/reference/tech/systems/world/sys-grid/sys-grid-api.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md)
- expected_output: C++ change in Source/RogueRigs/Drone/ (Orbit state keeps its name): escort slot from the miner velocity (behind and to the side when moving), damped-spring follow, slow figure-eight at rest, base height bobbing; provisional values as EditDefaultsOnly; drone.md (Orbit rule, provisional, owner date), sys-drone and sys-drone-dig updated; existing tests that read the orbit radius, period or height updated.
- done_when: Build passes; existing drone tests pass; one position check (headless, no capture): with the miner moving at constant velocity the drone settles behind it on the movement side, and at rest it stays within the escort distance; docs updated; lint clean.
- editor_state: closed (C++ build)
- token_ceiling: 150k
- model: opus
- tokens: input 98, cache write 171698, cache read 5906146, output 26515
- runs: 1
- milestone: M1
- result: Escort movement built (damped spring, rest figure-eight, bob; heading follows miner velocity). Build OK; DroneEscort position check passes (moving at 600 uu/s: 114.8 uu behind, 71.7 uu to the side; at rest 116.8-136.7 uu from the miner). Docs updated. RogueRigs.Ore.HealVein fails with and without this change (seed), not fixed, needs its own task. Not seen in play.

## Relations
- [[task-247-drone-escort-movement]] — parent global task.

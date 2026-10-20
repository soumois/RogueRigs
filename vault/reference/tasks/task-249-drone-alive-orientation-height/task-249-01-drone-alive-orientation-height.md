---
id: task-249-01-drone-alive-orientation-height
description: "The drone looks alive: height varies, it turns toward a point of interest, tilts with acceleration and makes small idle gestures."
parent: task-249-drone-alive-orientation-height
kind: task
status: in-progress
---
# Drone orientation and height variation

- goal: The drone looks alive: height varies, it turns toward a point of interest, tilts with acceleration and makes small idle gestures.
- agent: cpp
- depends_on: task-248-01-drone-no-flight-over-walls
- inputs: drone (vault/reference/gdd/player/drone.md), sys-drone (vault/reference/tech/systems/combat/sys-drone.md), sys-drone-dig (vault/reference/tech/systems/combat/sys-drone/sys-drone-dig.md), sys-player (vault/reference/tech/systems/core/sys-player.md), sys-grid-api (vault/reference/tech/systems/world/sys-grid/sys-grid-api.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md)
- expected_output: C++ change in Source/RogueRigs/Drone/: height 130-190 uu with two-frequency bobbing; yaw toward a point of interest with delay (dig or hack target, nearest enemy, nearest deposit or objective, else the miner); roll and pitch from miner acceleration; idle gestures every 4 to 8 s (scan sweep, nod to the miner, short dip); visual only, no collision change; provisional values as EditDefaultsOnly; docs updated.
- done_when: Build passes; existing drone tests pass; one position check (headless, no capture): drone height stays inside 130-190 uu above the floor and its yaw faces the dig target during Dig; docs updated; lint clean.
- editor_state: closed (C++ build)
- token_ceiling: 150k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-249-drone-alive-orientation-height]] — parent global task.

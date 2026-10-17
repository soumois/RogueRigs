---
id: task-221-04-xp-cube-capture
description: "cpp: -game capture of big XP data cubes on the floor (any orientation, glow), image for the owner."
parent: task-221-xp-cube-integration
kind: task
status: in-progress
---
# XP cube: in-game capture

- goal: an image of big XP cubes in game.
- agent: agent-cpp
- depends_on: task-221-03-xp-chip-cleanup
- inputs: `vault/reference/agents/agent-cpp.md` (headless game run, rr.Debug.Shots), `vault/reference/tech/systems/world/sys-pickups/sys-pickups-xp.md` (when big XP blocks appear), reference `vault/assets/xp-chip-big.png`.
- method: no C++ change and no build; editor closed. Produce big XP blocks (the existing merge rule or debug commands), capture a frame where several cubes rest on the floor, glow visible. Save `vault/assets/xp-cube-big-ingame.png`. Can run in the same game session as task-222-05.
- done_when: the image shows cubes, not chips (the Orchestrator opens it); no Blueprint error in the log.
- editor_state: closed
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-221-xp-cube-integration]] — parent global task.

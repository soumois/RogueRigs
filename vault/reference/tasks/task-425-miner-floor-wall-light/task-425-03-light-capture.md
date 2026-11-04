---
id: task-425-03-light-capture
description: "Capture: check that the light touches only floor and walls"
parent: task-425-miner-floor-wall-light
kind: task
status: done
---
# Miner light that lights only the floor and the walls: capture

- goal: One -game run on L_ArenaM1: frames with the light on and with FloorWallLight hidden or intensity 0 (same spot, robots near the miner), then check the list.
- agent: capture
- depends_on: task-425-02-miner-point-light
- inputs: agent-capture (vault/reference/agents/agent-capture.md), vault/reference/tasks/task-425-miner-floor-wall-light.md (acceptance)
- expected_output: Report per point seen / not seen / ambiguous with frame or crop paths in Saved/.
- done_when: C1 floor and rock/ore tops near the miner brighter with the light than without. C2 robots, miner, drone, pickups and barrels in the pool unchanged between the two frames. C3 pool radius about 400 uu (about 1/5 of the screen width). C4 floor under the miner below about 205/255. C5 the pool stays on the miner after it moves. Each point reported; ambiguous points go to the owner.
- editor_state: Unreal editor closed (-game run).
- token_ceiling: 0.4 M units
- model: none
- tokens: 0
- usage: none
- runs: 0
- milestone: M2
- started: open
- waiting_for: none
- result: Capture waived on the owner's word (2026-11-04): no run; visual check after task-426.

## Relations
- [[task-425-miner-floor-wall-light]] — parent global task.

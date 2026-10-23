---
id: task-288-16-capture-debris
description: "Capture the new robot deaths (cpp)."
parent: task-288-robot-death-debris
kind: task
status: in-progress
---
# Capture the new robot deaths

- goal: With the editor closed, kill each new robot near the miner (rr.Debug.SpawnNear, crit, slomo 0.25, timed rr.Debug.Shots as in the bp-robot-debris clean recheck) and capture the puff and the thrown pieces, compared with a chaser v1 death.
- agent: cpp
- depends_on: task-288-15-bp-shield-v2-death
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), enemy-death (vault/reference/gdd/enemies/enemy-death.md), sys-enemies-base (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-base.md), bp-robot-debris (vault/reference/tech/blueprints/fx/bp-robot-debris.md); files: the 283-01 capture command
- expected_output: Saved/RobotMeshes/debris/*.png and a description per robot
- done_when: 2 to 4 pieces of the right robot thrown (kamikaze 2 or 3), no glow, pieces gone after about 5 s; the Orchestrator sends the shots to the owner
- editor_state: closed
- token_ceiling: 60000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-288-robot-death-debris]] — parent global task.

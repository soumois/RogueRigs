---
id: task-229-03-pickup-physics-capture
description: "cpp: one -game run with pickups thrown and landed; capture their flight spin and that they rest on faces, not corners."
parent: task-229-pickup-physics
kind: task
status: in-progress
---
# Pickup physics: capture

- goal: images showing pickups tumbling and settled on a face.
- agent: agent-cpp
- depends_on: task-229-02-pickup-physics-cpp
- inputs: `vault/reference/agents/agent-cpp.md`, `vault/reference/tech/systems/core/sys-cheats.md` (rr.Stress.Pickups and other commands), the last capture script `scratchpad\run226.ps1` of the session scratchpad.
- method: ONE game run, editor closed, no build. Spawn XP nuts, big XP cubes and ore chunks around the miner (existing commands), shots during the pop, at landing and about 2 s later; motion blur off for the shots (`r.MotionBlurQuality 0`). Save `vault/assets/pickups-physics-flight.png` and `-rest.png`; report whether any cube rests on a corner or edge.
- done_when: images saved and opened by the Orchestrator; no error in the log; commit by path.
- editor_state: closed
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-229-pickup-physics]] — parent global task.

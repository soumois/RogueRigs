---
id: task-229-03-pickup-physics-capture
description: "cpp: one -game run with pickups thrown and landed; capture their flight spin and that they rest on faces, not corners."
parent: task-229-pickup-physics
kind: task
status: done
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
- model: opus
- tokens: input 60, cache write 73041, cache read 1371811, output 8465
- runs: 1
- milestone: M1
- result: Commit 57311527: vault/assets/pickups-physics-flight.png (a cube changing tilt between shots: light spin visible, random axis per pickup) and pickups-physics-rest.png (cubes on a face, none clearly on a corner; nuts flat or on one of their six flats; glow discs not cut). Roll after landing small (5-25 px, under one cube width), a lower bound: each screenshot froze the game 0.27 s and physics is capped at 1/30 s per frame, so physics ran about 8x slower during the shots. Knob if more roll is wanted: LandingHorizontalSpeedScale (landing horizontal speed about 70 uu/s). No error in the log.

## Relations
- [[task-229-pickup-physics]] — parent global task.

---
id: task-443-03-blueprint-wiring
description: "Bp: BP_ExtractionShuttle feeds the height to the four wash components."
parent: task-443-extractor-ground-blast
kind: task
status: done
---
# Extractor rotor blast on the ground: bp

- goal: BP_ExtractionShuttle feeds the height to the four wash components.
- agent: bp
- depends_on: task-443-02-blast-emitter
- inputs: bp-extraction-shuttle (vault/reference/tech/blueprints/world/bp-extraction-shuttle.md), bp-extractor-rotor-wash (vault/reference/tech/blueprints/fx/bp-extractor-rotor-wash.md)
- expected_output: see done_when
- done_when: Compiles and saves, no Event Tick, PIE on a landed extractor logs no error and reads the expected inputs.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-443
- usage: included in the executor run of task-443
- runs: 1
- milestone: M2
- started: 2026-11-05T20:55:34Z
- waiting_for: none
- result: Event OnHeightAboveFloorChanged(Height) sets User.HeightAboveFloor on WashFL/FR/RL/RR, no Event Tick, compiled, saved. PIE on a landed extractor: Intensity 0.6, FloorHeight 247.46/247.62/298.25/297.55, Height 0, no LogNiagara or Blueprint error. Paused takeoff behind the stats screen (world paused: yes): Height 241.98 then 360 then 400, Intensity 1, GroundBlast particles per rotor from fxdump 32-36 landed down to 12-14 and alpha falling to 0, particles at world Z 8 with horizontal outward velocity, so the sim runs while paused. Defect found and fixed: the last HLSL edit of a stack was not compiled (alpha stuck on its plain value), one more edit after it fixes it. Swirls has the same stale colour: alpha 0 on 60 of 60 particles read, left alone as told, reported.

## Relations
- [[task-443-extractor-ground-blast]] — parent global task.

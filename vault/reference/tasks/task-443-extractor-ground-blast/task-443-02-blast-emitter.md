---
id: task-443-02-blast-emitter
description: "Bp: The floor dust of the rotor wash becomes a stronger outward blast with a height input, none at 400 uu."
parent: task-443-extractor-ground-blast
kind: task
status: done
---
# Extractor rotor blast on the ground: bp

- goal: The floor dust of the rotor wash becomes a stronger outward blast with a height input, none at 400 uu.
- agent: bp
- depends_on: task-443-01-height-signal
- inputs: bp-extractor-rotor-wash (vault/reference/tech/blueprints/fx/bp-extractor-rotor-wash.md), color-code (vault/reference/gdd/art-audio/color-code.md)
- expected_output: see done_when
- done_when: System compiles with no error, no step cutoff at 300 left, strength read as a continuous function of height, bounds cover the blast.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-443
- usage: included in the executor run of task-443
- runs: 1
- milestone: M2
- started: 2026-11-05T20:51:14Z
- waiting_for: none
- result: Emitter DustRing renamed GroundBlast, world space (the old local-space offset was multiplied by the x4 scale and put the ring about 700 uu under the floor, computed): spawned on the floor at Owner.z + 8 - FloorHeight - HeightAboveFloor, disc 40 x4, outward speed 500-800 uu/s, drag 1.5, life 0.8-1.2 s, flat sprites aligned on the velocity. New User.HeightAboveFloor; strength saturate(1 - H/400) on rate 50/s x Intensity and on alpha 0.5 x Intensity written each update; step at 300 removed. Bounds local (-220,-220,-220) to (220,220,30). Compile UpToDate, no error, saved. Look unverified.

## Relations
- [[task-443-extractor-ground-blast]] — parent global task.

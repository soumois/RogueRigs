---
id: task-443-02-blast-emitter
description: "Bp: The floor dust of the rotor wash becomes a stronger outward blast with a height input, none at 400 uu."
parent: task-443-extractor-ground-blast
kind: task
status: in-progress
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-05T20:51:14Z
- waiting_for: none
- result: open

## Relations
- [[task-443-extractor-ground-blast]] — parent global task.

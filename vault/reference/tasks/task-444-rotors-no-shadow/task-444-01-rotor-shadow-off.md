---
id: task-444-01-rotor-shadow-off
description: "Bp: The hull and the four rotors cast no shadow."
parent: task-444-rotors-no-shadow
kind: task
status: done
---
# Extractor rotors cast no shadow: bp

- goal: The hull and the four rotors cast no shadow.
- agent: bp
- depends_on: none
- inputs: bp-extraction-shuttle (vault/reference/tech/blueprints/world/bp-extraction-shuttle.md), Art/Source/extractor-fx/20261105-owner-rotor-shadows-and-streaks.webp, sys-objectives-shuttle (vault/reference/tech/systems/world/sys-objectives/sys-objectives-shuttle.md), lights-no-shadows (vault/reference/tech/blueprints/world/lights-no-shadows.md)
- expected_output: see done_when
- done_when: Flags read back off on the hull and the four rotors of a spawned extractor; compile and save with no error.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-444
- usage: included in the executor run of task-444
- runs: 1
- milestone: M2
- started: 2026-11-05T21:07:22Z
- waiting_for: none
- result: BP_ExtractionShuttle defaults (f6a0408e): CastShadow, bCastDynamicShadow, bCastStaticShadow, bCastContactShadow off on Mesh, RotorFL, RotorFR, RotorRL, RotorRR (before: hull CastShadow on with dynamic already off; rotors all on). Compiled, saved, no Blueprint error. Read back false in PIE on a landed extractor (_C_1) and on the arrival extractor (_C_0), same class for both. Wash and chevrons were already off. Lumen flags (bAffectDynamicIndirectLighting, bAffectDistanceFieldLighting) left on. Look not viewed: no capture, owner validates.

## Relations
- [[task-444-rotors-no-shadow]] — parent global task.

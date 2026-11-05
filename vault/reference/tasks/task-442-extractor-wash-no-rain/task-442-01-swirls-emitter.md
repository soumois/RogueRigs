---
id: task-442-01-swirls-emitter
description: "Bp: The streaks emitter of the rotor wash is replaced by soft faint swirls."
parent: task-442-extractor-wash-no-rain
kind: task
status: done
---
# Extractor rotor wash that does not read as rain: bp

- goal: The streaks emitter of the rotor wash is replaced by soft faint swirls.
- agent: bp
- depends_on: none
- inputs: bp-extractor-rotor-wash (vault/reference/tech/blueprints/fx/bp-extractor-rotor-wash.md), Art/Source/extractor-fx/20261105-owner-rotor-shadows-and-streaks.webp, color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md)
- expected_output: see done_when
- done_when: System compiles with no error, no velocity-aligned thin sprite left in it, User.Intensity still drives the spawn and alpha, count read back within the bound.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-442
- usage: included in the executor run of task-442
- runs: 1
- milestone: M2
- started: 2026-11-05T20:35:50Z
- waiting_for: none
- result: NS_ExtractorRotorWash: emitter Streaks renamed Swirls, sprite Unaligned camera-facing (no velocity-aligned sprite left), new material M_ExtractorRotorSwirl (procedural two-arm spiral, depth fade 120). Spawn User.Intensity x 8/s, lifetime 1.2-1.8 s: at most 14.4 live per rotor at Intensity 1, below the 30 bound, read back. Alpha 0.14 x saturate(User.Intensity) x sin(pi x age), same (0.80, 0.76, 0.68). Compile UpToDate, no error, DustRing read back unchanged; short PIE with no LogNiagara error. Look unverified, the owner checks it in game.

## Relations
- [[task-442-extractor-wash-no-rain]] — parent global task.

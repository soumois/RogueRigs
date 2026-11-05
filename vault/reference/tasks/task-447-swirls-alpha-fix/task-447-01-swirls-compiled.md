---
id: task-447-01-swirls-compiled
description: "Bp: The compiled Swirls emitter gives particles the alpha the asset declares."
parent: task-447-swirls-alpha-fix
kind: task
status: done
---
# Fix the invisible extractor swirls: bp

- goal: The compiled Swirls emitter gives particles the alpha the asset declares.
- agent: bp
- depends_on: none
- inputs: task-442-extractor-wash-no-rain (vault/reference/tasks/task-442-extractor-wash-no-rain.md), task-443-extractor-ground-blast (vault/reference/tasks/task-443-extractor-ground-blast.md), bp-extractor-rotor-wash (vault/reference/tech/blueprints/fx/bp-extractor-rotor-wash.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: see done_when
- done_when: Particle dump in PIE: Swirls alpha above 0 and equal to the expression value at the current Intensity; GroundBlast values unchanged; no LogNiagara error; dump figures given in the result.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-447
- usage: included in the executor run of task-447
- runs: 1
- milestone: M2
- started: 2026-11-05T22:22:52Z
- waiting_for: none
- result: Done (5c65afc0). Cause confirmed: compiled Swirls Particles.Color kept alpha 0 (dump before fix: 24 of 24 Swirls particles A 0, RGB 0.8/0.76/0.68) while the read-back showed the task-442 expression. Fix: Particles.Color set to a plain value, then the same expression, saved; editor restarted to prove the saved compiled script. PIE after restart, landed exit extractor, Intensity 0.6, Height 0: 30 Swirls particles over 4 rotors, A 0.0027 to 0.0840 (cap 0.14 x 0.6 = 0.084), max error vs 0.14 x saturate(I) x sin(pi x age) 0.00000; size SwirlSize 222.9-317.4 x (0.7 + 0.6 age), error 0.001. GroundBlast 27-30 per rotor, A max 0.300, Z 8, error vs its expression 0.00000. Paused extraction takeoff (rr.Run.EndLevel): Intensity 1, Swirls A max 0.1396; GroundBlast A max 0.389 at Height 88.9 and 0.198 at 241.98, error 0.00000. No LogNiagara error or warning.

## Relations
- [[task-447-swirls-alpha-fix]] — parent global task.

---
id: task-542-01-fix-edges
description: "Bp: Find the ground particle effect of image 2 and remove its visible sprite edges"
parent: task-542-ground-particle-edges
kind: task
status: done
---
# Ground particles: no visible sprite edges: bp

- goal: Find the ground particle effect of image 2 and remove its visible sprite edges
- agent: bp
- depends_on: none
- inputs: color-code (vault/reference/gdd/art-audio/color-code.md), Saved/Scratch/owner-ui-batch-2026-11-13/2.webp
- expected_output: see done_when
- done_when: A1 to A4 hold.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-542
- usage: included in the executor run of task-542
- runs: 1
- milestone: M2
- started: 2026-11-13T23:15:05Z
- waiting_for: none
- result: Done (80bd4cad). The ground particles of image 2 are NS_ExtractorGroundWind (task-466 partial state, cancelled, no doc node), not the GroundBlast of NS_ExtractorRotorWash. Cause: the Custom node of M_ExtractorGroundWind fed the Shape sample with frac(UV) per atlas cell; at the sprite border the interpolated UV crosses 0 or 1 inside a 2x2 pixel quad, frac jumps, the UV derivative explodes and the GPU samples the smallest mip (average beige, non-zero alpha), drawing dotted lines on the four quad edges. Fix: saturate(UV) instead of frac(UV), identical inside the quad, so colour, size, timing and count unchanged; shared by the four MI_GroundWind_* (Disc, Puffs, Ring, Streaks), so every use of the effect (arrival and exit extractor) is fixed (A2). Diagnosis tries on M_ExtractorBlastWind (unreferenced) reverted. Headless Python edit, no editor session.

## Relations
- [[task-542-ground-particle-edges]] — parent global task.

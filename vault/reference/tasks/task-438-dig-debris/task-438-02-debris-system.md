---
id: task-438-02-debris-system
description: "Bp: The one-shot dig debris Niagara system exists and is the one the game plays."
parent: task-438-dig-debris
kind: task
status: done
---
# Debris when digging: bp

- goal: The one-shot dig debris Niagara system exists and is the one the game plays.
- agent: bp
- depends_on: task-438-01-trigger-on-break
- inputs: bp-dig-impact (vault/reference/tech/blueprints/fx/bp-dig-impact.md), color-code (vault/reference/gdd/art-audio/color-code.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md)
- expected_output: see done_when
- done_when: System compiles with no error, one-shot, at most 12 sprites and 0.8 s read back, colour from the parameter the code sets.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-438
- usage: included in the executor run of task-438
- runs: 1
- milestone: M2
- started: 2026-11-05T19:09:45Z
- waiting_for: none
- result: NS_DigDebris from NS_ShotImpactSpark: CPU, Loop Once, burst 12, lifetime 0.55 to 0.8, colour linked to User.ChipColor, cone axis from User.ThrowDirection, M_DigChip, sizes 9 to 22 uu. Compile UpToDate, 0 errors, values read back. Dig.DigDebrisAsset under -RenderOffScreen: 6 bursts played, back in the pool after 48 frames (0.8 s). Dig and Ore pass with the shipped asset. Look not seen: no capture in this task, owner validates.

## Relations
- [[task-438-dig-debris]] — parent global task.

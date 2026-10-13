---
id: perf-stress-damage-numbers-recheck
description: "Damage numbers recheck after the per-digit rebuild (task-025-06): -game screenshot findings, particle dump, remaining readability issues, S2 100/400 on vs off."
parent: perf-stress-damage-numbers
kind: doc
---
# Damage numbers recheck (2026-10-05, task-025-06)

Method: as [[perf-stress-damage-numbers]]; 30 robots, `-Mode fire`, `rr.Debug.CritChance 0.5`, `rr.Debug.Shots` at 3–15 s real time (one run at `slomo 0.5` with `fx.DumpParticleData 1`, one at normal speed). Kept: `Saved/Screenshots/WindowsEditor/V6_01.png`, `V6_02.png`, `V6T_03.png` (not committed).

## Confirmed
- Multi-digit values read left to right from the top-down camera: "10", "20", "40", "25", "35" ([[sys-damage-numbers-niagara]] works as specified).
- Dump: `DNIdx` 0, 1 within each number, `DNDigit` / `SubImageIndex` match the pushed digits (15 → 1, 5), second digit at +59.5 uu along Y, crit size 96, colour (1, 0.8, 0.3) after the white flash.
- Continuous: pushes and visible numbers up to the end of the run (batch 36 at 17 s; shot at 15 s shows three numbers). Particles spawned = 2 × numbers pushed at every shot (8/4, 14/7, 26/13, 90/45): no double spawn, no missing digit. Earlier defects 1–4 are fixed.

## Remaining issues
1. Crit colour reads pale cream, not gold (pixel ≈ (235, 225, 180) on the grey floor; nearly invisible over a yellow robot). Cause: `CriticalColor` is linear; (1, 0.8, 0.3) linear is ≈ (255, 231, 149) sRGB before tonemapping. Options: a linear value such as (1, 0.55, 0.08) (owner decision, config `CriticalColor`), and/or a dark outline or drop shadow in `T_DamageDigits` / `M_DamageNumber` (bp agent) for contrast on yellow robots.
2. Numbers of neighbouring robots overlap in a dense crowd (35, 20 and 25 merged into one cluster): a crit number is ≈ 155 uu wide, 96 uu tall, robots are ≈ 100 uu apart and `Jitter` is 25 uu. Not a same-target stack (merge works). Fix is in C++ layout (e.g. per-number vertical offset when an open number is close), owner decision.

## S2 continuous fire, editor -game, settle 5 s, measure 20 s (frame avg / p99, game avg / p99, GPU avg, ms)
| Robots | Numbers on, run 1 | Numbers on, run 2 | Numbers off (`NumberSystem=None`) |
|---|---|---|---|
| 100 | 26.41 / 30.30, 7.90 / 11.54, 22.93 | 26.64 / 32.25, 6.34 / 9.82, 23.44 | 25.63 / 31.12, 6.95 / 11.65, 22.21 |
| 400 | 29.01 / 34.26, 12.75 / 26.22, 25.58 | 28.25 / 33.53, 10.50 / 15.95, 24.97 | 27.52 / 33.28, 11.52 / 19.61, 24.19 |

- Game thread: off lies between the two on runs (±1.2 ms noise): no measurable cost. Run 2: 214 numbers, 504 digits in 45 s.
- GPU bound (22–26 ms, higher than task-025-03 on the same machine): on vs off GPU +0.7–1.4 ms, within noise of one run each.
- Cause of the 22–26 ms (task-029-01): not the project. A third-party game (`GraveyardKeeper2.exe`) ran in the background from 14:19 (≈ 27 % of the GPU 3D engine, ≈ 2.4 CPU cores, GPU at 82 °C); the task-025-03 and 025-06 runs came after, the [[perf-stress-grid-cells]] runs (11:18–11:28) before. Bisect with the same load: the task-023-01 commit (a416610, built in a worktree) gives S0 / S2 100 15.1 / 14.0 ms (GPU 12.4 / 11.0), HEAD 16.2 / 13.8 ms (GPU 13.4 / 11.1), versus 8.4–9.0 ms when measured at 023-01. No project or asset fix; measure with other games and GPU apps closed. Confirmed once it closed: the same commit gives S0 / S2 100 8.50 / 8.88 ms (GPU 7.5 / 7.7), [[perf-stress-grid-cells]] after section.

## Relations
- [[perf-stress-damage-numbers]] — parent: first check and the defects this recheck closes.
- [[bp-damage-numbers]] — asset the readability fix applies to.
- [[sys-damage-numbers]] — `CriticalColor`, `Jitter` and layout tunables.

---
id: stress-damage-numbers
description: "Niagara damage numbers check (task-025-03): -game screenshot findings, asset defects with fixes for the bp agent, S2 100/400 numbers on vs off."
parent: performance-index
kind: doc
---
# Damage numbers: visual check and S2 cost (2026-10-05, task-025-03)

Method: editor `-game` 1920×1080 on L_ArenaM1, `Run-StressTest.ps1 -Mode fire`, `-PreExec "rr.Debug.CritChance 0.5,rr.Debug.Shots 9 8 0.25 Crit,slomo 0.25"` ([[sys-stress]]); particle data from `fx.DumpParticleData 1` and pushes from the `LogRogueRigs` Verbose line ([[sys-damage-numbers]]).

## What the screenshots show (after the C++ reactivation fix)
- Digits appear above hit robots (spawn at capsule top, z ≈ 166), upright and readable for the top-down camera, rise 60 uu and fade over the last 45 % of 0.7 s (dump: z 166 → 222, alpha 1 → 0.07). Crits are about 1.6× larger with a +15 % pop; their colour (1, 0.85, 0.45) reads as pale beige on the grey floor rather than clearly white-gold.
- Only ONE digit per number shows: 10 → "1", 30 → "3". Numbers of the same batch stack on the first number.

## Asset defects (NS_DamageNumbers, [[bp-damage-numbers]]) and fixes for the bp agent
1. Emitter completes after its first loop (emitter ticks only on frames 1–50 of a 400-frame run): nothing rendered after about 1 s. Fix: Emitter State loop behaviour Infinite (and system state not completing), so the persistent component never finishes. C++ now reactivates a completed component on the next push as a fallback, but numbers pushed while the emitter is past its loop and still holding particles are lost.
2. `Engine.ExecutionIndex` reads 0 for every particle in Particle Spawn (dump: DNIdx 0, DNSlot 0 for all): every particle is digit slot 0 of number 0. Fix: read the index inside a scratch/HLSL module of Particle Spawn (or a spawn-index attribute set there), then check with `fx.DumpParticleData 1` that DNIdx and DNSlot vary.
3. Spawn count is 4 per number, not 5 (2 numbers → 8, 1 → 4; integration test with a renderer: 8 for 2): fix the `NewCount * 5` expression.
4. About 1 batch in 4 spawns twice (15 numbers → 19 batches of particles): `NewCount` is a level reset to 0 one tick later, and Niagara sometimes reads it on two frames. Fix suggestion: spawn on a change of a batch id (C++ would add an incrementing `User.DamageNumberBatchId`; contract change to be agreed before either side moves).
5. Crit colour: raise saturation (e.g. (1, 0.8, 0.3)) or add a dark outline in M_DamageNumber; normal digits (≈ 25 px tall at 1080p) are small but legible.

## S2 continuous fire, editor -game, settle 5 s, measure 20 s (ms avg / p99 / max)
| Robots | Numbers on: frame | game | GPU avg | Numbers off (`NumberSystem=None`): frame | game | GPU avg | [[stress-opt-m1]] game avg |
|---|---|---|---|---|---|---|---|
| 100 | 18.91 / 39.03 / 47.5 | 8.20 / 12.57 | 15.67 | 15.37 / 21.86 / 25.1 | 8.21 / 13.09 | 12.46 | 5.20 |
| 400 | 18.58 / 27.75 / 28.6 | 15.49 / 22.88 | 16.02 | 20.06 / 27.52 / 1399 (shot) | 16.09 / 25.37 | 15.54 | 12.30 |

- Game thread: no regression with numbers on (+0.0 ms at 100, −0.6 ms at 400, within run noise); acceptance (3) of [[task-025-damage-numbers]] holds on the game thread. The actor damage text of [[stress-opt-m1]] no longer exists, so the same-build on/off pair is the comparison; the gap to stress-opt-m1 comes from later tasks (weapons, fine grid), not from the numbers.
- Frame is render/GPU bound in this build (GPU 12–16 ms); the 100 robots on/off GPU gap (3 ms) did not repeat at 400 and is treated as noise. Rendering was cheap because of defects 2–3; recheck once the asset is fixed (5 sprites per number at most 64 numbers per tick).

## Relations
- [[performance-index]] — parent index.
- [[bp-damage-numbers]] — the asset the defects apply to.
- [[sys-damage-numbers]] — the subsystem and contract measured.
- [[stress-opt-m1]] — earlier S2 reference.

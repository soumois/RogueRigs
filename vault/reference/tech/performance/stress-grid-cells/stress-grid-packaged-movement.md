---
id: stress-grid-packaged-movement
description: "Packaged build and spike causes (068-01) and robot light movement (071-01), 2026-10-10. Open for editor versus packaged and movement cost."
parent: stress-grid-cells
kind: doc
---
# Grid stress — packaged movement

## Packaged build and spike causes (task-068-01, 2026-10-10)
Staged Development build ([[build-toolchain]]; Test cannot be built with the launcher engine: "Targets cannot be built in the Test configuration with this engine distribution"), HEAD 0525a68, same commands as task-066-02 (S0 100+400, dig 400) and task-067-01 (game 250, `rr.Pacing.RunLookahead 0`), one launch each, all runs within 11 min. Owner's normal profile, nothing closed. ms avg / p99 / max (game avg); game spikes = frames > 25 ms (wave / GC / untagged).

| Run | Packaged | Editor `-game` | Packaged, traced |
|---|---|---|---|
| S0 100 | 8.45 / 9.70 / 14.9 (3.16) | 8.93 / 11.07 / 18.1 (5.15) | 9.01 / 10.73 / 22.4 (3.93) |
| S0 400 | 9.18 / 10.48 / 13.1 (6.06) | 10.34 / 12.94 / 15.3 (9.10) | 10.05 / 11.72 / 13.5 (7.37) |
| Dig 400 | 9.77 / 11.28 / 12.7 (6.12) | 9.87 / 12.00 / 17.9 (8.84) | 10.07 / 11.49 / 12.4 (7.28) |
| Game 250 | 11.27 / 22.31 / 44.3 (10.42); 20 spikes (8 / 0 / 12) | 14.05 / 30.14 / 84.2 (13.96); 93 (64 / 1 / 28) | 12.46 / 23.33 / 34.9 (12.11); 32 (28 / 0 / 4) |

- Packaged vs editor: game thread −2.0 ms (S0 100) to −3.5 ms (game 250); frames render bound in S0 and dig (draw ≈ frame), so frame gains are 0.1–1.2 ms there; game 250 p99 30.1 → 22.3 ms, under the 25 ms budget.
- S0 100 21 ms frame (task-066-02): reproduced in the trace (22.4 ms, game work 3.4 ms). The game thread waits 18 ms in `Sync_RenderingThread`; the render thread waits 14.5 ms on the RHI occlusion fence; `FD3D12Queue::ExecuteCommandLists` blocks 17.7 ms and the GPU frame stretches to 22 ms (TSR 14 ms vs 2.3). Cause: GPU/driver submission stall outside the game; no GC, PSO, shader or loading event.
- Dig 400 57.9 ms frame (task-066-02): not reproduced in 3 runs (max 12.4–17.9 ms); its game time was ≤ 15.8 ms, same signature as the S0 stall.
- Game 250 spikes, untagged ones explained ([[perf-trace-real-game]]): robot waves (0.74 ms per robot spawned) and XP bursts (robots killed together drop all their cubes on one frame, 195 cubes and 67 destroys = 34.9 ms). Packaged GC: 5.3 ms then 2.1 ms purge on 20 frames, never a spike; the 47–60 ms GC frames of task-067-01 are editor-only.
- Untraced runs also show 5–8 consecutive render-bound frames (draw 22–32 ms, GPU normal) early in the game measure; the trace shows render-thread frames up to 18 ms from RHI fence waits and ray-tracing instance gathering (3.5 ms), cause not isolated.

## Robot light movement (task-071-01, 2026-10-10)
Editor `-game`, same commands as task-067-01 (game 250, settle 30, measure 60, `rr.Pacing.RunLookahead 0`) and task-066-02 (S0 400). Before = 3ad37c9, after = b28e4bf (`rr.Enemy.LightMovement` 1, [[sys-enemies-movement]]), same session within 30 min, owner's normal profile, nothing closed. ms avg / p99 / max (game avg / p99); movement = Insights `-statnamedevents` trace of a third game 250 run, inclusive ms per frame of `UCharacterMovementComponent_TickComponent` before, `URRRobotMovementComponent_Tick` after (engine-path fallback 0.21 included).

| Run | Before | After |
|---|---|---|
| Game 250 #1 | 17.18 / 37.02 / 85.0 (17.14 / 37.02); 140 spikes (80 fill) | 14.79 / 32.67 / 67.5 (14.72 / 32.67); 100 (70) |
| Game 250 #2 | 17.24 / 38.98 / 109.4 (17.16 / 39.02); 188 (80) | 13.43 / 29.81 / 98.3 (12.76 / 30.40); 75 (51) |
| Game 250 traced | 16.12 / 35.20 / 94.9 (16.06 / 35.76) | 15.64 / 32.96 / 76.1 (15.48 / 33.53) |
| S0 400 | 10.40 / 13.03 / 15.0 (9.12 / 12.49) | 10.24 / 12.67 / 15.8 (7.84 / 10.23) |
| Movement ms / frame (traced, ≈ 205 robots) | 5.56 (PhysWalking 2.08, FindFloor 0.30) | 4.39 |

- Game thread at 250: −2.4 to −4.4 ms avg, −4.4 to −8.6 ms p99 untraced (run-to-run spread ≈ 2 ms, robots alive 203–210 in every run); S0 400 −1.3 ms avg, −2.3 ms p99. Frames stay game-thread bound at 250; S0 400 frames are render bound (draw ≈ frame).
- Movement scope −1.2 ms per frame traced: walking physics, floor search and the separate rotation move are gone; what remains is the swept move itself (child transforms of the parts, `HitCapsule` overlap update), ≈ 21 µs per robot.
- Spikes: fewer frames over 25 ms (75–100 vs 140–188), same causes (wave fill, XP bursts, one GC); max frame unchanged in kind.
- Kills 860–1064 per run (before 940–1099): robots still reach the miner around rock (also `RogueRigs.Enemies.LightMovementAroundRock`). Miner damage is not logged: the pacing harness keeps the miner invulnerable.

## Relations
- [[stress-grid-cells]] — parent hub: summary and verdict.
- [[build-toolchain]] — referenced above.
- [[perf-trace-real-game]] — referenced above.
- [[sys-enemies-movement]] — referenced above.
